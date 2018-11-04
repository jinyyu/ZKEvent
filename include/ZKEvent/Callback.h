#ifndef ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
#define ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
#include <functional>
#include <set>
#include <memory>
#include <ZKEvent/Status.h>
#include <ZKEvent/Slice.h>

typedef std::function<void()> Callback;

typedef std::function<void(const Status& status)> VoidCallback;

typedef std::function<void(const Status& status, const Slice& data)> StringCallback;

typedef std::function<void(const Status& status, bool exists)> ExistsCallback;

typedef std::shared_ptr<std::set<std::string>> StringSetPtr;

typedef std::function<void(const Status& status, StringSetPtr strings)> StringsCallback;

enum ChildEvent
{
    ChildEventErr = 0,
    ChildEventAdd = 1,
    ChildEventDel = 2
};

typedef std::function<void(const Status& status, ChildEvent ev, const Slice& path)> ChildEventCallback;

#endif //ZKEVENT_DISTRIBUTION_ASYNCCALLBACK_H
