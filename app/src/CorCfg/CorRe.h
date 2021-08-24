
#ifndef _CORRE_H
#define _CORRE_H

#include <string>

enum CorReCode{
    CORRE_CODE_ERR  =   -1,
    CORRE_CODE_OK   =   0,
    CORRE_MAX,
};

struct CorRe{
    private:
        CorReCode code;
        std::string mdesc;
    public:
    CorRe(CorReCode _code, const char* _desc){
        code = _code;   mdesc = _desc;
    }
    CorRe(CorReCode _code, std::string _desc){
        code = _code;   mdesc = _desc;
    }
    CorRe(CorReCode s){
        code = s;
    }
    CorRe(){
        code = CORRE_CODE_OK;
    }
    std::string desc() const{
        return mdesc;
    }

    bool operator==(CorReCode re){
        if(this->code == re)    return true;
        else                    return false;
    }

    bool operator!=(CorReCode re){
        if(this->code != re)    return true;
        else                    return false;
    }

};

#define CORRE_IS_OK(re) ((re)==CORRE_CODE_OK)

#define CORRE_ERR(desc) CorRe(CORRE_CODE_ERR, desc)
#define CORRE_OK      CorRe(CORRE_CODE_OK)
#define CORRE_OK_DESC(desc)      CorRe(CORRE_CODE_OK, desc)

#endif
