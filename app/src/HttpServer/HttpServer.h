
#ifndef _HttpServer_H
#define _HttpServer_H

#include <string>

#include "http-server.h"

class HttpServer
{
private:
    std::string rootPath = ".";
    http_server_t* pHttpSrv;
    cor_netfd_t corSrvFd;
public:
    HttpServer();
    ~HttpServer();
    void handle_session(cor_netfd_t _cliSock);
    void handle_session_thread(void* arg);
    void accept_cor_thread(void* arg);
    bool start(const char* ip, int port);
};

#endif
