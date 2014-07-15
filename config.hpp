#pragma once
#include <string>
#include <vector>
#include <map>

extern std::map<std::string, std::vector<std::string>> inc2lib;
extern bool release;
extern std::string incDir;
extern std::string libDir;
extern std::string cfgDir;

void readConfig();
