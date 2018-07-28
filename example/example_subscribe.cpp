#include <ZkClient/ZkClient.h>
#include <ZkClient/DebugLog.h>
#include <memory>
#include <unistd.h>
using namespace zkcli;

typedef std::shared_ptr<ZkClient> ZkClientPtr;
class Subscriber* g_subscriber;

class Subscriber
{
public:
    explicit Subscriber(boost::asio::io_service& io_service, const std::string& server, int timeout)
        : server_(server),
          timeout_(timeout),
          io_service_(io_service),
          timer_(io_service)
    {
        start_timer();
    }

    void start_timer()
    {
        timer_.expires_from_now(boost::posix_time::seconds(5));
        timer_.async_wait([this](const boost::system::error_code& err) {
            this->handle_timeout(err);
        });
    }

    void handle_timeout(const boost::system::error_code& err){
        if (err) {
            LOG_DEBUG("async error %s", err.message().c_str());
            return;
        }
        start_timer();
    }

    ~Subscriber()
    {

    }

    void init_zk_client()
    {
        LOG_DEBUG("init zk client");
        ZkClient* zk = new ZkClient(io_service_, server_, timeout_);
        zk->set_connected_callback([zk, this]() {
            LOG_DEBUG("connected");
            ZkClientPtr p(zk);
            zk_ = p;
            this->subscriber_data_changes();
            this->subscribe_child_changes();
        });

        zk->set_session_expired_callback([]() {
            LOG_DEBUG("session expired")
            g_subscriber->init_zk_client();
        });
        zk->start_connect();
    }

private:
    void subscriber_data_changes()
    {
        const char* path = "/test";
        zk_->subscribe_data_changes(path, [this, path](int err, DataChangesEvent event) {
            if (err != ZOK) {
                LOG_DEBUG("subscribe data error %s", ZkClient::err_to_string(err));
                return;
            }

            switch (event) {
                case CREATE:{
                    LOG_DEBUG("path create %s", path);
                    break;
                }
                case CHANGES: {
                    LOG_DEBUG("path data changes %s", path);
                    break;
                }
                case DELETE: {
                    LOG_DEBUG("path delete %s", path);
                    break;
                }
                default:
                    LOG_DEBUG("unknown envet %d", event);
                    break;
            }
            LOG_DEBUG("data changes %s", path)
        });
    }

    void subscribe_child_changes()
    {
        const char* path = "/test";
        zk_->subscribe_child_changes(path, [path, this](int err, StringVectorPtr strings) {
            if (err != ZOK) {
                LOG_DEBUG("subscribe child changes error %s", ZkClient::err_to_string(err))
                return;
            }

            if (!strings) {
                LOG_DEBUG("not child");
                return;
            }

            for(std::string& p : *strings) {
                LOG_DEBUG("child = %s", p.c_str());

            }
        });

    }



private:

    boost::asio::deadline_timer timer_;
    std::string server_;
    int timeout_;
    ZkClientPtr zk_;
    boost::asio::io_service& io_service_;

};

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    g_subscriber = new Subscriber(io_service, "127.0.0.1:2181", 5000);
    g_subscriber->init_zk_client();
    io_service.run();
}
