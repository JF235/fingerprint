#ifndef DEBUG_MSG_HPP
#define DEBUG_MSG_HPP

#include <iostream>

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

#endif // DEBUG_MSG_HPP