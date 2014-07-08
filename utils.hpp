#pragma once
#include <string>
#include <ctime>

bool exists(std::string fileName);
time_t last_write_time(std::string fileName);
bool is_directory(std::string fileName);
std::string extension(std::string fileName);
void create_directory(std::string dir);
std::string current_path();
std::string filename(std::string fileName);
std::string replace_extension(std::string path, std::string ext);
