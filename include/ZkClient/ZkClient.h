#ifndef ZKCLIENT_ZKCLIENT_H
#define ZKCLIENT_ZKCLIENT_H
#include <string>
#include <zookeeper/zookeeper.h>
#include <ZkClient/AsyncCallback.h>
#include <ZkClient/errors.h>
#include <unordered_map>
#include <boost/asio.hpp>


namespace zkcli
{

class ZkClient
{
public:
    static const char* err_to_string(int err);

    explicit ZkClient(boost::asio::io_service& io_service, const std::string& servers, int timeout);

    void set_connected_callback(const VoidCallback& cb);

    void set_session_expired_callback(const VoidCallback& cb);

    void start_connect();

    ~ZkClient();

    void async_create(const std::string& path, const Slice& data, const StringCallback& cb);

    void async_get(const std::string& path, int watch, const StringCallback& cb);

    void async_set(const std::string& path, const Slice& data, const AsyncCallback& cb);

    void async_exists(const std::string& path, int watch, const ExistsCallback& cb);
    
    void async_get_children(const std::string& path, int watch, const StringsCallback& cb);

    void subscribe_data_changes(const std::string& path, const AsyncCallback& cb);
    
    void subscribe_child_changes(const std::string& path, const StringsCallback& cb);

private:

    void run_in_loop(const VoidCallback& cb);

    static void zk_event_cb(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx);

    void do_watch_event_cb(zhandle_t* zh, int type, int state, const std::string& path);

    static void string_completion(int rc, const char* value, const void* data);

    static void data_completion(int rc, const char* value, int value_len, const struct Stat* stat, const void* data);

    static void stat_completion(int rc, const struct Stat* stat, const void* data);

    static void strings_completion(int rc, const struct String_vector *strings, const void *data);

    void do_subscribe_data_changes(const std::string& path);

    void do_subscribe_child_changes(const std::string& path);
private:
    std::string servers_;
    int timeout_;

    pthread_t thread_id_;

    zhandle_t* zk_;

    const clientid_t* client_id_;
    std::unordered_map<std::string, AsyncCallback> data_changes_cb_;
    std::unordered_map<std::string, StringsCallback> child_changes_cb_;
    VoidCallback connected_cb_;
    VoidCallback session_expired_cb_;
    boost::asio::io_service& io_service_;
};

}

#endif //ZKCLIENT_ZKCLIENT_H
