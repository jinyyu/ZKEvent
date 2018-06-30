#include <assert.h>
#include "ZkClient/ZkClient.h"
#include "ZkClient/DebugLog.h"

namespace zkcli
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

ZkClient::ZkClient(const std::string& servers, int timeout)
    : servers_(servers),
      timeout_(timeout),
      zk_(nullptr)
{
    //disable log
    zoo_set_debug_level((ZooLogLevel) 0);
}

ZkClient::~ZkClient()
{
    if (zk_) {
        zookeeper_close(zk_);
    }
}

void ZkClient::start_connect()
{
    zk_ = zookeeper_init(servers_.c_str(), ZkClient::zk_event_cb, timeout_, nullptr, this, 0);
    if (!zk_) {
        LOG_DEBUG("zookeeper_init error %s", strerror(errno));
        return;
    }
}

void ZkClient::zk_event_cb(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
    ZkClient* zk = (ZkClient*) watcherCtx;
    zk->do_watch_event_cb(zh, type, state, path);
}

void ZkClient::do_watch_event_cb(zhandle_t* zh, int type, int state, const char* path)
{
    LOG_DEBUG("state %d, %s, %d, %s", state, zk_state_to_str(state), type, path)
}

void ZkClient::async_create(const std::string& path, const Slice& data, const StringCallback& cb)
{
    StringCallback* callback = new StringCallback(cb);
    int ret = zoo_acreate(zk_,
                          path.c_str(),
                          data.data(),
                          data.size(),
                          &ZOO_OPEN_ACL_UNSAFE,
                          0,
                          ZkClient::string_completion,
                          callback);
    if (ret != ZOK) {
        cb((ZOO_ERRORS) ret, Slice(nullptr, 0));
        delete (callback);
    }
}

void ZkClient::async_get(const std::string& path, int watch, const StringCallback& cb)
{
    StringCallback* callback = new StringCallback(cb);
    int ret = zoo_aget(zk_, path.c_str(), watch, ZkClient::data_completion, callback);
    if (ret != ZOK) {
        cb((ZOO_ERRORS) ret, Slice(nullptr, 0));
        delete (callback);
    }
}

void ZkClient::async_set(const std::string& path, const Slice& data, const AsyncCallback& cb)
{
    StateCallback* callback = new StateCallback([cb](int err, const Stat* state) {
        cb(err);
    });
    int ret = zoo_aset(zk_, path.c_str(), data.data(), data.size(), -1, ZkClient::stat_completion, callback);
    if (ret != ZOK) {
        cb(ret);
        delete (callback);
    }
}

void ZkClient::async_exists(const std::string& path, int watch, const ExistsCallback& cb)
{
    StateCallback* callback = new StateCallback([cb](int err,const Stat* state) {
        if (err == ZOK) {
            cb(ZOK, true);
        } else if (err == ZNONODE) {
            cb(ZOK, false);
        } else {
            cb(err, false);
        }

    });
    int ret = zoo_aexists(zk_, path.c_str(), watch,  ZkClient::stat_completion, callback);
    if (ret != ZOK) {
        cb(ret, false);
        delete (callback);
    }
}

void ZkClient::string_completion(int rc, const char* value, const void* data)
{
    StringCallback* cb = (StringCallback*) data;
    Slice result;
    if (rc == ZOK) {
        result = Slice(value, strlen(value));
    }
    cb->operator()((ZOO_ERRORS) rc, result);
    delete (cb);
}

void ZkClient::data_completion(int rc, const char* value, int value_len,
                               const struct Stat* stat, const void* data)
{
    StringCallback* cb = (StringCallback*) data;
    Slice result;
    if (rc == ZOK) {
        result = Slice(value, value_len);
    }
    cb->operator()((ZOO_ERRORS) rc, result);
    delete (cb);
}

void ZkClient::stat_completion(int rc, const struct Stat* stat, const void* data)
{
    StateCallback* cb = (StateCallback*) data;
    cb->operator()(rc, stat);
    delete (cb);
}
}