#include "ZKClient.h"
#include "ZKEvent/ZKEvent.h"
#include <string.h>
#include "DebugLog.h"

namespace detail
{

static const char* zk_state_to_str(int state)
{
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
        return "CONNECTED_STATE";
    }
    if (state == ZOO_READONLY_STATE) {
        return "READONLY_STATE";
    }
    if (state == ZOO_NOTCONNECTED_STATE) {
        return "NOTCONNECTED_STATE";
    }
    return "INVALID_STATE";
}

static const char* zk_type_to_str(int type)
{
    if (type == ZOO_CREATED_EVENT) {
        return "CREATED_EVENT";
    }
    if (type == ZOO_DELETED_EVENT) {
        return "DELETED_EVENT";
    }
    if (type == ZOO_CHANGED_EVENT) {
        return "CHANGED_EVENT";
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

ZKClient::ZKClient(ZKEvent* owner)
    : owner_(owner),
      zk_(nullptr),
      client_id_(nullptr)
{
    zk_ = zookeeper_init(owner->servers_.c_str(),
                         ZKClient::zk_event_cb,
                         owner->timeout_,
                         (clientid_t*) owner_->zk_client_id_,
                         this,
                         0);
    if (!zk_) {
        LOG_DEBUG("zookeeper_init error %s", strerror(errno));
        throw std::runtime_error("inid zookeeper error");
    }
}

ZKClient::~ZKClient()
{
    if (zk_) {
        zookeeper_close(zk_);
    }
}

void ZKClient::zk_event_cb(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
    LOG_DEBUG("state %s, type = %s, path = %s", zk_state_to_str(state), zk_type_to_str(type), path);
    ZKClient* thiz = (ZKClient*) watcherCtx;
    ZKEvent* owner = thiz->owner_;

    if (type == ZOO_SESSION_EVENT && state == ZOO_CONNECTED_STATE) {
        owner->zk_client_id_ = (void*) zoo_client_id(zh);
        owner->on_connected();
    }

    if (state == ZOO_EXPIRED_SESSION_STATE) {
        owner->start_connect();
        return;
    }

}

}
