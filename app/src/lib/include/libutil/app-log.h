#ifndef _app_log_h_
#define _app_log_h_

// from sys/syslog.h
#define BCK_LOG_EMERG       0       /* system is unusable */
#define BCK_LOG_ALERT       1       /* action must be taken immediately */
#define BCK_LOG_CRIT        2       /* critical conditions */
#define BCK_LOG_ERROR       3       /* error conditions */
#define BCK_LOG_WARNING     4       /* warning conditions */
#define BCK_LOG_NOTICE      5       /* normal but significant condition */
#define BCK_LOG_INFO        6       /* informational */
#define BCK_LOG_DEBUG       7       /* debug-level messages */

#ifdef __cplusplus
extern "C" {
#endif

#define BCK_APP_LOG_WITH_LINE(level, fmt, ...)	BCK_app_log(level, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define BCK_APP_LOG_ERROR(fmt, ...)				BCK_APP_LOG_WITH_LINE(LOG_ERROR, fmt, ##__VA_ARGS__)
#define BCK_APP_LOG_WARNING(fmt, ...)			BCK_APP_LOG_WITH_LINE(LOG_WARNING, fmt, ##__VA_ARGS__)
#define BCK_APP_LOG_NOTICE(fmt, ...)			BCK_APP_LOG_WITH_LINE(LOG_NOTICE, fmt, ##__VA_ARGS__)
#define BCK_APP_LOG_INFO(fmt, ...)				BCK_APP_LOG_WITH_LINE(LOG_INFO, fmt, ##__VA_ARGS__)
#define BCK_APP_LOG_DEBUG(fmt, ...)				BCK_APP_LOG_WITH_LINE(LOG_DEBUG, fmt, ##__VA_ARGS__)


void BCK_app_log(int level, const char* format, ...);

void BCK_app_log_setlevel(int level);

#ifdef __cplusplus
}
#endif
#endif /* !_app_log_h_ */
