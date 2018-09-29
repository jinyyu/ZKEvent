#include <ZkClient/ZkClient.h>
#include <ZkClient/DebugLog.h>
#include <unistd.h>
#include <condition_variable>

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

void test_async_get_child()
{
    StringsCallback cb = [](int code, StringVectorPtr strings) {
        LOG_DEBUG("async get child code = %s", err_string(code));
        if (code != ZOK) {
            return;
        }

        for (auto it = strings->begin(); it != strings->end(); ++it) {
            LOG_DEBUG("child %s", it->c_str());
        }
    };
    cli->async_get_children("/", 0, cb);
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

void on_data_changes(int err, DataChangesEvent event)
{

}

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    cli = new ZkClient(io_service, "localhost:2181", 10);

    cli->set_connected_callback([]() {
        cli->subscribe_data_changes("/test_zkcli", on_data_changes);
        test_async_create();
        test_async_set();
        test_async_get();
        test_async_exists();
        test_async_get_child();

    });

    cli->set_session_expired_callback([]() {
        LOG_DEBUG("SESSION TIME OUT")
    });

    cli->start_connect();

    io_service.run();

    delete (cli);
    LOG_DEBUG("exit");
}
