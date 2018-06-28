#include <ZkClient/ZkClient.h>
#include <ZkClient/DebugLog.h>
#include <unistd.h>

using namespace zkcli;

ZkClient* cli;

void test_async_create()
{
    StringCallback cb = [] (int code, const Slice& data) {
       LOG_DEBUG("async create code = %s", err_string(code));
    };

    cli->async_create("/test_zkcli", "mydata", cb);
}




int main(int argc, char *argv[]){
    cli = new ZkClient("localhost:2181", 5000);
    test_async_create();

    sleep(2);
}
