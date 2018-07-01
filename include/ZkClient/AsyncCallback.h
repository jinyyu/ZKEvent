#ifndef ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
#define ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
#include <zookeeper/zookeeper.h>
#include <functional>
#include <ZkClient/Slice.h>

namespace zkcli
{

typedef std::function<void()> VoidCallback;

typedef std::function<void(int err,const Stat* state)> StateCallback;

typedef std::function<void(int err)> AsyncCallback;

typedef std::function<void(int err,const Slice&)> StringCallback;

typedef std::function<void(int err, bool exists)> ExistsCallback;

}


#endif //ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
