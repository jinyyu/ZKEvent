#include "ClientImpl.h"
#include "ZkClient/ZKClient.h"


ClientImpl::ClientImpl(ZKClient* owner)
    : owner_(owner_)
{

}

ClientImpl::~ClientImpl()
{

}
