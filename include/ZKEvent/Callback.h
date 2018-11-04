#ifndef ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
#define ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
#include <functional>
#include <vector>
#include <memory>
#include <ZKEvent/Status.h>
#include <ZKEvent/Slice.h>

typedef std::function<void()> Callback;

typedef std::function<void(const Status& status)> VoidCallback;

typedef std::function<void(const Status& status, const Slice& data)> StringCallback;

typedef std::function<void(const Status& status, bool exists)> ExistsCallback;

typedef std::shared_ptr<std::vector<std::string>> StringVectorPtr;

typedef std::function<void(const Status& status, StringVectorPtr strings)> StringsCallback;


#endif //ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
