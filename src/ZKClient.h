#ifndef ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
#define ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
#include <zookeeper/zookeeper.h>


class ZKEvent;

namespace detail
{
class ZKClient
{
public:
    explicit ZKClient(ZKEvent* owner);

    ~ZKClient();

private:
    static void zk_event_cb(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx);

private:
    ZKEvent* owner_;
    zhandle_t* zk_;
    const clientid_t* client_id_;
};

}


#endif //ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
