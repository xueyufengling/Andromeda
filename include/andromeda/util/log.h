#ifndef ANDROMEDA_UTIL_DEBUG
#define ANDROMEDA_UTIL_DEBUG
//定义调试相关的宏，例如输出报错信息的方式

#define DEBUG true

#include <iostream>
#include "../util/string_utils.h"

#if DEBUG
#define LOG_DEBUG(...) {std::cout<<(andromeda::util::str_join(__VA_ARGS__))<<std::endl;}
#else
#define LOG_DEBUG(...) ;
#endif

#define LOG_INFO(...) {std::cout<<(andromeda::util::str_join(__VA_ARGS__))<<std::endl;}

#define LOG_ERROR(...) {std::cerr<<(andromeda::util::str_join(__VA_ARGS__))<<std::endl;}

#endif //ANDROMEDA_UTIL_DEBUG
