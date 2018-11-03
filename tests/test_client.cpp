#include <ZKEvent/ZKEvent.h>
#include <thread>
#include <unistd.h>


int main(int argc, char* argv[])
{
    std::vector<std::string> servers;
    servers.emplace_back("127.0.0.1:2181");
    servers.emplace_back("192.168.242.128:2181");

    ZKEvent* client = new ZKEvent(servers, 5000);

    std::thread t([client]() {
        for (int i = 0; i < 10; ++i) {
            sleep(5);
            client->post_callback([client]() {
                fprintf(stderr, "wake up %d\n", pthread_self());

                client->post_callback([](){
                    fprintf(stderr, "-----wake up %d\n", pthread_self());
                });
            });

        }
        client->stop();
    });

    client->loop();

    t.join();
    delete client;
}
