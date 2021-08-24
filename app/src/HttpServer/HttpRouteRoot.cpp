

#include "HttpRoute.h"

extern HttpRe httpApp_index(http_session_t* session);
extern HttpRe httpApp_doc(http_session_t* session);
extern HttpRe httpApp_Console(http_session_t* session) ;

HttpRouteNode httpConsoleNode[]=
{
    {"apiv1_1_tmp",     nullptr,        nullptr,    nullptr},
    {"console",          httpApp_Console,     nullptr,    nullptr},
};

HttpRouteNode httpRootNode[]=
{
    {"apiv1_1_tmp",     nullptr,        nullptr,    nullptr},
    {"apiv1",           httpApp_index,     nullptr,    httpConsoleNode},
    {"/",           httpApp_index,     nullptr,    nullptr},
    {"doc",           httpApp_doc,     nullptr,    nullptr},
};

