#ifndef ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
#define ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
#include <zookeeper/zookeeper.h>
#include <functional>
#include <ZkClient/Slice.h>

namespace zkcli
{

typedef std::function<void(int err,const Slice&)> StringCallback;








}


#endif //ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
