
#ifndef _HTTPUTIL_H
#define _HTTPUTIL_H

#include <vector>
#include <string>

enum HttpReCode{
    HTTPCODE_OK             =   200,    //  200——交易成功
    HTTPCODE_INVAL_PARAM    =   400,    //  400——错误请求，如语法错误         
    HTTPCODE_NOT_SUPPORT    =   404,    //  404——没有发现文件、查询或URl
    HTTPCODE_ERR            =   500,    //  500——服务器产生内部错误
    HTTPCODE_TIMEOUT        =   503,    //  503——服务器过载或暂停维修
    HTTPCODE_MAX,
};

struct HttpReDesc{
    HttpReCode code;
    std::string desc;
};

const static std::vector<HttpReDesc> gHttpReDesc = {
    {HTTPCODE_OK,               "(200)ok"},
    {HTTPCODE_INVAL_PARAM,      "(400)inval param"},
    {HTTPCODE_NOT_SUPPORT,      "(404)not support"},
    {HTTPCODE_ERR,              "(500)err"},
    {HTTPCODE_TIMEOUT,          "(503)timeout"},
};

struct HttpRe{
    private:
        HttpReCode code;
        std::string _desc;

    public:
    HttpRe(HttpReCode s, const char* d){
        code = s;
        _desc = d;
    }
    HttpRe(HttpReCode s){
        code = s;
    }
    HttpRe(){
        code = HTTPCODE_OK;
    }
    std::string desc() const{
        return std::string(std::string(gHttpReDesc[code].desc) + ":" + _desc);
    }

    bool operator==(HttpReCode re){
        if(this->code == re)    return true;
        else                    return false;
    }

    bool operator!=(HttpReCode re){
        if(this->code != re)    return true;
        else                    return false;
    }

};


#define HTTPRE_OK           HttpRe(HTTPCODE_OK           )  //  200——交易成功
#define HTTPRE_INVAL_PARAM  HttpRe(HTTPCODE_INVAL_PARAM  )  //  400——错误请求，如语法错误 
#define HTTPRE_NOT_SUPPORT  HttpRe(HTTPCODE_NOT_SUPPORT  )   //  404——没有发现文件、查询或URl
#define HTTPRE_ERR          HttpRe(HTTPCODE_ERR          )  //  500——服务器产生内部错误
#define HTTPRE_TIMEOUT      HttpRe(HTTPCODE_TIMEOUT     )   //  503——服务器过载或暂停维修 

#define sHTTPRE_OK(s)           HttpRe(HTTPCODE_OK          , s.c_str())   //  200——交易成功
#define sHTTPRE_INVAL_PARAM(s)  HttpRe(HTTPCODE_INVAL_PARAM , s.c_str())   //  400——错误请求，如语法错误 
#define sHTTPRE_NOT_SUPPORT(s)  HttpRe(HTTPCODE_NOT_SUPPORT , s.c_str())   //  404——没有发现文件、查询或URl
#define sHTTPRE_ERR(s)          HttpRe(HTTPCODE_ERR         , s.c_str())   //  500——服务器产生内部错误
#define sHTTPRE_TIMEOUT(s)      HttpRe(HTTPCODE_TIMEOUT     , s.c_str())   //  503——服务器过载或暂停维修 

#define HttpReIsOk(h)           (h == HTTPCODE_OK)
#define HttpReIsNotSupport(h)   (h == HTTPCODE_NOT_SUPPORT)
#define HttpReIsErr(h)          (h == HTTPCODE_ERR)

#endif



/*

Http状态码一览表

    所谓的404页就是服务器404重定向状态返回页面。数字404指的是404号状态码。

一般常用到的有200号状态码和404号状态码。200号表示网页被下载成功，而404号表示不能成功下载并产生错误。下面是HTTP状态码一览表。

1xx：请求收到，继续处理
2xx：操作成功收到，分析、接受
3xx：完成此请求必须进一步处理
4xx：请求包含一个错误语法或不能完成
5xx：服务器执行一个完全有效请求失败

 

1xx：请求收到，继续处理

100——客户必须继续发出请求
101——客户要求服务器根据请求转换HTTP协议版本

 

2xx：操作成功收到，分析、接受

200——交易成功
201——提示知道新文件的URL
202——接受和处理、但处理未完成
203——返回信息不确定或不完整
204——请求收到，但返回信息为空
205——服务器完成了请求，用户代理必须复位当前已经浏览过的文件
206——服务器已经完成了部分用户的GET请求

 

3xx：完成此请求必须进一步处理

300——请求的资源可在多处得到
301——删除请求数据
302——在其他地址发现了请求数据
303——建议客户访问其他URL或访问方式
304——客户端已经执行了GET，但文件未变化
305——请求的资源必须从服务器指定的地址得到
306——前一版本HTTP中使用的代码，现行版本中不再使用
307——申明请求的资源临时性删除

 

4xx：请求包含一个错误语法或不能完成

400——错误请求，如语法错误
401——请求授权失败
402——保留有效ChargeTo头响应
403——请求不允许
404——没有发现文件、查询或URl
405——用户在Request-Line字段定义的方法不允许
406——根据用户发送的Accept拖，请求资源不可访问
407——类似401，用户必须首先在代理服务器上得到授权
408——客户端没有在用户指定的饿时间内完成请求
409——对当前资源状态，请求不能完成
410——服务器上不再有此资源且无进一步的参考地址
411——服务器拒绝用户定义的Content-Length属性请求
412——一个或多个请求头字段在当前请求中错误
413——请求的资源大于服务器允许的大小
414——请求的资源URL长于服务器允许的长度
415——请求资源不支持请求项目格式
416——请求中包含Range请求头字段，在当前请求资源范围内没有range指示值，请求
也不包含If-Range请求头字段
417——服务器不满足请求Expect头字段指定的期望值，如果是代理服务器，可能是下
一级服务器不能满足请求

 

5xx：服务器执行一个完全有效请求失败

500——服务器产生内部错误
501——服务器不支持请求的函数
502——服务器暂时不可用，有时是为了防止发生系统过载
503——服务器过载或暂停维修
504——关口过载，服务器使用另一个关口或服务来响应用户，等待时间设定值较长
505——服务器不支持或拒绝支请求头中指定的HTTP版本

 

 

=======================================================

一些常见的状态码为：

　　200 - 服务器成功返回网页

　　404 - 请求的网页不存在

　　503 - 服务器超时

*/