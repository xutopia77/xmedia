
#ifndef _HttpRoute_H
#define _HttpRoute_H

#include <functional>
#include <vector>

#include "HttpUtil.h"
#include "http-server.h"

typedef std::function<HttpRe(http_session_t* session)> RouteFunc; 

struct HttpRouteNode{
    const char* url;
    RouteFunc main;
    HttpRouteNode* preNode;
    HttpRouteNode* nextNode;
};

int sethttpRouteRootNode(HttpRouteNode* node);
int httpRoute(void* param, http_session_t* session, const char* method, const char* path);

#if defined(__cplusplus)
extern "C" {
#endif

int http_server_reply(http_session_t* session, int code, const void* data, size_t bytes);

#if defined(__cplusplus)
}
#endif

#endif
