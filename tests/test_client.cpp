#include <ZkClient/ZKClient.h>


int main(int argc, char* argv[])
{
    std::vector<std::string> servers;
    servers.emplace_back("127.0.0.1:2181");
    servers.emplace_back("192.168.242.128:2181");

    ZKClient client(servers);


}
