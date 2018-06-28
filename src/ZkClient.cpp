#include <assert.h>
#include "ZkClient/ZkClient.h"
#include "DebugLog.h"


static const char* zk_state_to_str(int state) {
    if (state == ZOO_EXPIRED_SESSION_STATE) {
        return "EXPIRED_SESSION_STATE";
    }
    if (state == ZOO_AUTH_FAILED_STATE) {
        return "AUTH_FAILED_STATE";
    }
    if (state == ZOO_CONNECTING_STATE) {
        return "CONNECTING_STATE";
    }
    if (state == ZOO_ASSOCIATING_STATE) {
        return "ASSOCIATING_STATE";
    }
    if (state == ZOO_CONNECTED_STATE) {
        return "ZOO_CONNECTED_STATE";
    }
    if (state == ZOO_READONLY_STATE) {
        return "READONLY_STATE";
    }
    if (state == ZOO_NOTCONNECTED_STATE) {
        return "NOTCONNECTED_STATE";
    }
    return "INVALID_STATE";
}

const char* zk_type_to_str(int type)
{
    if (type == ZOO_CREATED_EVENT) {
        return "CREATED_EVENT";
    }
    if (type == ZOO_DELETED_EVENT) {
        return "DELETED_EVENT";
    }
    if (type == ZOO_CHANGED_EVENT) {
        return  "CHANGED_EVENT";
    }
    if (type == ZOO_CHILD_EVENT) {
        return "CHILD_EVENT";
    }
    if (type == ZOO_SESSION_EVENT) {
        return "SESSION_EVENT";
    }
    if (type == ZOO_NOTWATCHING_EVENT) {
        return "NOTWATCHING_EVENT";
    }
    return "INVALID_TYPE";
}


ZkClient::ZkClient(const std::string& servers, int timeout)
    : servers_(servers),
      timeout_(timeout),
      zk_(nullptr),
      state_(ZOO_CONNECTING_STATE)
{
    //disable log
    zoo_set_debug_level((ZooLogLevel)0);

    start_connect();
}

ZkClient::~ZkClient(){
   if (zk_) {
       zookeeper_close(zk_);
   }
}

void ZkClient::start_connect()
{
    zhandle_t* zoo = zookeeper_init(servers_.c_str(), ZkClient::zk_event_cb, timeout_, nullptr, this, 0);
    if (!zoo) {
        LOG_DEBUG("zookeeper_init error %s", strerror(errno));
        return;
    }
}

void ZkClient::zk_event_cb(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
    ZkClient* zk = (ZkClient*)watcherCtx;
    zk->do_watch_event_cb(zh, type, state, path);
}

void ZkClient::do_watch_event_cb(zhandle_t* zh, int type, int state, const char* path)
{
    LOG_DEBUG("state %d, %s, %d, %s", state, zk_state_to_str(state), type, path)

    std::unique_lock<std::mutex> guard(mutex_);
    //update state
    state_ = state;

}