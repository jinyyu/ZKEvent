#ifndef ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
#define ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
#include <functional>
#include <ZKEvent/Status.h>
#include <ZKEvent/Slice.h>

typedef std::function<void()> VoidCallback;

typedef std::function<void(const Status& status, const Slice& data)> StringCallback;

typedef std::function<void(const Status& status, bool exists)> ExistsCallback;


#endif //ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
