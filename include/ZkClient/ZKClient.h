#ifndef ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#define ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#include <string>
#include <vector>
#include <memory>


class ClientImpl;


class ZKClient
{
    typedef std::shared_ptr<ClientImpl> ClientPtr;
public:

    ZKClient(const std::vector<std::string>& servers);

    ~ZKClient();

private:
    std::string servers_;
    ClientPtr client_;
};

#endif //ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
