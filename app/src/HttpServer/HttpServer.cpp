
#include <fstream>

#include "HttpServer.h"
#include "HttpRoute.h"
#include "http-session.h"
// #include "http-server-internal.h"
// #include "http-parser.h"
#include "sys/system.h"
#include "sys/path.h"
#ifdef HLS
#include "hls-param.h"
#endif
#include "utf8codec.h"

#include "xlog.h"

#include "CorThread.h"

#include "sockutil.h"


#include "RtmpSrv.h"

#include "sys/sync.hpp"

using namespace std;

const string htmlRootPath = "../app/html";

extern "C" int http_list_dir(http_session_t* session, const char* path);

HttpServer::HttpServer()
{
}

HttpServer::~HttpServer()
{

}

static int http_server_onwebsocket(void* param, http_websocket_t* ws, const char* path, const char* subprotocols, void** wsparam)
{
	printf("WS onupgrade path: %s, subprotocols: %s\n", path ? path : "<nil>", subprotocols ? subprotocols : "<nil>");
	return 0;
}

static void http_server_test_ws_ondestroy(void* param)
{
}

static int http_server_test_ws_onsend(void* param, int code, size_t bytes)
{
	printf("WS send: %u\n", (unsigned int)bytes);
	assert(0 == code);
	return 0;
}

static int http_server_test_ws_ondata(void* param, int opcode, const void* data, size_t bytes, int flags)
{
	printf("WS opcode: %d, bytes: %u, flags: %x\n", opcode, (unsigned int)bytes, flags);
	return websocket_send((struct http_websocket_t*)param, opcode, data, bytes);
}

#define CWD "."

#if 0

static int http_server_ondownload(void* /*http*/, http_session_t* session, const char* /*method*/, const char* path)
{
	char header[1024];

	path += 1; // map "/download/*" to current path
	if (path_testfile(path))
	{
		// add your MIME
		//http_server_set_content_type(session, "text/html");

		// choose transfer encoding
		//http_server_set_header(session, "Transfer-Encoding", "chunked");

		// map server filename to download filename
		const char* name = path_basename(path);
		size_t n = snprintf(header, sizeof(header), "attachment; filename=\"%s\"", name);
		assert(n > 0 && n < sizeof(header));
		http_server_set_header(session, "Content-Disposition", name);

		return http_server_sendfile(session, path, NULL, NULL);
	}
	else{
		char buf[256];   
		getcwd(buf,sizeof(buf));  
		LOG_WARN("path %s 404\n", path);
	}

	http_server_set_status_code(session, 404, NULL);
	return http_server_send(session, "", 0, NULL, NULL);
}

static int http_server_onvod(void* /*http*/, http_session_t* session, const char* /*method*/, const char* path)
{
	UTF8Decode utf8(path + 5 /* /vod/ */);
	std::string fullpath = std::string(CWD) + "/";
	fullpath += utf8;
	LOG_INFO("http_server_onvod: %s\n", fullpath.c_str());

	if (path_testdir(fullpath.c_str()))
	{
		return http_list_dir(session, fullpath.c_str());
	}
	else if (path_testfile(fullpath.c_str()))
	{
		http_server_set_header(session, "Access-Control-Allow-Origin", "*");
		http_server_set_header(session, "Access-Control-Allow-Methods", "GET, POST, PUT");
		//http_server_set_header(session, "Transfer-Encoding", "chunked");
		if (std::string::npos != fullpath.find(".m3u8"))
			http_server_set_header(session, "content-type", HLS_M3U8_TYPE);
		else if (std::string::npos != fullpath.find(".mpd"))
			http_server_set_header(session, "content-type", "application/dash+xml");
		else if (std::string::npos != fullpath.find(".mp4") || std::string::npos != fullpath.find(".m4v"))
			http_server_set_header(session, "content-type", "video/mp4");
		else if (std::string::npos != fullpath.find(".m4a"))
			http_server_set_header(session, "content-type", "audio/mp4"); 
		return http_server_sendfile(session, fullpath.c_str(), NULL, NULL);
	}

	http_server_set_status_code(session, 404, NULL);
	return http_server_send(session, "", 0, NULL, NULL);
}

static int http_server_onroot(void* /*http*/, http_session_t* session, const char* /*method*/, const char* path)
{
	UTF8Decode utf8(path + 5 /* /vod/ */);
	std::string fullpath = std::string("../app/html") + "/";
	fullpath += utf8;
    char buf[256] = {0};
	LOG_INFO("http_server_onroot: %s\n", fullpath.c_str());
    getcwd(buf, sizeof(buf));
    string indexHtmlPath = string(buf) + "/" + htmlRootPath + "/doc/index.html";

    ifstream srcFile(indexHtmlPath, ios::in);
    if(srcFile.is_open()){
        const std::string reply((std::istreambuf_iterator<char>(srcFile)),
                           (std::istreambuf_iterator<char>()));

        http_server_set_header(session, "content-type", "text/html");
        http_server_set_header(session, "Access-Control-Allow-Origin", "*");
        http_server_set_header(session, "Access-Control-Allow-Methods", "GET, POST, PUT");
        http_server_reply(session, 200, reply.c_str(), reply.length());

        return 0;
    }

    LOG_WARN("open %s err,404\n", indexHtmlPath.c_str());
	http_server_set_status_code(session, 404, NULL);
	return http_server_send(session, "", 0, NULL, NULL);
}

#endif

extern HttpRouteNode httpRootNode[];

static int httpCorWritev(http_session_t* session, socket_bufvec_t *vec, int n){
	return cor_writev(http_session_get_netfd(session), vec,n, 2000);
}

void HttpServer::handle_session_thread(void* arg){
	cor_netfd_t* pCliFd = (cor_netfd_t*)arg;
    struct sockaddr_in* from = (struct sockaddr_in*)cor_netfd_getspecific(*pCliFd);
	struct http_session_t* session = http_session_create(pHttpSrv, *pCliFd, (struct sockaddr*)from, sizeof(struct sockaddr));
	usr_assert(session);
	http_session_set_writeCallBck(session, httpCorWritev);
    int ire = 0;
    #define RECV_MAX_LEN 8 * 1024 * 1024
    char* packet = http_session_get_recvDatAddr(session);
    while (true){
		packet = http_session_get_recvDatAddr(session);//更新dat指针位置
        ire = cor_read(*pCliFd, packet, HTTP_RECV_BUFFER, COR_UTIME_NO_TIMEOUT); 
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
		// 传入0表示读取成功
		http_session_handle_recv(session, 0, ire);//[todo] 中间的0是不是可以省略了
    }
	cor_netfd_close(*pCliFd);
    delete pCliFd;
	http_session_handle_destroy(session);
    LOG_WARN("handle session thread exit\n");
}

void HttpServer::handle_session(cor_netfd_t _cliSock){
	cor_netfd_t* pCliFd = new cor_netfd_t;
	*pCliFd = _cliSock;
    CorThread* ctdCli = new CorThread("cli session", std::bind(&HttpServer::handle_session_thread,this,std::placeholders::_1), pCliFd);
    usr_assert(ctdCli->start());
}

void HttpServer::accept_cor_thread(void* arg) {
    // cor_set_thread_desc((char*)"accept coroutine thread");
    struct sockaddr_in from;
    int fromlen;

    fromlen = sizeof(from);
    while (1) {
        cor_netfd_t cli_nfd = cor_accept(corSrvFd, (struct sockaddr*)&from, &fromlen, COR_UTIME_NO_TIMEOUT);
        if (cli_nfd == NULL) {
            int errNoSave = errno;
            LOG_ERR("can't accept connection: %s \n", getErrnoDesc(errNoSave));
            continue;
        }
        /* Save peer address, so we can retrieve it later */
        cor_netfd_setspecific(cli_nfd, &from, nullptr);
        handle_session(cli_nfd);
    }
	cor_netfd_close(corSrvFd);
	http_server_destroy(pHttpSrv);
    LOG_INFO("accept exit\n");
}

bool HttpServer::start(const char* ip, int port){
    LOG_INFO("http server start %s:%d\n",ip==nullptr?"0.0.0.0":ip, port);

    corSrvFd = cor_tcp_listen_ipv4(ip, port, 1000);
    usr_assert(corSrvFd != COR_NETFD_ERR);
    // 在accept结束后释放
	pHttpSrv = http_server_create(corSrvFd);
	sethttpRouteRootNode(httpRootNode);
	http_server_set_handler(pHttpSrv, httpRoute, pHttpSrv);//[todo] 参数冗余，但是在其他地方用的比较多，暂时放在这里吧

	// http_server_addroute("/download/", http_server_ondownload);
    // http_server_addroute("/vod/", http_server_onvod);
    // http_server_addroute("/", http_server_onroot);

	struct websocket_handler_t *phandler = (struct websocket_handler_t*)malloc(sizeof(struct websocket_handler_t));
	phandler->onupgrade = http_server_onwebsocket;
	phandler->ondestroy = http_server_test_ws_ondestroy;
	phandler->onsend = http_server_test_ws_onsend;
	phandler->ondata = http_server_test_ws_ondata;
	http_server_websocket_sethandler(pHttpSrv, phandler, NULL);
	free(phandler);

    CorThread* ctdAccept = new CorThread("accept coroutine", std::bind(&HttpServer::accept_cor_thread, this, std::placeholders::_1), nullptr);
    ctdAccept->start();

    return true;
}
