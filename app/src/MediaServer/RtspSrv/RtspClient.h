
#ifndef _RTSPCLIENT_H
#define _RTSPCLIENT_H

#include "AppCfg.h"

class RtspClient
{
private:
    AppCfg::RtspCliCfg cliCfg;
public:
    RtspClient();
    ~RtspClient();
     void setCfg(AppCfg::RtspCliCfg& cfg){cliCfg = cfg;}
};






#endif
