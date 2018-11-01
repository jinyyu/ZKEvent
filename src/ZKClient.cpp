#include "ZkClient/ZKClient.h"
#include <boost/algorithm/string.hpp>
#include "DebugLog.h"



ZKClient::ZKClient(const std::vector<std::string>& servers)
{
    servers_ = boost::join(servers, ",");
    LOG_DEBUG("servers = %s", servers_.c_str());
}

ZKClient::~ZKClient()
{

}