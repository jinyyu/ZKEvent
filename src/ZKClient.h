#ifndef ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
#define ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
#include <zookeeper/zookeeper.h>
#include <ZKEvent/Callback.h>

class ZKEvent;

namespace detail
{

typedef std::function<void(const Status& status, const struct Stat* zk_state, const Slice& data)>  DataCompletion;
class ZKClient
{
public:
    explicit ZKClient(ZKEvent* owner);

    ~ZKClient();

    void get(const std::string& path, int watch, const DataCompletion& cb);

private:
    static void zk_event_cb(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx);

    static void data_completion(int rc, const char* value, int value_len, const struct Stat* stat, const void* data);



private:
    ZKEvent* owner_;
    zhandle_t* zk_;
    const clientid_t* client_id_;
};

}


#endif //ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
