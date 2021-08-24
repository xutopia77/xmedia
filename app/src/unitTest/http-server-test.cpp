
#include "http-server.h"

#include "sys/thread.h"
#include "sys/system.h"
#include "sys/path.h"
#include <assert.h>
#include "CorThread.h"

#include "HttpRoute.h"


static HttpRe http_server_ondownload(http_session_t* session)
{
	char header[1024];

	//  (info.st_mode&S_IFREG))


	char *path; // map "/download/*" to current path
	path = (char*)http_session_get_request_uri(session);
	path += 1;

	struct stat info;
	stat(path, &info);
	 
	LOG_DBG("%s %x %x %x\n", path, info.st_mode ,info.st_mode&S_IFREG ,S_IFREG);

	if (path_testfile(path)){
		// add your MIME
		//http_server_set_content_type(session, "text/html");

		// choose transfer encoding
		//http_server_set_header(session, "Transfer-Encoding", "chunked");

		// map server filename to download filename
		const char* name = path_basename(path);
		long unsigned int n = snprintf(header, sizeof(header), "attachment; filename=\"%s\"", name);
		assert(n > 0 && n < sizeof(header));
		http_server_set_header(session, "Content-Disposition", name);
	
		if(http_server_sendfile(session, path, NULL, NULL) > 0 ){
			return HTTPRE_OK;
		}
		else{
			return HTTPRE_ERR;
		}
	}
	else{
		char buf[256];   
		getcwd(buf,sizeof(buf));  
		LOG_WARN("path %s 404\n", path);
	}

	http_server_set_status_code(session, 404, NULL);

	if( http_server_send(session, "", 0, NULL, NULL) > 0 ){
		return HTTPRE_OK;
	}
	else{
		return HTTPRE_ERR;
	}
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





























extern HttpRe httpApp_doc(http_session_t* session);
extern HttpRe httpApp_index(http_session_t* session);


static HttpRouteNode sg_httpRootNode[]=
{
    {"apiv1_1_tmp",     nullptr,        nullptr,    nullptr},
    {"/download/",           http_server_ondownload,     nullptr,    nullptr},
    {"/",           httpApp_index,     nullptr,    nullptr},
    {"doc",           httpApp_doc,     nullptr,    nullptr},
};






static cor_netfd_t sg_corSrvFd;
static http_server_t* sg_pHttpSrv;

static int httpCorWritev(struct http_session_t* session, socket_bufvec_t *vec, int n){
	return cor_writev(http_session_get_netfd(session), vec,n, 2000);
}

static void* handle_session_thread(void* arg){
	cor_netfd_t* pCliFd = (cor_netfd_t*)arg;
    struct sockaddr_in* from = (struct sockaddr_in*)cor_netfd_getspecific(*pCliFd);
	struct http_session_t* session = http_session_create(sg_pHttpSrv, *pCliFd, (struct sockaddr*)from, sizeof(struct sockaddr));
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
	return NULL;
}

void handle_session(cor_netfd_t _cliSock){
	cor_netfd_t* pCliFd = new cor_netfd_t;
	*pCliFd = _cliSock;
	cor_thread_create(handle_session_thread, pCliFd, 0, 0);
}

void* accept_cor_thread(void* arg) {
    // cor_set_thread_desc((char*)"accept coroutine thread");
    struct sockaddr_in from;
    int fromlen;

    fromlen = sizeof(from);
    while (1) {
        cor_netfd_t cli_nfd = cor_accept(sg_corSrvFd, (struct sockaddr*)&from, &fromlen, COR_UTIME_NO_TIMEOUT);
        if (cli_nfd == NULL) {
            int errNoSave = errno;
            LOG_ERR("can't accept connection: %s \n", getErrnoDesc(errNoSave));
            continue;
        }
        /* Save peer address, so we can retrieve it later */
        cor_netfd_setspecific(cli_nfd, &from, nullptr);
        handle_session(cli_nfd);
    }
	cor_netfd_close(sg_corSrvFd);
	http_server_destroy(sg_pHttpSrv);
    LOG_INFO("accept exit\n");
}

void _http_server_test(const char* ip, int port)
{
	/* Initialize the ST library */
	if (cor_thread_init() < 0)
		fatal("initialization failed: st_init\n");

    sg_corSrvFd = cor_tcp_listen_ipv4(ip, port, 1000);
    usr_assert(sg_corSrvFd != COR_NETFD_ERR);

	sg_pHttpSrv = http_server_create(sg_corSrvFd);
	sethttpRouteRootNode(sg_httpRootNode);
	
	struct websocket_handler_t handler;
	handler.onupgrade = http_server_onwebsocket;
	handler.ondestroy = http_server_test_ws_ondestroy;
	handler.onsend = http_server_test_ws_onsend;
	handler.ondata = http_server_test_ws_ondata;
	http_server_websocket_sethandler(sg_pHttpSrv, &handler, NULL);

	cor_thread_create(accept_cor_thread, sg_corSrvFd, 0, 0);


	cor_thread_exit(nullptr);	

	http_server_destroy(sg_pHttpSrv);
}

int http_server_test(int argc, char const *argv[]){
	_http_server_test("0.0.0.0", 8000);
	// LOG_WARN("coming soon\n");
	return 0;
}
