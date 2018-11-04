#ifndef ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#define ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <deque>
#include <sys/epoll.h>
#include <ZKEvent/Callback.h>

namespace detail
{
class ZKClient;

typedef std::shared_ptr<ZKClient> ClientPtr;

class Event;

}

extern int CreateEphemeral;

extern int CreateSequence;

class ZKEvent
{
public:

    explicit ZKEvent(const std::vector<std::string>& servers, int timeout);

    ~ZKEvent();

    void stop()
    {
        running_ = false;
        wakeup();
    }

    void loop();

    void start_connect();

    void set_connected_callback(const VoidCallback& cb)
    {
        connected_cb_ = cb;
    }

    void get(const std::string& path, const StringCallback& cb);

    void create(const std::string& path, const std::string& data, int flag, const StringCallback& cb);

    void exists(const std::string& path, const ExistsCallback& cb);
private:

    void post_callback(const VoidCallback& cb);

    void wakeup();

    void setup();

    void register_event(detail::Event* e);

    int pull_events(std::vector<detail::Event*>& events);

    void on_connected();

    void on_session_timeout();

    volatile bool running_;
    std::string servers_;
    void* zk_client_id_;
    int timeout_;
    friend class detail::ZKClient;
    detail::ClientPtr client_;

    std::vector<struct epoll_event> events_;
    pthread_t id_;
    int epoll_fd_;
    int wakeup_fd_;
    detail::Event* wakeup_event_;

    std::vector<VoidCallback> pending_callback_;
    std::mutex lock_;

    std::deque<VoidCallback> task_queue_;

    VoidCallback connected_cb_;
};

#endif //ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
