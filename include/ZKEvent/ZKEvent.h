#ifndef ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#define ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <deque>
#include <unordered_map>
#include <sys/epoll.h>
#include <ZKEvent/Callback.h>

namespace detail
{
class ZKClient;

typedef std::shared_ptr<ZKClient> ClientPtr;

class Event;

struct DataChangesContext
{
    std::string data;
    int64_t zxid;
    StringCallback cb;
};

typedef std::shared_ptr<DataChangesContext> DataChangesContextPtr;

struct ChildChangesContext
{
    StringSetPtr children;
    ChildEventCallback cb;
};

typedef std::shared_ptr<ChildChangesContext> ChildChangesContextPtr;

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

    void set_connected_callback(const Callback& cb)
    {
        connected_cb_ = cb;
    }

    void get(const std::string& path, const StringCallback& cb);

    void create(const std::string& path, const std::string& data, int flag, const StringCallback& cb);

    void exists(const std::string& path, const ExistsCallback& cb);

    void del(const std::string& path, const VoidCallback& cb);

    void children(const std::string& path, const StringsCallback& cb);

    void subscribe_data_changes(const std::string& path, const StringCallback& cb);

    void subscribe_child_changes(const std::string& path, const ChildEventCallback& cb);
private:

    void post_callback(const Callback& cb);

    void wakeup();

    void register_event(detail::Event* e);

    int pull_events(std::vector<detail::Event*>& events);

    void on_connected();

    void on_data_changes(const std::string& path);

    void on_child_changes(const std::string& path);

    void do_subscribe_data_changes(const std::string& path, detail::DataChangesContextPtr ctx);

    void do_subscribe_data_changes(const std::string& path, detail::ChildChangesContextPtr ctx);

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

    std::vector<Callback> pending_callback_;
    std::mutex lock_;

    std::deque<Callback> task_queue_;

    Callback connected_cb_;

    std::unordered_map<std::string, detail::DataChangesContextPtr> data_ctx_;
    std::unordered_map<std::string, detail::ChildChangesContextPtr> child_ctx_;

};

#endif //ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
