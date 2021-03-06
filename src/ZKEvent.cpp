#include "ZKEvent/ZKEvent.h"
#include "ZKClient.h"
#include "Event.h"
#include "DebugLog.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include <stdexcept>


using namespace detail;

#define MAX_EPOLL_EVENT (128)

ZKEvent::ZKEvent(const std::vector<std::string>& servers, int timeout)
    : running_(true),
      timeout_(timeout),
      events_(MAX_EPOLL_EVENT),
      id_(0),
      epoll_fd_(0),
      wakeup_fd_(0),
      wakeup_event_(nullptr)
{
    for (size_t i = 0; i < servers.size(); ++i) {
        if (!servers_.empty()) {
            servers_.push_back(',');
        }
        servers_.append(servers[i]);
    }

    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ == -1) {
        LOG_DEBUG("poll_create1 error %d", errno);
        throw std::runtime_error("poll_create1 error");
    }

    wakeup_fd_ = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (wakeup_fd_ == -1) {
        LOG_DEBUG("eventfd error %d", errno);
        throw std::runtime_error("eventfd error");
    }

    wakeup_event_ = new Event(wakeup_fd_);
    wakeup_event_->enable_reading();
    wakeup_event_->set_reading_callback([this]() {
        uint64_t n;
        if (::eventfd_read(wakeup_fd_, &n) < 0) {
            LOG_DEBUG("eventfd_read error %d", errno);
        }
    });

    register_event(wakeup_event_);

}

ZKEvent::~ZKEvent()
{
    if (wakeup_fd_) close(wakeup_fd_);

    if (wakeup_event_) {
        delete (wakeup_event_);
    }

    if (epoll_fd_) ::close(epoll_fd_);
}

void ZKEvent::loop()
{
    id_ = pthread_self();
    LOG_DEBUG("id = %lu", id_);
    std::vector<detail::Event*> events;

    while (running_) {
        events.clear();
        int ret = pull_events(events);
        if (ret == -1) {
            break;
        }

        for (size_t i = 0; i < events.size(); ++i) {
            events[i]->handle_events();
        }

        std::vector<Callback> callbacks(0);

        {
            std::lock_guard<std::mutex> guard(lock_);
            std::swap(callbacks, pending_callback_);
        }

        for (size_t i = 0; i < callbacks.size(); ++i) {
            callbacks[i]();
        }

        while (!task_queue_.empty()) {
            task_queue_.front()();
            task_queue_.pop_front();
        }
    }
}

void ZKEvent::start_connect()
{
    post_callback([this]() {
        LOG_DEBUG("start connect");
        client_ = std::make_shared<detail::ZKClient>(this);
    });
}

void ZKEvent::get(const std::string& path, const StringCallback& cb)
{
    post_callback([this, path, cb]() {
        if (client_) {
            client_
                ->get(path, 0, [cb](const Status& status, const struct Stat* zk_state, const Slice& data) {
                    cb(status, data);
                });
        }
        else {
            cb(Status::io_error("not connected"), Slice());
        }
    });
}

void ZKEvent::set(const std::string& path, const std::string& data, const VoidCallback& cb)
{
    post_callback([this, path, data, cb]() {
        if (client_) {
            client_->set(path, data, -1, cb);
        }
        else {
            cb(Status::io_error("not connected"));
        }
    });
}

void ZKEvent::create(const std::string& path, const std::string& data, int flag, const StringCallback& cb)
{
    post_callback([this, path, data, flag, cb]() {
        if (client_) {
            client_->create(path, data, flag, [cb](const Status& status, const Slice& data) {
                cb(status, data);
            });
        }
        else {
            cb(Status::io_error("not connected"), Slice());
        }
    });
}

void ZKEvent::exists(const std::string& path, const ExistsCallback& cb)
{
    post_callback([this, path, cb]() {
        if (client_) {
            client_->exists(path, 0, cb);
        }
        else {
            cb(Status::io_error("not connected"), false);
        }
    });
}

void ZKEvent::del(const std::string& path, const VoidCallback& cb)
{
    post_callback([this, path, cb]() {
        if (client_) {
            client_->del(path, -1, [cb](const Status& status) {
                cb(status);
            });
        }
        else {
            cb(Status::io_error("not connected"));
        }
    });
}

void ZKEvent::children(const std::string& path, const StringsCallback& cb)
{
    post_callback([this, path, cb]() {
        if (client_) {
            client_->children(path, 0, cb);
        }
        else {
            cb(Status::io_error("not connected"), nullptr);
        }
    });
}

void ZKEvent::subscribe_data_changes(const std::string& path, const StringCallback& cb)
{
    post_callback([this, path, cb]() {
        DataChangesContextPtr ctx(new DataChangesContext);
        ctx->cb = cb;
        ctx->zxid = 0;
        data_ctx_[path] = ctx;

        do_subscribe_data_changes(path, ctx);
    });
}

void ZKEvent::subscribe_child_changes(const std::string& path, const ChildEventCallback& cb)
{
    post_callback([this, path, cb]() {
        ChildChangesContextPtr ctx(new ChildChangesContext);
        ctx->cb = cb;
        ctx->children = std::make_shared<std::set<std::string>>();
        child_ctx_[path] = ctx;

        do_subscribe_child_changes(path, ctx);
    });
}

void ZKEvent::post_callback(const Callback& cb)
{
    if (id_ == pthread_self()) {
        task_queue_.push_back(cb);
    }
    else {
        std::lock_guard<std::mutex> guard(lock_);
        pending_callback_.push_back(cb);
        wakeup();
    }
}

int ZKEvent::pull_events(std::vector<detail::Event*>& events)
{
    int n_events = epoll_wait(epoll_fd_, events_.data(), events_.size(), 1000);
    if (n_events == -1) {
        LOG_DEBUG("epoll_wait error %d", errno);
        return n_events;
    }
    for (int i = 0; i < n_events; ++i) {
        Event* ev = (Event*) events_[i].data.ptr;
        ev->ready_ops(events_[i].events);
        events.push_back(ev);
    }
    return n_events;
}

void ZKEvent::register_event(detail::Event* e)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.ptr = e;
    event.events = e->interest_ops();

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, e->fd(), &event) != 0) {
        LOG_DEBUG("epoll_ctl error %s", strerror(errno));
    }
}

void ZKEvent::wakeup()
{
    if (::eventfd_write(wakeup_fd_, 1) < 0) {
        LOG_DEBUG("eventfd_write error %d", errno);
    }
}

void ZKEvent::on_connected()
{
    if (connected_cb_) {
        connected_cb_();
    }
}

void ZKEvent::on_data_changes(const std::string& path)
{
    post_callback([path, this]() {
        auto it = data_ctx_.find(path);
        if (it == data_ctx_.end()) {
            return;
        }
        DataChangesContextPtr ctx = it->second;
        do_subscribe_data_changes(path, ctx);
    });
}

void ZKEvent::on_child_changes(const std::string& path)
{
    post_callback([path, this]() {
        auto it = child_ctx_.find(path);
        if (it == child_ctx_.end()) {
            return;
        }
        ChildChangesContextPtr ctx = it->second;
        do_subscribe_child_changes(path, ctx);
    });
}

void ZKEvent::do_subscribe_data_changes(const std::string& path, detail::DataChangesContextPtr ctx)
{
    if (!client_) {
        ctx->cb(Status::io_error("not connected"), std::string());
        return;
    }
    client_->get(path, 1, [path, this, ctx](const Status& status, const struct Stat* zk_state, const Slice& data) {
        if (!status.is_ok()) {
            ctx->cb(status, std::string());
            return;
        }

        //success
        Slice slice(ctx->data);

        ctx->zxid = zk_state->mzxid;

        if (slice != data) {
            ctx->data = data.to_string();
            ctx->cb(status, ctx->data);
        }
    });
}

void ZKEvent::do_subscribe_child_changes(const std::string& path, detail::ChildChangesContextPtr ctx)
{

    if (!client_) {
        ctx->cb(Status::io_error("not connected"), ChildEventErr, std::string());
        return;
    }


    client_->children(path, 1, [path, this, ctx](const Status& status, StringSetPtr strings) {
        if (!status.is_ok()) {
            ctx->cb(status, ChildEventErr, std::string());
            return;
        }

        //success
        for (auto it = strings->begin(); it != strings->end(); ++it) {
            if (ctx->children->find(*it) == ctx->children->end()) {
                ctx->cb(status, ChildEventAdd, *it);
            }
        }

        for (auto it = ctx->children->begin(); it != ctx->children->end(); ++it) {
            if (strings->find(*it) == strings->end()) {
                ctx->cb(status, ChildEventDel, *it);
            }
        }

        ctx->children = strings;
    });
}