#ifndef ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#define ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
#include <string>
#include <vector>
#include <memory>


class ClientImpl;
class ZKClient
{
    friend class ClientImpl;
    typedef std::shared_ptr<ClientImpl> ClientPtr;
public:

    ZKClient(const std::vector<std::string>& servers, int timeout);

    ~ZKClient();

private:

    void on_connected();

    void on_session_timeout();

    std::string servers_;
    int timeout_;
    ClientPtr client_;
};

#endif //ZKCLIENT_DISTRIBUTION_ZKCLIENT_H
