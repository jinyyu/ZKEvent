#ifndef ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
#define ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
#include <zookeeper/zookeeper.h>
#include <functional>
#include <ZkClient/Slice.h>
#include <vector>
#include <string>
#include <memory>

namespace zkcli
{

typedef std::function<void()> VoidCallback;

typedef std::function<void(int err,const Stat* state)> StateCallback;

typedef std::function<void(int err)> AsyncCallback;

typedef std::function<void(int err,const Slice&)> StringCallback;

typedef std::function<void(int err, bool exists)> ExistsCallback;

typedef std::shared_ptr<std::vector<std::string>> StringVectorPtr;

typedef std::function<void(int err, StringVectorPtr strings)> StringsCallback;

}


#endif //ZKCLIENT_DISTRIBUTION_ASYNCCALLBACK_H
