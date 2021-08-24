
#include "HttpRoute.h"
#include <string>
#include <fstream>

using namespace std;

extern const string htmlRootPath = "../app/html";
// extern "C" int http_server_reply(http_session_t* session, int code, const void* data, size_t bytes);

HttpRe httpApp_index(http_session_t* session) {

	std::string fullpath = std::string("../app/html") + "/";

    char buf[256] = {0};
	// LOG_INFO("http_server_onroot: %s\n", fullpath.c_str());
    getcwd(buf, sizeof(buf));
    string indexHtmlPath = string(buf) + "/" + htmlRootPath + "/doc/index.html";

    ifstream srcFile(indexHtmlPath, ios::in);
    if(srcFile.is_open()){
        const std::string reply((std::istreambuf_iterator<char>(srcFile)),
                           (std::istreambuf_iterator<char>()));

        http_server_set_header(session, "content-type", "text/html");
        http_server_set_header(session, "Access-Control-Allow-Origin", "*");
        http_server_set_header(session, "Access-Control-Allow-Methods", "GET, POST, PUT");
        http_server_reply(session, 200, reply.c_str(), reply.length());

        return HTTPRE_OK;
    }

    LOG_WARN("open %s err,404\n", indexHtmlPath.c_str());
	http_server_set_status_code(session, 404, NULL);
	if(0 == http_server_send(session, "", 0, NULL, NULL)){
        return HTTPRE_OK;
    }
    else{
        return HTTPRE_ERR;
    }
}

HttpRe httpApp_doc(http_session_t* session) {

	std::string fullpath = std::string("../app/html") + "/";

    char buf[256] = {0};
	// LOG_INFO("http_server_onroot: %s\n", fullpath.c_str());
    getcwd(buf, sizeof(buf));
    string indexHtmlPath = string(buf) + "/" + htmlRootPath + http_session_get_request_uri(session);

    ifstream srcFile(indexHtmlPath, ios::in);
    if(srcFile.is_open()){
        const std::string reply((std::istreambuf_iterator<char>(srcFile)),
                           (std::istreambuf_iterator<char>()));

        http_server_set_header(session, "content-type", "text/html");
        http_server_set_header(session, "Access-Control-Allow-Origin", "*");
        http_server_set_header(session, "Access-Control-Allow-Methods", "GET, POST, PUT");
        http_server_reply(session, 200, reply.c_str(), reply.length());

        return HTTPRE_OK;
    }

    LOG_WARN("open %s err,404\n", indexHtmlPath.c_str());
	http_server_set_status_code(session, 404, NULL);
	if(0 == http_server_send(session, "", 0, NULL, NULL)){
        return HTTPRE_OK;
    }
    else{
        return HTTPRE_ERR;
    }

}
