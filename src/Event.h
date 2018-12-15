#ifndef ZKEVENT_DISTRIBUTION_EVENT_H
#define ZKEVENT_DISTRIBUTION_EVENT_H
#include <stdint.h>
#include <sys/epoll.h>
#include "ZKEvent/Callback.h"

namespace detail
{

class Event
{
public:
    explicit Event(int fd)
        : fd_(fd),
          interest_ops_(0),
          ready_ops_(0)
    {

    }

    ~Event() = default;

    void enable_reading()
    {
        enable_ops(EPOLLIN);
    }

    int fd()
    {
        return fd_;
    }

    void set_reading_callback(const Callback& cb)
    {
        read_cb_ = cb;
    }

    void disable_reading()
    {
        disable_ops(EPOLLIN);
    }

    void enable_writing()
    {
        enable_ops(EPOLLOUT);
    }

    void set_writing_callback(const Callback& cb)
    {
        write_cb_ = cb;
    }

    void disable_writing()
    {
        disable_ops(EPOLLOUT);
    }

    void handle_events()
    {
        if (ready_ops_ & EPOLLIN && read_cb_) {
            read_cb_();
        }
        if (ready_ops_ & EPOLLOUT && write_cb_) {
            write_cb_();
        }
    }

    uint32_t ready_ops() const
    {
        return ready_ops_;
    }

    void ready_ops(uint32_t ops)
    {
        ready_ops_ = ops;
    }

    uint32_t interest_ops() const
    {
        return interest_ops_;
    }

    void interest_ops(uint32_t ops)
    {
        interest_ops_ = ops;
    }

private:

    void enable_ops(uint32_t op)
    {
        interest_ops_ |= op;
    }

    void disable_ops(uint32_t op)
    {
        interest_ops_ &= ~op;
    }

    int fd_;
    uint32_t interest_ops_;
    uint32_t ready_ops_;
    Callback read_cb_;
    Callback write_cb_;
};

}

#endif //ZKEVENT_DISTRIBUTION_EVENT_H
