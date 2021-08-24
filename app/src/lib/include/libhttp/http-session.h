#ifndef _HTTP_SESSION_h_
#define _HTTP_SESSION_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/sock.h"

typedef struct _st_netfd * cor_netfd_t;
typedef struct http_session_t http_session_t;

#define HTTP_RECV_BUFFER		(2*1024)

cor_netfd_t http_session_get_netfd(http_session_t* session);
http_session_t* http_session_create(void *server, cor_netfd_t cliFd, const struct sockaddr* sa, socklen_t salen);

void http_session_handle_destroy(http_session_t* session);

void http_session_handle_recv(http_session_t* session, int code, size_t bytes);

char* http_session_get_recvDatAddr(http_session_t* session);

typedef int httpCorWritevType(http_session_t* session, socket_bufvec_t *vec, int n);

void http_session_set_writeCallBck(http_session_t* session, httpCorWritevType func);

const char* http_session_get_request_uri(http_session_t* session);

#ifdef __cplusplus
}
#endif
#endif
