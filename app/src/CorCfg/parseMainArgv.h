
#ifndef __parseMainArgv_H
#define __parseMainArgv_H

int parseMainArgv(int argc, char const *argv[]);

bool correct_cfg_int(char ch, int& cfg);
bool correct_cfg_str(char ch, std::string& cfg);

#endif
