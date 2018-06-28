#include "ZkClient/errors.h"
#include <zookeeper/zookeeper.h>


namespace zkcli
{
const char* err_string(int err)
{
    ZOO_ERRORS e = (ZOO_ERRORS) err;
    switch (e) {
        case ZOK:
            return "OK";
        case ZSYSTEMERROR:
            return "system error";
        case ZRUNTIMEINCONSISTENCY:
            return "A runtime inconsistency was found";
        case ZDATAINCONSISTENCY:
            return "A data inconsistency was found";
        case ZCONNECTIONLOSS:
            return "Connection to the server has been lost";
        case ZMARSHALLINGERROR:
            return "Error while marshalling or unmarshalling data";
        case ZUNIMPLEMENTED:
            return "Operation is unimplemented";
        case ZOPERATIONTIMEOUT:
            return "Operation timeout";
        case ZBADARGUMENTS:
            return "Invalid arguments";
        case ZINVALIDSTATE:
            return "Invliad zhandle state";
        case ZNEWCONFIGNOQUORUM:
            return "No quorum of new config is connected";
        case ZRECONFIGINPROGRESS:
            return "currently not supported";
        case ZNONODE:
            return "node does not exist";
        case ZNOAUTH:
            return "not authenticated";
        case ZBADVERSION:
            return "version conflict";
        case ZNOCHILDRENFOREPHEMERALS:
            return "ephemeral nodes may not have children";
        case ZNODEEXISTS:
            return "the node already exists";
        case ZNOTEMPTY:
            return "The node has children";
        case ZSESSIONEXPIRED:
            return "session has been expired";
        case ZINVALIDCALLBACK:
            return "invalid callback specified";
        case ZINVALIDACL:
            return "invalid ACL specified";
        case ZAUTHFAILED:
            return "client authentication failed";
        case ZCLOSING:
            return "zooKeeper is closing";
        case ZNOTHING:
            return "no server responses to process";
        case ZSESSIONMOVED:
            return "session moved to another server, so operation is ignored";
        case ZNOTREADONLY:
            return "state-changing request is passed to read-only server";
        case ZEPHEMERALONLOCALSESSION:
            return "attempt to create ephemeral node on a local session";
        case ZNOWATCHER:
            return "the watcher couldn't be found";
        case ZRECONFIGDISABLED:
            return "attempts to perform a reconfiguration operation when reconfiguration feature is disabled";
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "invalid error %d", err);
    return buffer;
}

}