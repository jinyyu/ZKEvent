#include <ZKEvent/ZKEvent.h>
#include <thread>
#include <unistd.h>


int main(int argc, char* argv[])
{
    std::vector<std::string> servers;
    servers.emplace_back("127.0.0.1:2181");
    servers.emplace_back("192.168.242.128:2181");

    ZKEvent* client = new ZKEvent(servers, 5000);

    client->set_connected_callback([client]() {
        client->get("/test", [](const Status& status, const Slice& data){
            if (!status.is_ok()) {
                fprintf(stderr, "----------------------------------------get error %s\n", status.to_string().c_str());
            } else {
                fprintf(stderr, "----------------------------------------get ok, %s\n", data.to_string().c_str());
            }
        });
                                   });

    client->get("/test", [](const Status& status, const Slice& data){
        if (!status.is_ok()) {
            fprintf(stderr, "----------------------------------------get error %s\n", status.to_string().c_str());
        } else {
            fprintf(stderr, "----------------------------------------get ok, %s\n", data.to_string().c_str());
        }
    });

    std::thread t([client]() {
        sleep(1);
        client->start_connect();
    });

    client->loop();

    t.join();
    delete client;
}
