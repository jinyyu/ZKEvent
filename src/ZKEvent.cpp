#include "ZKEvent/ZKEvent.h"
#include "Event.h"
#include "DebugLog.h"
#include <unistd.h>
#include <sys/eventfd.h>


#define MAX_EPOLL_EVENT (128)

ZKEvent::ZKEvent(const std::vector<std::string>& servers, int timeout)
    : timeout_(timeout),
      id_(0),
      running_(true),
      epoll_fd_(0),
      events_(MAX_EPOLL_EVENT)
{
    for (int i = 0; i < servers.size(); ++i) {
        if (!servers_.empty()) {
            servers_.push_back(',');
        }
        servers_.append(servers[i]);
    }

}

ZKEvent::~ZKEvent()
{
    if (epoll_fd_) {
        ::close(epoll_fd_);
    }
}

void ZKEvent::loop()
{
    id_ = pthread_self();

    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ == -1) {
        LOG_DEBUG("poll_create1 error %d", errno);
        return;
    }

    wakeup_fd_ = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (wakeup_fd_ == -1) {
        LOG_DEBUG("eventfd error %d", errno);
        return;
    }

    int n_events;
    while (running_) {
        n_events = epoll_wait(epoll_fd_, events_.data(), events_.size(), 1000);
        if (n_events == -1) {
            LOG_DEBUG("epoll_wait error %d", errno);
            return;
        }
        if (n_events == 0) {
            LOG_DEBUG("no events");
        }
    }
}

void ZKEvent::register_event(detail::Event* e)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.ptr = e;
    event.events = e->interest_ops();

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, e->interest_ops(), &event) != 0) {
        LOG_DEBUG("epoll_ctl error %s", strerror(errno));
    }
}

void ZKEvent::wakeup()
{

}

void ZKEvent::on_connected()
{

}

void ZKEvent::on_session_timeout()
{

}