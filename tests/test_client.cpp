#include <ZkClient/ZkClient.h>
#include <ZkClient/DebugLog.h>
#include <unistd.h>
#include <condition_variable>

using namespace zkcli;

ZkClient* cli;

std::mutex g_mutex;

std::condition_variable g_cv;

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

void test_async_exists()
{
    ExistsCallback cb = [](int code, bool exists) {
        LOG_DEBUG("async exists %d code = %s", exists, err_string(code));
    };

    cli->async_exists("/test_zkcli", 0, cb);
}

void on_data_changes(int err, const Slice& data)
{
    std::string str(data.data(), data.size());
    LOG_DEBUG("data changes %s", str.c_str());
}

int main(int argc, char* argv[])
{
    cli = new ZkClient("localhost:2181", 10);

    cli->set_connected_callback([]() {
        cli->subscribe_data_changes("/test_zkcli", on_data_changes);
        //test_async_create();
        //test_async_set();
        //test_async_get();
        //test_async_exists();

    });

    cli->set_session_expired_callback([]() {
        LOG_DEBUG("SESSION TIME OUT")
        g_cv.notify_one();
    });

    cli->start_connect();

    cli->run();

    delete (cli);
    LOG_DEBUG("exit");
}
