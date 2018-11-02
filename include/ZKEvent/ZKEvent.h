#ifndef ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#define ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <sys/epoll.h>
#include <ZKEvent/Callback.h>

namespace detail
{
class ZKClient;

typedef std::shared_ptr<ZKClient> ClientPtr;

class Event;

}

class ZKEvent
{
public:

    ZKEvent(const std::vector<std::string>& servers, int timeout);

    ~ZKEvent();

    void stop()
    {
        running_ = false;
        wakeup();
    }

    void loop();

private:
    void register_event(detail::Event* e);

    void wakeup();

    void on_connected();

    void on_session_timeout();

    volatile bool running_;
    std::string servers_;
    int timeout_;
    detail::ClientPtr client_;
    std::vector<struct epoll_event> events_;
    pthread_t id_;
    int epoll_fd_;
    int wakeup_fd_;
    detail::Event* wakeup_event_;

    std::vector<VoidCallback> pending_callback;
    std::mutex lock_;

};

#endif //ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
