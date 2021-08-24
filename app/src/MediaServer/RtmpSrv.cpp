
#include "RtmpSrv.h"

#include <string.h>

#include "rtmp-server.h"
#include "rtmp-util.h"

#include "CorThread.h"
#include "xlog.h"
#include "AppCfg.h"
#include "RtmpSource.h"
#include "CorRe.h"

using namespace std;

extern shared_ptr<RtmpSrv> g_rtmpSrv;

RtmpSrv::RtmpSrv()
{
}

RtmpSrv::~RtmpSrv()
{

}

void RtmpSrv::handle_session_thread(void* arg){
    CorRe re;
    CorRe tmpre;
    rtmp_session_t* session = (rtmp_session_t*)arg;
    struct sockaddr_in* from = (struct sockaddr_in*)cor_netfd_getspecific(rtmp_session_get_netfd(session));
    int ire = 0;
    #define RECV_MAX_LEN 8 * 1024 * 1024
    char* packet = new char[RECV_MAX_LEN];
    while (true){
        ire = cor_read(rtmp_session_get_netfd(session), packet, RECV_MAX_LEN, COR_UTIME_NO_TIMEOUT); 
        if ( ire < 0) {
            int errnoSave = errno;
            if(errnoSave == 104){
                // hostInfo->leaveCount++;//对端关闭不算错误
            }else{
                // hostInfo->errCount++;
            }
            LOG_ERR("can't read request from %s:%d: cor_read [%d]%s\n", inet_ntoa((*from).sin_addr), (*from).sin_port, errnoSave, getErrnoDesc(errnoSave));
            break;
        }
        if ( ire == 0) {
            tmpre = CORRE_OK_DESC(string(inet_ntoa((*from).sin_addr)) + to_string((*from).sin_port) + "disconnect");
            break;
        }
        usr_assert(0 == rtmp_server_input(session, (unsigned char*)packet, ire));
    }

    cor_netfd_close(rtmp_session_get_netfd(session));
    rtmp_session_destroy(session);

    delete[] packet;
    
    // LOG_WARN("handle session thread exit, %s %s\n", tmpre.desc().c_str(), re.desc().c_str());
    LOG_WARN("handle session thread exit, %s\n", tmpre.desc().c_str());
}

void RtmpSrv::handle_session(cor_netfd_t _cliSock){
    rtmp_session_t* session = rtmp_session_create(_cliSock, rtmpSrv);
    usr_assert(session != NULL);
    rtmp_srv_session_init(session);

    CorThread* ctdCli = new CorThread("cli session", std::bind(&RtmpSrv::handle_session_thread,this,std::placeholders::_1), session);
    usr_assert(ctdCli->start());
}

void RtmpSrv::accept_cor_thread(void* arg) {
    // cor_set_thread_desc((char*)"accept coroutine thread");
    struct sockaddr_in from;
    int fromlen;

    fromlen = sizeof(from);
    while (1) {
        cor_netfd_t cli_nfd = cor_accept(corSrvfd, (struct sockaddr*)&from, &fromlen, COR_UTIME_NO_TIMEOUT);
        if (cli_nfd == NULL) {
            int errNoSave = errno;
            LOG_ERR("can't accept connection: %s \n", getErrnoDesc(errNoSave));
            continue;
        }
        /* Save peer address, so we can retrieve it later */
        cor_netfd_setspecific(cli_nfd, &from, nullptr);
        handle_session(cli_nfd);
    }
	rtmp_server_destroy(rtmpSrv);
    cor_netfd_close(corSrvfd);    
    LOG_INFO("accept exit\n");
}

void RtmpSrv::statiscInfo(){
    RtmpSrvInfo info = getSrvInfo();
    std::map<std::string, RtmpSourceInfo>::iterator iter = info.sourceInfo.begin();
    RtmpSourceInfo sourceInfo;
    string sourceKey;
    RtmpPlayerInfo playerInfo;
    int sourceNum=0, perPlayerNum=0, totalPlayerNum=0;
    sourceNum = info.sourceInfo.size();
    if(srvCfg.staticPrtLevel > 1){
        LOG_INFO("%s statisc info: sourceNum=%d\n", info.hostName.c_str(), sourceNum);
    }
    char timestampBuf[64]={0};
    for (; iter != info.sourceInfo.end(); iter++)
    {
        sourceKey = iter->first;
        sourceInfo = iter->second;
        perPlayerNum = sourceInfo.playerInfo.size();
        totalPlayerNum += perPlayerNum;
        
        if(srvCfg.staticPrtLevel > 1){
            LOG_INFO_N("sourceInfo:\n");
            LOG_INFO_N("\tsource name: %s, bRecord:%s, playNum: %d\n", sourceInfo.srvName.c_str(), sourceInfo.bRecord ? "ture" : "false", perPlayerNum);
            LOG_INFO_N("\tV[ %d, %s, %f kB], A[ %d, %s, %f kB]\n", sourceInfo.rxVideoStartTime, convert_ms(sourceInfo.rxVideoTime, timestampBuf, sizeof(timestampBuf)), sourceInfo.rxVideoBytes / 1000.0f\
            , sourceInfo.rxAudioStartTime, convert_ms(sourceInfo.rxAudioTime, timestampBuf, sizeof(timestampBuf)), sourceInfo.rxAudioBytes / 1000.0f);
            std::map<std::string, RtmpPlayerInfo>::iterator playIter = sourceInfo.playerInfo.begin();
            for (; playIter != sourceInfo.playerInfo.end(); playIter++){
                playerInfo = playIter->second;
                LOG_INFO_N("\t\tplayer:%s\n", playerInfo.name.c_str());
            }
        }

    }
}


void RtmpSrv::statisc_cor_thread(void* arg) {
    // cor_set_thread_desc((char*)"static coroutine thread");
    for (;;) {
        // LOG_INFO("host statisc [%d %d %d]\n",
        // hostInfo.rqstCount, hostInfo.leaveCount, hostInfo.errCount);
        // LOG_INFO("static coroutine thread\n");
        statiscInfo();
        cor_sleep(2);
    }
}

static int rtmp_server_send(cor_netfd_t _corCliFd, const void* header, size_t len, const void* data, size_t bytes){
    cor_netfd_t corCliFd = _corCliFd;

	socket_bufvec_t vec[2];
	socket_setbufvec(vec, 0, (void*)header, len);
	socket_setbufvec(vec, 1, (void*)data, bytes);
    size_t sendBytes = cor_writev(corCliFd, vec, bytes > 0 ? 2 : 1, 2000);
    return sendBytes;
	// return socket_send_v_all_by_time(*socket, vec, bytes > 0 ? 2 : 1, 0, 2000);
}

static int rtmp_server_onpublish(rtmp_session_t* session, const char* app, const char* stream, const char* type){
	return g_rtmpSrv->onpublish(session, app, stream, type);
}
static int rtmp_server_onscript(rtmp_session_t* session, const void* script, size_t bytes, uint32_t timestamp){
    return g_rtmpSrv->onscript(session, script, bytes, timestamp);
}
static int rtmp_server_onvideo(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp){
    return g_rtmpSrv->onvideo(session, data, bytes, timestamp);
}
static int rtmp_server_onaudio(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp){
    return g_rtmpSrv->onaudio(session, data, bytes, timestamp);
}

static int rtmp_server_onplay(rtmp_session_t* session, const char* app, const char* stream, double start, double duration, uint8_t reset){
    return g_rtmpSrv->onplay(session, app, stream, start, duration, reset);
}
static int rtmp_server_onpause(rtmp_session_t* session, int pause, uint32_t ms){
    //[todo]
	printf("rtmp_server_onpause(%d, %u)\n", pause, (unsigned int)ms);
	return 0;
}
static int rtmp_server_onseek(rtmp_session_t* session, uint32_t ms){
	//[todo]
    printf("rtmp_server_onseek(%u)\n", (unsigned int)ms);
	return 0;
}
static void rtmp_session_before_destroy(rtmp_session_t* session){
    g_rtmpSrv->onclose(session);
}

bool RtmpSrv::start(const char* ip, int port) {
    if(g_rtmpSrv == nullptr){
        LOG_RTMP_ERR("rtmp server null\n");
        return false;
    }

    string srvIp; uint16_t srvPort;
    {
        if(nullptr == ip
        || string("localhost") == ip)   
            srvIp = "0.0.0.0";
        else
            srvIp = ip;
        srvPort = port;
        LOG_INFO("rtmp server start %s:%d\n", srvIp.c_str(), srvPort);

        struct rtmp_server_handler_t handler;
        memset(&handler, 0, sizeof(handler));
        handler.send = rtmp_server_send;
        //handler.oncreate_stream = rtmp_server_oncreate_stream;
        //handler.ondelete_stream = rtmp_server_ondelete_stream;
        handler.onplay = rtmp_server_onplay;
        handler.onpause = rtmp_server_onpause;
        handler.onseek = rtmp_server_onseek;
        handler.onpublish = rtmp_server_onpublish;
        handler.onscript                = rtmp_server_onscript;
        handler.onvideo                 = rtmp_server_onvideo;
        handler.onaudio                 = rtmp_server_onaudio;
        handler.session_before_destroy  = rtmp_session_before_destroy;
        //[todo] 这里传入的第一个参数，原来是客户端的socket，现在要更改了，具体更改为什么，需要视情况而定
        rtmpSrv = rtmp_server_create(nullptr, &handler);
    }

    corSrvfd = cor_tcp_listen_ipv4(srvIp.c_str(), srvPort, srvCfg.listenQSize==-1 ? LISTENQ_SIZE_DEFAULT : srvCfg.listenQSize);

    CorThread* ctdAccept = new CorThread("accept coroutine", std::bind(&RtmpSrv::accept_cor_thread, this, std::placeholders::_1), nullptr);
    ctdAccept->start();

    CorThread* ctdstatisc = new CorThread("accept coroutine", std::bind(&RtmpSrv::statisc_cor_thread, this, std::placeholders::_1), nullptr);
    ctdstatisc->start();

    return true;
}

int RtmpSrv::onpublish(rtmp_session_t* session, const char* app, const char* stream, const char* type){
    std::string key(app);
    key += "/";
    key += stream;
    string StrRecordFile = g_appCfg.datOutPath + "/srv_" + app + "_" + stream + ".flv";

    std::shared_ptr<RtmpSource> source = make_shared<RtmpSource>(StrRecordFile.c_str());
    source->key = key;
    rtmp_session_set_sourceOrPlayer(session, source.get());
    LOG_INFO("new publisher[%d](%s, %s, %s),  record:true, reocrdFileName:%s\n", sourceLives.size(), app, stream, type, StrRecordFile.c_str());
    AutoThreadLocker locker(sourceLocker);
    assert(sourceLives.find(key) == sourceLives.end());//[todo] 相同的推流上来了该怎么处理？
    sourceLives[key] = source;
    srvInfo.sourceInfo[key].srvName = key;
    srvInfo.sourceInfo[key].bRecord = true;
    return 0;
}

int RtmpSrv::onscript(rtmp_session_t* session,  const void* script, size_t bytes, uint32_t timestamp){
    void* sourceOrPlay = rtmp_session_get_sourceOrPlayer(session);
    if(sourceOrPlay != nullptr){
        RtmpSource* source = (RtmpSource*)sourceOrPlay;
        AutoThreadLocker locker(source->locker);
        source->doRecord(FLV_TYPE_SCRIPT, script, bytes, timestamp);
        return flv_demuxer_input(source->demuxer, FLV_TYPE_SCRIPT, script, bytes, timestamp);
    }
    return -1;
}

int RtmpSrv::onvideo(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp){
    void* sourceOrPlay = rtmp_session_get_sourceOrPlayer(session);
    if(sourceOrPlay != nullptr){
        RtmpSource* source = (RtmpSource*)sourceOrPlay;
        AutoThreadLocker locker(source->locker);
        // char timestampBuf[64]={0};
        // convert_ms( timestamp, timestampBuf,sizeof(timestampBuf));
        // LOG_RTMP_DBG("timestamp V: %s\n", timestampBuf);
        RtmpSourceInfo& info = getSrvInfo().sourceInfo[source->key];
        info.rxVideoBytes += bytes;
        if(-1 == info.rxVideoStartTime){
            info.rxVideoStartTime = timestamp;
        }
        info.rxVideoTime = timestamp;

        source->doRecord(FLV_TYPE_VIDEO, data, bytes, timestamp);
        return flv_demuxer_input(source->demuxer, FLV_TYPE_VIDEO, data, bytes, timestamp);
    }
    return -1;
}

int RtmpSrv::onaudio(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp){
    void* sourceOrPlay = rtmp_session_get_sourceOrPlayer(session);
    if(sourceOrPlay != nullptr){
        RtmpSource* source = (RtmpSource*)sourceOrPlay;
        AutoThreadLocker locker(source->locker);
        // char timestampBuf[64]={0};
        // convert_ms( timestamp, timestampBuf,sizeof(timestampBuf));
        // LOG_RTMP_DBG("timestamp A: %s\n", timestampBuf);
        RtmpSourceInfo& info = getSrvInfo().sourceInfo[source->key];
        info.rxAudioBytes += bytes;
        if(-1 == info.rxAudioStartTime){
            info.rxAudioStartTime = timestamp;
        }
        info.rxAudioTime = timestamp;

        source->doRecord(FLV_TYPE_AUDIO, data, bytes, timestamp);
        return flv_demuxer_input(source->demuxer, FLV_TYPE_AUDIO, data, bytes, timestamp);
    }
    return -1;
}

int RtmpSrv::onplay(rtmp_session_t* session, const char* app, const char* stream, double start, double duration, uint8_t reset){
    LOG_INFO("onplay(%s, %s, %f, %f, %d)\n", app, stream, start, duration, (int)reset);
    std::string key(app);
    key += "/";
    key += stream;

    std::shared_ptr<struct RtmpSource> s;
    {
        AutoThreadLocker locker(sourceLocker);
        auto it = sourceLives.find(key);
        if (it == sourceLives.end())
        {
            printf("source(%s, %s) not found\n", app, stream);
            return -1;
        }
        s = it->second;
    }
    
    std::shared_ptr<RtmpPlayer> player(new RtmpPlayer(session));
    AutoThreadLocker locker(s->locker);
    s->players.push_back(player);
    rtmp_session_set_sourceOrPlayer(session, player.get());
    char peerIp[65]={0};
    uint16_t peerPort;
    cor_getpeername(rtmp_session_get_netfd(session), peerIp, &peerPort);
    string playerName = string(peerIp) + ":" +to_string(peerPort);
    srvInfo.sourceInfo[key].playerInfo[playerName].name = playerName;
    return 0;
}

CorRe RtmpSrv::onclose(rtmp_session_t* session){
    CorRe re;
    void* sourceOPlay = rtmp_session_get_sourceOrPlayer(session);
    AutoThreadLocker locker(sourceLocker);
    for (auto it = sourceLives.begin(); it != sourceLives.end(); ++it){
        std::shared_ptr<RtmpSource>& s = it->second;
        if (sourceOPlay == s.get()){
            sourceLives.erase(it);
            srvInfo.sourceInfo.erase(it->first);
            re = CORRE_OK_DESC(it->first + " pusher leave");
            LOG_WARN(re.desc().c_str());
            return re;
        }

        AutoThreadLocker l(s->locker);
        for (auto j = s->players.begin(); j != s->players.end(); ++j)
        {
            if (j->get() == sourceOPlay){
                s->players.erase(j);
                re = CORRE_OK_DESC(it->first + " player leave");
                LOG_WARN(re.desc().c_str());
                return re;
            }
        }
    }
    re = CORRE_OK_DESC("unknow what is leave");
    LOG_WARN(re.desc().c_str());
    return re;
}


