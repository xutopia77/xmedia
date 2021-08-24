
#ifndef _UTIL_H
#define _UTIL_H

#include <vector>
#include <string>

void stringsplit(const std::string& str, const std::string& splits, std::vector<std::string>& res);
bool stringRemove(std::string& message, const std::string& str);
bool stringSplitFrom(const std::string& strin,std::string str , std::string& out1,std::string& out2);

#endif
