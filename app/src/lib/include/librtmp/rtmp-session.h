
#ifndef _RTMP_SESSION_H
#define _RTMP_SESSION_H

#include <stdint.h>
#include <stddef.h>

typedef struct _st_netfd * cor_netfd_t;
typedef struct rtmp_session_t rtmp_session_t;

#ifndef COR_NETFD_ERR
#define COR_NETFD_ERR NULL
#endif


rtmp_session_t* rtmp_session_create(cor_netfd_t _cliSock, void* srvOrCli);
void rtmp_srv_session_init(rtmp_session_t* session);
void rtmp_client_session_init(rtmp_session_t* session);

void rtmp_session_destroy(rtmp_session_t* session);
cor_netfd_t rtmp_session_get_netfd(rtmp_session_t* session);
void* rtmp_session_get_sourceOrPlayer(rtmp_session_t* session);
void rtmp_session_set_sourceOrPlayer(rtmp_session_t* session, void* sourceOrPlayer);


#endif