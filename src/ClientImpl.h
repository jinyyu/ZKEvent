#ifndef ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
#define ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
#include <zookeeper/zookeeper.h>


class ZKClient;
class ClientImpl
{
public:
    explicit ClientImpl(ZKClient* owner);

    ~ClientImpl();

public:
    ZKClient* owner_;
    zhandle_t* zk_;
};


#endif //ZKCLIENT_DISTRIBUTION_CLIENTIMPL_H
