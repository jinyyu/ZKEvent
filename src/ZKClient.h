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

public:
    ZKEvent* owner_;
    zhandle_t* zk_;
};

}


#endif //ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
