#include "ZkClient/ZkClient.h"


ZkClient::ZkClient(const std::string& servers, int timeout)
    : servers_(servers),
      timeout_(timeout)
{
    zk_ = zookeeper_init(servers_.c_str(), ZkClient::zk_event_cb, timeout_, nullptr, this, 0);
}

void ZkClient::zk_event_cb(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{

}

void ZkClient::do_watch_event_cb(int type, int state, const char* path)
{

}