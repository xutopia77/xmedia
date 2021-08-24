#include "sockutil.h"
#include "rtmp-client.h"
#include "flv-writer.h"
#include "flv-proto.h"
#include <assert.h>
#include <stdio.h>

#include "rtmp-session.h"
#include "CorThread.h"
#include "xlog.h"
#include <string>
#include "AppCfg.h"

using namespace std;

static void* s_flv;

static int rtmp_client_send(cor_netfd_t _corCliFd, const void* header, size_t len, const void* data, size_t bytes){
    cor_netfd_t corCliFd = _corCliFd;

	socket_bufvec_t vec[2];
	socket_setbufvec(vec, 0, (void*)header, len);
	socket_setbufvec(vec, 1, (void*)data, bytes);
    return cor_writev(corCliFd, vec, bytes > 0 ? 2 : 1, 2000);
	// return socket_send_v_all_by_time(*socket, vec, bytes > 0 ? 2 : 1, 0, 2000);
}

static int rtmp_client_onaudio(void* /*param*/, const void* data, size_t bytes, uint32_t timestamp)
{
	return flv_writer_input(s_flv, FLV_TYPE_AUDIO, data, bytes, timestamp);
}

static int rtmp_client_onvideo(void* /*param*/, const void* data, size_t bytes, uint32_t timestamp)
{
	// LOG_RTMP_DBG("[v]timestamp %u\n", timestamp);
	return flv_writer_input(s_flv, FLV_TYPE_VIDEO, data, bytes, timestamp);
}

static int rtmp_client_onscript(void* /*param*/, const void* data, size_t bytes, uint32_t timestamp)
{
	return flv_writer_input(s_flv, FLV_TYPE_SCRIPT, data, bytes, timestamp);
}


// rtmp://live.alivecdn.com/live/hello?key=xxxxxx
// rtmp_publish_aio_test("live.alivecdn.com", "live", "hello?key=xxxxxx", save-to-local-flv-file-name)
void _rtmp_play_test(const char* _host, const char* app, const char* stream, const char* flv)
{
	RtmpRe re;
    /* Initialize the ST library */
    if (cor_thread_init() < 0)
        fatal("initialization failed: st_init\n");

	char* host = ((_host==NULL)?(char*)"0.0.0.0":(char*)_host);
	host = ((*host=='\0')?(char*)"0.0.0.0":host);
	host = ((string(host)=="localhost")?(char*)"0.0.0.0":host);
	uint16_t srvPort = 1935;
	cor_netfd_t corCliFd = cor_connect_host(host, srvPort, 2000);
	if(corCliFd == COR_NETFD_ERR){
		fatal("connect %s:%d err\n", host, srvPort, _host);
	}
	
	#define RECV_MAX_LEN  8*1024*1024
	static char packet[RECV_MAX_LEN];
	snprintf(packet, sizeof(packet), "rtmp://%s/%s", host, app); // tcurl

	// socket_t socket = socket_connect_host(host, 1935, 2000);
	// socket_setnonblock(socket, 0);

	struct rtmp_client_handler_t handler;
	memset(&handler, 0, sizeof(handler));
	handler.send = 			rtmp_client_send;
	handler.onaudio = 		rtmp_client_onaudio;
	handler.onvideo = 		rtmp_client_onvideo;
	handler.onscript = 		rtmp_client_onscript;
	rtmp_client_t* rtmp = 	rtmp_client_create(app, stream, packet/*tcurl*/, nullptr, &handler);

	rtmp_session_t* session = rtmp_session_create(corCliFd, rtmp);
	rtmp_client_session_init(session);

	s_flv = flv_writer_create(flv);

	int r = rtmp_client_start(session, 1);
	usr_assert(r==0);

	int ire = 0;
	struct sockaddr_in* from = (struct sockaddr_in*)cor_netfd_getspecific(rtmp_session_get_netfd(session));
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
            LOG_DBG("%s:%d disconnect\n", inet_ntoa((*from).sin_addr), (*from).sin_port);
            break;
        }
        if( (re = rtmp_client_input(session, (unsigned char*)packet, ire)).code != RTMP_CODE_OK){
			fatal("client input err %s\n", re.desc);
		}
    }

	rtmp_client_stop(session);
	flv_writer_destroy(s_flv);

	cor_netfd_close(corCliFd);
	rtmp_session_destroy(session);
	rtmp_client_destroy(rtmp);

    cor_thread_exit(nullptr);
    LOG_INFO("client play exit\n");

}
