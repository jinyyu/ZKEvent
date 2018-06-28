#include <ZkClient/ZkClient.h>
#include <ZkClient/DebugLog.h>
#include <unistd.h>

using namespace zkcli;

ZkClient* cli;

void test_async_create()
{
    StringCallback cb = [](int code, const Slice& data) {
        LOG_DEBUG("async create code = %s", err_string(code));
    };

    cli->async_create("/test_zkcli", "mydata", cb);
}

void test_async_set()
{
    AsyncCallback cb = [](int code) {
        LOG_DEBUG("async SET code = %s", err_string(code));
    };

    cli->async_set("/test_zkcli", "new data", cb);
}

void test_async_get()
{
    StringCallback cb = [](int code, const Slice& data) {
        std::string str(data.data(), data.size());
        LOG_DEBUG("async get %s code = %s", str.c_str(), err_string(code));
    };

    cli->async_get("/test_zkcli", 0, cb);
}

int main(int argc, char* argv[])
{
    cli = new ZkClient("localhost:2181", 5000);
    test_async_create();
    test_async_set();
    test_async_get();
    sleep(2);
    delete (cli);
}
