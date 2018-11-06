#ifndef ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
#define ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
#include <zookeeper/zookeeper.h>
#include <ZKEvent/Callback.h>

class ZKEvent;

namespace detail
{

typedef std::function<void(const Status& status, const struct Stat* zk_state, const Slice& data)> DataCompletion;

typedef std::function<void(const Status& status, const struct Stat* zk_state, bool exists)> ExistsCompletion;

class ZKClient
{
public:
    explicit ZKClient(ZKEvent* owner);

    ~ZKClient();

    void get(const std::string& path, int watch, const DataCompletion& cb);

    void create(const std::string& path, const Slice& data, int flag, const StringCallback& cb);

    void exists(const std::string& path, int watch, const ExistsCompletion& cb);

    void del(const std::string& path, int version, const VoidCallback& cb);

    void children(const std::string& path, int watch, const StringsCallback& cb);
private:
    static void zk_event_cb(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx);

    static void data_completion(int rc, const char* value, int value_len, const struct Stat* stat, const void* data);

    static void string_completion(int rc, const char* string, const void* data);

    static void exists_completion(int rc, const struct Stat* stat, const void* data);

    static void void_completion(int rc, const void* data);

    static void strings_completion(int rc, const struct String_vector* strings, const void* data);

private:
    ZKEvent* owner_;
    zhandle_t* zk_;
};

}


#endif //ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
