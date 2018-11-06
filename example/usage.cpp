#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <ZKEvent/ZKEvent.h>
#include <thread>
#include <unistd.h>

#define LOG_DEBUG(format, ...) do { fprintf(stderr, "[DEBUG] [%s:%d] " format "\n", strrchr(__FILE__, '/') + 1, __LINE__, ##__VA_ARGS__); } while(0)

static const char* g_zk_server = "127.0.0.1:2181";

static int g_timeout = 5000;

std::shared_ptr<ZKEvent> g_client;

static GOptionEntry g_entries[] = {
    {"zk", 'z', 0, G_OPTION_ARG_STRING, &g_zk_server, "zookeeper server list", NULL},
    {"timeout", 't', 0, G_OPTION_ARG_INT, &g_timeout, "zookeeper timeout", NULL},
    {NULL}
};

void notify_thread()
{
    for (int i = 0; i < 20; ++i) {
        sleep(1);
        auto create_cb = [](const Status& status, const Slice& path) {
            if (status.is_ok()) {
                LOG_DEBUG("create success, path = %s", path.to_string().c_str());
            }
            else {
                LOG_DEBUG("create error %s", status.to_string().c_str());
            }
        };
        g_client->create("/test-", "some data", CreateEphemeral | CreateSequence, create_cb);
        g_client->set("/test", std::to_string(i), [](const Status& status) {});
    }
    g_client->stop();
}

int main(int argc, char* argv[])
{
    GError* error = NULL;
    GOptionContext* context = g_option_context_new("usage");
    g_option_context_add_main_entries(context, g_entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        fprintf(stderr, "option parsing failed: %s\n", error->message);
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> servers;
    servers.emplace_back(g_zk_server);

    g_client = std::make_shared<ZKEvent>(servers, g_timeout);
    g_client->set_connected_callback([]() {
        g_client->exists("/test", [](const Status& status, bool exists) {
            if (!status.is_ok()) {
                LOG_DEBUG("exists error %s", status.to_string().c_str());
                g_client->stop();
                return;
            }
            if (exists) {
                std::thread t(notify_thread);
                t.detach();
            }
        });

        g_client->create("/test", "data", 0, [](const Status& status, const Slice& path) {
            if (status.is_ok()) {
                LOG_DEBUG("create success, path = %s", path.to_string().c_str());
            }
            else {
                LOG_DEBUG("create error %s", status.to_string().c_str());
            }
        });


        g_client->get("/test", [](const Status& status, const Slice& data) {
            if (status.is_ok()) {
                LOG_DEBUG("get success, data = %s", data.to_string().c_str());
            }
            else {
                LOG_DEBUG("get error %s", status.to_string().c_str());
            }
        });


        g_client->children("/", [](const Status& status, StringSetPtr strings) {
            if (status.is_ok()) {
                LOG_DEBUG("children success");
                for (auto it = strings->begin(); it != strings->end(); ++it) {
                    LOG_DEBUG("path = %s", ("/" + *it).c_str());
                }
            }
            else {
                LOG_DEBUG("children error");
            }
        });

        g_client->subscribe_data_changes("/test", [](const Status& status, const Slice& data) {
            if (status.is_ok()) {
                LOG_DEBUG("%s data changes: %s", "/test", data.to_string().c_str());
            }
            else {
                LOG_DEBUG("data changes");
            }
        });

        g_client->subscribe_child_changes("/", [](const Status& status, ChildEvent ev, const Slice& path) {

            if (!status.is_ok()) {
                LOG_DEBUG("subscribe error %s", status.to_string().c_str());
            }
            else {
                switch (ev) {
                    case ChildEventAdd:
                        LOG_DEBUG("/ child add %s", path.to_string().c_str());
                        break;
                    case ChildEventDel:
                        LOG_DEBUG("/ child del %s", path.to_string().c_str());
                        break;
                    default:
                        LOG_DEBUG("unknown event %d", ev);
                }
            }

        });
    });

    g_client->start_connect();
    g_client->loop();
    g_client = nullptr;
    g_option_context_free(context);
}
