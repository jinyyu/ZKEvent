#include "ZKClient.h"
#include "ZKEvent/ZKEvent.h"

namespace detail
{

ZKClient::ZKClient(ZKEvent* owner)
    : owner_(owner),
      zk_(nullptr)
{


}

ZKClient::~ZKClient()
{
    if (zk_) {
        zookeeper_close(zk_);
    }
}


}
