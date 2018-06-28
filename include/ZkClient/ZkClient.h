#ifndef ZKCLIENT_ZKCLIENT_H
#define ZKCLIENT_ZKCLIENT_H
#include <string>
#include <zookeeper/zookeeper.h>
#include <mutex>
#include <ZkClient/AsyncCallback.h>
#include <ZkClient/errors.h>

namespace zkcli
{

class ZkClient
{
public:
    explicit ZkClient(const std::string& servers, int timeout);

    ~ZkClient();

    void async_create(const std::string& path, const Slice& data, const StringCallback& cb);

private:
    void start_connect();

    static void zk_event_cb(zhandle_t* zh, int type,
                            int state, const char* path, void* watcherCtx);

    void do_watch_event_cb(zhandle_t* zh, int type, int state, const char* path);

    static void handle_create(int rc, const char* value, const void* data);
private:
    std::string servers_;
    int timeout_;

    std::mutex mutex_;
    int state_;
    zhandle_t* zk_;
};

}

#endif //ZKCLIENT_ZKCLIENT_H
