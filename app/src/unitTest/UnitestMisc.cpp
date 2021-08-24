

#include <string>

#include "xlog.h"
#include "Unitest.h"
#include "sys/system.h"

using namespace std;

int unitestMisc(int argc, char const *argv[]){
    size_t num = system_getcpucount();
    LOG_DBG_N("system_getcpucount = %d\n", num);

    return 0;
}
