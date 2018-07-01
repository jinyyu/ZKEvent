#ifndef ZKCLIENT_ZKCLIENT_H
#define ZKCLIENT_ZKCLIENT_H
#include <string>
#include <zookeeper/zookeeper.h>
#include <ZkClient/AsyncCallback.h>
#include <ZkClient/errors.h>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <deque>
#include <thread>


namespace zkcli
{

class ZkClient
{
public:
    explicit ZkClient(const std::string& servers, int timeout);

    void set_connected_callback(const VoidCallback& cb);

    void set_session_expired_callback(const VoidCallback& cb);

    void start_connect();

    ~ZkClient();

    void run();

    void stop();

    void async_create(const std::string& path, const Slice& data, const StringCallback& cb);

    void async_get(const std::string& path, int watch, const StringCallback& cb);

    void async_set(const std::string& path, const Slice& data, const AsyncCallback& cb);

    void async_exists(const std::string& path, int watch, const ExistsCallback& cb);

    void subscribe_data_changes(const std::string& path, const StringCallback& cb);

private:

    void run_in_loop(const VoidCallback& cb);

    static void zk_event_cb(zhandle_t* zh, int type,
                            int state, const char* path, void* watcherCtx);

    void do_watch_event_cb(zhandle_t* zh, int type, int state, const std::string& path);

    static void string_completion(int rc, const char* value, const void* data);

    static void data_completion(int rc, const char* value, int value_len,
                                const struct Stat* stat, const void* data);

    static void stat_completion(int rc, const struct Stat* stat, const void* data);

    void do_subscribe_data_changes(const std::string& path);

private:
    std::string servers_;
    int timeout_;

    pthread_t thread_id_;
    volatile bool running_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<VoidCallback> pending_callbacks_;
    int state_;

    zhandle_t* zk_;

    const clientid_t* client_id_;
    std::unordered_map<std::string, StringCallback> data_changes_cb_;

    VoidCallback connected_cb_;
    VoidCallback session_expired_cb_;
};

}

#endif //ZKCLIENT_ZKCLIENT_H
