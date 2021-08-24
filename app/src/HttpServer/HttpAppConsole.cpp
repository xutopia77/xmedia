

/*
{
	"name":	"media server",
	"host":	[{
			"name":	"",
			"source":	[{
					"srvName":	"live/avc",
					"bRecord":	true,
					"player":	["0"]
				}],
			"publishNum":	1,
			"playerNum":	1
		}, {
			"name":	"rtspHost",
			"publishNum":	3222,
			"playerNum":	111,
			"source":	["source1", "source2"]
		}]
}
*/

#include "HttpUtil.h"
#include "HttpRoute.h"
#include "cJSON.h"

#include <string>
#include <fstream>
#include <memory>
#include <vector>

#include "MediaServer.h"
#include "RtmpSrv.h"


using namespace std;

extern const string htmlRootPath;
extern int http_server_reply(http_session_t* session, int code, const void* data, size_t bytes);

extern shared_ptr<MediaServer> pMediaSrv;

struct HostDat
{
    std::string name;
    int publishNum;
    int playerNum;
    vector<string> source;
};


struct ConsoleDat
{
    std::string name = "media server";
    RtmpSrvInfo rtmpSrvInfo;
    RtspSrvInfo rtspSrvInfo;
};


HttpRe makeConsoleDat(cJSON** proot){

    ConsoleDat clDat;
    clDat.name = "media server";

    clDat.rtmpSrvInfo = pMediaSrv->getRtmpSrvInfo();
    clDat.rtspSrvInfo = pMediaSrv->getRtspSrvInfo();


    cJSON * root =  cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString(clDat.name.c_str()));
    {
        RtmpSrvInfo& rtmpSrvInfo = clDat.rtmpSrvInfo;

        cJSON * host =  cJSON_CreateArray();
        cJSON_AddItemToObject(root, "host", host);
        {
            uint32_t publishNum=0, playerNum=0;
            cJSON *hostItem = cJSON_CreateObject();
            cJSON_AddItemToArray(host, hostItem);
            cJSON_AddStringToObject(hostItem, "name",       rtmpSrvInfo.hostName.c_str());
            
            cJSON * sourceArr =  cJSON_CreateArray();
            cJSON_AddItemToObject(hostItem, "source", sourceArr);
            std::map<std::string, RtmpSourceInfo>::iterator iter = rtmpSrvInfo.sourceInfo.begin();
            while(iter != rtmpSrvInfo.sourceInfo.end() ){
                cJSON * sourceIterm =  cJSON_CreateObject();
                cJSON_AddItemToArray(sourceArr, sourceIterm);
                cJSON_AddStringToObject(sourceIterm,    "srvName", iter->second.srvName.c_str());
                cJSON_AddBoolToObject(  sourceIterm,    "bRecord", iter->second.bRecord);
                std::map<std::string, RtmpPlayerInfo>::iterator iterPlayer = iter->second.playerInfo.begin();
                cJSON * PlayerArr =  cJSON_CreateArray();
                cJSON_AddItemToObject(sourceIterm,  "player", PlayerArr);
                while(iterPlayer != iter->second.playerInfo.end() ){
                    cJSON_AddItemToArray(PlayerArr, cJSON_CreateString(iterPlayer->second.name.c_str()));
                    playerNum++;
                    iterPlayer++;
                }
                publishNum++;
                iter++;
            }
            cJSON_AddNumberToObject(hostItem, "publishNum", publishNum);
            cJSON_AddNumberToObject(hostItem, "playerNum",  playerNum);

        }
        
        
        
        
        {
            RtspSrvInfo& rtspSrvInfo = clDat.rtspSrvInfo;

            cJSON *hostItem = cJSON_CreateObject();
            cJSON_AddItemToArray(host, hostItem);
            cJSON_AddStringToObject(hostItem, "name",       rtspSrvInfo.hostName.c_str());
            cJSON_AddNumberToObject(hostItem, "rxBytes",    rtspSrvInfo.rxBytes);
            cJSON_AddNumberToObject(hostItem, "txBytes",    rtspSrvInfo.txBytes);
            
            cJSON * sourceArr =  cJSON_CreateArray();
            cJSON_AddItemToObject(hostItem, "source", sourceArr);
            for(auto& x: rtspSrvInfo.sourceInfo){
                RtspSourceInfo source = x.second;
                cJSON * sourceIterm =  cJSON_CreateObject();
                cJSON_AddItemToArray(sourceArr, sourceIterm);
                cJSON_AddStringToObject(sourceIterm,    "name", source.name.c_str());
                
                cJSON * PlayerArr =  cJSON_CreateArray();
                cJSON_AddItemToObject(sourceIterm,  "client", PlayerArr);
                for(auto& y: source.playerInfo){
                    RtspClientInfo client = y.second;
                    cJSON * clientIterm =  cJSON_CreateObject();
                    cJSON_AddItemToArray(PlayerArr, clientIterm);
                    cJSON_AddStringToObject(clientIterm,    "name", client.name.c_str());
                    cJSON_AddStringToObject(clientIterm,    "info", client.info.c_str());
                }
            }
        }
    }

    *proot = root;
    return HTTPRE_OK;
}

HttpRe httpApp_Console(http_session_t* session) {

    cJSON * root;
    makeConsoleDat(&root);
    const std::string reply(cJSON_Print(root));

    http_server_set_header(session, "content-type", "text/json");
    http_server_set_header(session, "Access-Control-Allow-Origin", "*");
    http_server_set_header(session, "Access-Control-Allow-Methods", "GET, POST, PUT");
    http_server_reply(session, 200, reply.c_str(), reply.length());

    cJSON_free(root);
    return HTTPRE_OK;
}






#if 0

/*
    {
        "name":"media server",
        "host":[
            {
                "name":"rtmpHost",
                "publishNum":1,
                "playerNum":2,
                "source":[
                    "live1","live2"
                ]
            },
            {
                "name":"rtspHost",
                "publishNum":1,
                "playerNum":2,
                "source":[
                    "rtsplive1","rtsplive2"
                ]
            }
        ]
    }
*/

#include "http-server-internal.h"
#include "HttpUtil.h"
#include "HttpRoute.h"
#include "cJSON.h"

#include <string>
#include <fstream>
#include <vector>

#include "RtmpSrv.h"

using namespace std;

extern const string htmlRootPath;
extern int http_server_reply(http_session_t* session, int code, const void* data, size_t bytes);

struct HostDat
{
    std::string name;
    int publishNum;
    int playerNum;
    vector<string> source;
};


struct ConsoleDat
{
    std::string name = "media server";
    HostDat hostRtmpSrvDat;
    HostDat hostRtspSrvDat;
};


HttpRe makeConsoleDat(cJSON** proot){

    ConsoleDat clDat;
    {
        clDat.name = "media server";
        clDat.hostRtmpSrvDat.name          =   "rtmpHost";
        clDat.hostRtmpSrvDat.playerNum     =   1;
        clDat.hostRtmpSrvDat.publishNum    =   3;
        clDat.hostRtmpSrvDat.source.push_back("source1");
        clDat.hostRtmpSrvDat.source.push_back("source2");
        clDat.hostRtmpSrvDat.source.push_back("source3");
        clDat.hostRtmpSrvDat.source.push_back("source4");
        clDat.hostRtmpSrvDat.source.push_back("source5");
        clDat.hostRtmpSrvDat.source.push_back("source6");
        clDat.hostRtmpSrvDat.source.push_back("source7");
        clDat.hostRtmpSrvDat.source.push_back("source8");
        clDat.hostRtmpSrvDat.source.push_back("source9");
        clDat.hostRtmpSrvDat.source.push_back("source10");
        clDat.hostRtmpSrvDat.source.push_back("source11");
        clDat.hostRtmpSrvDat.source.push_back("source12");

        clDat.hostRtspSrvDat.name          =   "rtspHost";
        clDat.hostRtspSrvDat.playerNum     =   111;
        clDat.hostRtspSrvDat.publishNum    =   3222;
        clDat.hostRtspSrvDat.source.push_back("source1");
        clDat.hostRtspSrvDat.source.push_back("source2");
    }

    cJSON * root =  cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString(clDat.name.c_str()));
    {
        cJSON * host =  cJSON_CreateArray();
        cJSON_AddItemToObject(root, "host", host);
        {
            cJSON *hostItem = cJSON_CreateObject();
            cJSON_AddItemToArray(host, hostItem);
            cJSON_AddStringToObject(hostItem, "name",       clDat.hostRtmpSrvDat.name.c_str());
            cJSON_AddNumberToObject(hostItem, "publishNum", clDat.hostRtmpSrvDat.publishNum);
            cJSON_AddNumberToObject(hostItem, "playerNum",  clDat.hostRtmpSrvDat.playerNum);
            
            cJSON * source =  cJSON_CreateArray();
            cJSON_AddItemToObject(hostItem, "source", source);
            for(auto& x: clDat.hostRtmpSrvDat.source){
            cJSON_AddItemToArray(source, cJSON_CreateString(x.c_str()));
            }
        }
        {
            cJSON *hostItem = cJSON_CreateObject();
            cJSON_AddItemToArray(host, hostItem);
            cJSON_AddStringToObject(hostItem, "name",       clDat.hostRtspSrvDat.name.c_str());
            cJSON_AddNumberToObject(hostItem, "publishNum", clDat.hostRtspSrvDat.publishNum);
            cJSON_AddNumberToObject(hostItem, "playerNum",  clDat.hostRtspSrvDat.playerNum);
            
            cJSON * source =  cJSON_CreateArray();
            cJSON_AddItemToObject(hostItem, "source", source);
            for(auto& x: clDat.hostRtspSrvDat.source){
            cJSON_AddItemToArray(source, cJSON_CreateString(x.c_str()));
            }
        }
    }

    *proot = root;
    return HTTPRE_OK;
}

HttpRe httpApp_Console(http_session_t* session) {

    cJSON * root;
    makeConsoleDat(&root);
    const std::string reply(cJSON_Print(root));

    http_server_set_header(session, "content-type", "text/json");
    http_server_set_header(session, "Access-Control-Allow-Origin", "*");
    http_server_set_header(session, "Access-Control-Allow-Methods", "GET, POST, PUT");
    http_server_reply(session, 200, reply.c_str(), reply.length());

    cJSON_free(root);
    return HTTPRE_OK;
}

#endif