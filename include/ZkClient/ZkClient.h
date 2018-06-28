#ifndef ZKCLIENT_ZKCLIENT_H
#define ZKCLIENT_ZKCLIENT_H
#include <string>
#include <zookeeper/zookeeper.h>

class ZkClient
{
public:
    explicit ZkClient(const std::string& servers, int timeout);


private:
    static void zk_event_cb(zhandle_t* zh, int type,
                            int state, const char* path, void* watcherCtx);

    void do_watch_event_cb(int type, int state, const char* path);
private:
    std::string servers_;
    int timeout_;
    zhandle_t* zk_;
};

#endif //ZKCLIENT_ZKCLIENT_H
