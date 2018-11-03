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

Status zk_rc_status(int rc)
{
    ZOO_ERRORS e = (ZOO_ERRORS) rc;
    switch (e) {
        case ZOK:
            return Status::ok();
        case ZSYSTEMERROR:
            return Status::io_error("System and server-side errors");
        case ZRUNTIMEINCONSISTENCY:
            return Status::io_error("A runtime inconsistency was found");
        case ZDATAINCONSISTENCY:
            return Status::io_error("A data inconsistency was found");
        case ZCONNECTIONLOSS:
            return Status::io_error("Connection to the server has been lost");
        case ZMARSHALLINGERROR:
            return Status::io_error("Error while marshalling or unmarshalling data");
        case ZUNIMPLEMENTED:
            return Status::not_supported("Operation is unimplemented");
        case ZOPERATIONTIMEOUT:
            return Status::io_error("Operation timeout");
        case ZBADARGUMENTS:
            return Status::invalid_argument("Invalid arguments");
        case ZINVALIDSTATE:
            return Status::io_error("Invliad zhandle state");
        case ZNEWCONFIGNOQUORUM:
            return Status::io_error("No quorum of new config is connected");
        case ZRECONFIGINPROGRESS:
            return Status::not_supported("currently not supported");
        case ZNONODE:
            return Status::not_found("node does not exist");
        case ZNOAUTH:
            return Status::io_error("not authenticated");
        case ZBADVERSION:
            return Status::io_error("version conflict");
        case ZNOCHILDRENFOREPHEMERALS:
            return Status::not_found("ephemeral nodes may not have children");
        case ZNODEEXISTS:
            return Status::not_found("the node already exists");
        case ZNOTEMPTY:
            return Status::io_error("The node has children");
        case ZSESSIONEXPIRED:
            return Status::io_error("session has been expired");
        case ZINVALIDCALLBACK:
            return Status::invalid_argument("invalid callback specified");
        case ZINVALIDACL:
            return Status::invalid_argument("invalid ACL specified");
        case ZAUTHFAILED:
            return Status::invalid_argument("client authentication failed");
        case ZCLOSING:
            return Status::io_error("zooKeeper is closing");
        case ZNOTHING:
            return Status::io_error("no server responses to process");
        case ZSESSIONMOVED:
            return Status::io_error("session moved to another server, so operation is ignored");
        case ZNOTREADONLY:
            return Status::io_error("state-changing request is passed to read-only server");
        case ZEPHEMERALONLOCALSESSION:
            return Status::io_error("attempt to create ephemeral node on a local session");
        case ZNOWATCHER:
            return Status::not_found("the watcher couldn't be found");
        case ZRECONFIGDISABLED:
            return Status::io_error(
                "attempts to perform a reconfiguration operation when reconfiguration feature is disabled");
    }
    return Status::io_error("unknown error");
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

void ZKClient::get(const std::string& path, int watch, const DataCompletion& cb)
{
    DataCompletion* callback = new DataCompletion(cb);
    int ret = zoo_aget(zk_, path.c_str(), watch, ZKClient::data_completion, callback);
    if (ret != ZOK) {
        cb(zk_rc_status(ret), NULL, Slice());
        delete (callback);
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

void ZKClient::data_completion(int rc, const char* value, int value_len, const struct Stat* stat, const void* data)
{
    DataCompletion* cb = (DataCompletion*) (data);
    cb->operator()(zk_rc_status(rc), stat, Slice(value, value_len));
    delete(cb);

}

}
