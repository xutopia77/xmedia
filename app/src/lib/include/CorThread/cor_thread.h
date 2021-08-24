
#ifndef _COR_THREAD_H
#define _COR_THREAD_H


#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
// #include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
// #include <time.h>
// #include <errno.h>
#include <poll.h>
#include "stdint.h"

#define COR_NETFD_ERR NULL

typedef int     cor_threadid_t;
typedef struct _st_thread*   cor_thread_t;

cor_threadid_t   cor_get_threadid();

int        cor_set_thread_desc(char* desc);
char*      cor_get_thread_desc();





#define COR_VERSION	    "1.9"
#define COR_VERSION_MAJOR    1
#define COR_VERSION_MINOR    9

// /* Undefine this to remove the context switch callback feature. */
// #define ST_SWITCH_CB

// #ifndef ETIME
// #define ETIME ETIMEDOUT
// #endif

#ifndef COR_UTIME_NO_TIMEOUT
#define COR_UTIME_NO_TIMEOUT ((cor_utime_t) -1LL)
#endif

// #ifndef ST_UTIME_NO_WAIT
// #define ST_UTIME_NO_WAIT 0
// #endif

// #define ST_EVENTSYS_DEFAULT 0
// #define ST_EVENTSYS_SELECT  1
// #define ST_EVENTSYS_POLL    2
// #define ST_EVENTSYS_ALT     3


typedef unsigned long long  cor_utime_t;
typedef struct _st_thread * cor_thread_t;
typedef struct _st_cond *   cor_cond_t;
typedef struct _st_mutex *  cor_mutex_t;
typedef struct _st_netfd *  cor_netfd_t;
// #ifdef ST_SWITCH_CB
// typedef void (*st_switch_cb_t)(void);
// #endif

/**
 * 
*/
int cor_thread_init();
int cor_getfdlimit(void);

int cor_set_eventsys(int eventsys);
int cor_get_eventsys(void);
const char *cor_get_eventsys_name(void);

// #ifdef ST_SWITCH_CB
// extern st_switch_cb_t st_set_switch_in_cb(st_switch_cb_t cb);
// extern st_switch_cb_t st_set_switch_out_cb(st_switch_cb_t cb);
// #endif

// extern st_thread_t st_thread_self(void);
void cor_thread_exit(void *retval);
int cor_thread_join(cor_thread_t thread, void **retvalp);
void cor_thread_interrupt(cor_thread_t thread);
cor_thread_t cor_thread_create(void *(*start)(void *arg), void *arg, int joinable, int stk_size);

int cor_randomize_stacks(int on);
int cor_set_utime_function(cor_utime_t (*func)(void));

cor_utime_t cor_utime(void);
cor_utime_t cor_utime_last_clock(void);
int cor_timecache_set(int on);
time_t cor_time(void);
int cor_usleep(cor_utime_t usecs);
int cor_sleep(int secs);
cor_cond_t st_cond_new(void);
int cor_cond_destroy(cor_cond_t cvar);
int cor_cond_timedwait(cor_cond_t cvar, cor_utime_t timeout);
int cor_cond_wait(cor_cond_t cvar);
int cor_cond_signal(cor_cond_t cvar);
int cor_cond_broadcast(cor_cond_t cvar);
cor_mutex_t cor_mutex_new(void);
int cor_mutex_destroy(cor_mutex_t lock);
int cor_mutex_lock(cor_mutex_t lock);
int cor_mutex_unlock(cor_mutex_t lock);
int cor_mutex_trylock(cor_mutex_t lock);

// int st_key_create(int *keyp, void (*destructor)(void *));
// int st_key_getlimit(void);
// int st_thread_setspecific(int key, void *value);
// void *st_thread_getspecific(int key);

cor_netfd_t cor_netfd_open(int osfd);
cor_netfd_t cor_netfd_open_socket(int osfd);
void cor_netfd_free(cor_netfd_t fd);
int cor_netfd_close(cor_netfd_t fd);
int cor_netfd_fileno(cor_netfd_t fd);
void cor_netfd_setspecific(cor_netfd_t fd, void *value, void (*destructor)(void *));
void *cor_netfd_getspecific(cor_netfd_t fd);
int cor_netfd_serialize_accept(cor_netfd_t fd);
int cor_netfd_poll(cor_netfd_t fd, int how, cor_utime_t timeout);

int cor_poll(struct pollfd *pds, int npds, cor_utime_t timeout);

/**
  * wait for client connection
  * @param[in] fd server socket(must be bound and listening)
  * @param[out] addr struct sockaddr_in for IPv4
  * @param[out] addrlen addr length in bytes
  * @param[in] timeout timeout in millisecond, COR_UTIME_NO_TIMEOUT for ever
  * @return COR_NETFD_ERR :err, other succ. 
 */
cor_netfd_t cor_accept(cor_netfd_t fd, struct sockaddr *addr, int *addrlen, cor_utime_t timeout);
int cor_connect(cor_netfd_t fd, const struct sockaddr *addr, int addrlen, cor_utime_t timeout);
ssize_t cor_read(cor_netfd_t fd, void *buf, size_t nbyte, cor_utime_t timeout);
ssize_t cor_read_fully(cor_netfd_t fd, void *buf, size_t nbyte, cor_utime_t timeout);
int cor_read_resid(cor_netfd_t fd, void *buf, size_t *resid, cor_utime_t timeout);
ssize_t cor_readv(cor_netfd_t fd, const struct iovec *iov, int iov_size, cor_utime_t timeout);
int cor_readv_resid(cor_netfd_t fd, struct iovec **iov, int *iov_size, cor_utime_t timeout);
ssize_t cor_write(cor_netfd_t fd, const void *buf, size_t nbyte, cor_utime_t timeout);
int cor_write_resid(cor_netfd_t fd, const void *buf, size_t *resid, cor_utime_t timeout);
ssize_t cor_writev(cor_netfd_t fd, const struct iovec *iov, int iov_size, cor_utime_t timeout);
int cor_writev_resid(cor_netfd_t fd, struct iovec **iov, int *iov_size, cor_utime_t timeout);


int cor_recvfrom(cor_netfd_t fd, void *buf, int len, struct sockaddr *from, int *fromlen, cor_utime_t timeout);
int cor_sendto(cor_netfd_t fd, const void *msg, int len,const struct sockaddr *to, int tolen, cor_utime_t timeout);
int cor__recvmsg(cor_netfd_t fd, struct msghdr *msg, int flags, cor_utime_t timeout);
int cor__sendmsg(cor_netfd_t fd, const struct msghdr *msg, int flags, cor_utime_t timeout);
// st_netfd_t st_open(const char *path, int oflags, mode_t mode);

// #ifdef DEBUG
// extern void _st_show_thread_stack(st_thread_t thread, const char *messg);
// extern void _st_iterate_threads(void);
// #endif

//=usr api=========================================================================

/**
 * create a new IPv4 TCP socket, bind, and listen
 * @param[in] ip socket bind local address, NULL-bind any address
 * @param[in] port bind local port
 * @param[in] backlog the maximum length to which the queue of pending connections for socket may grow
 * @return COR_NETFD_ERR, use socket_geterror() to get error code, other-ok 
*/
cor_netfd_t cor_tcp_listen_ipv4(const char* ip, uint16_t port, int backlog);

/** 
 * @param[in] timeout timeout in millisecond, COR_UTIME_NO_TIMEOUT for ever
 * @return COR_NETFD_ERR :err, other clifd. 
*/
cor_netfd_t cor_connect_host(const char* ip,  uint16_t port, cor_utime_t timeout_ms);

/**
 * 
 * @return null:err
*/
char* cor_getsockname(cor_netfd_t netfd, char* ip, uint16_t* port);

/**
 * INET6_ADDRSTRLEN
 * @return null:err
*/
char* cor_getpeername(cor_netfd_t netfd, char* ip, uint16_t* port);

/**
 * INET6_ADDRSTRLEN
 * @param[in] sa
 * @param[in] salen
 * @param[out] ip   char ip[INET6_ADDRSTRLEN];
 * @param[out] port uint16_t port;
 * @return null:err
*/
char* cor_addr_to(const struct sockaddr* sa, socklen_t salen, char* ip, uint16_t* port);

#ifdef __cplusplus
}
#endif

#endif