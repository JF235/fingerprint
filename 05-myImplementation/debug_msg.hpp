#ifndef DEBUG_MSG_HPP
#define DEBUG_MSG_HPP

#include <iostream>

#ifdef INSDEBUG
#define INSDEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#define INSDEBUG_INLINE(str) do { std::cout << str; } while( false )

#else
#define INSDEBUG_MSG(str) do { } while ( false )
#define INSDEBUG_INLINE(str) do { } while ( false )
#endif

#ifdef KNNDEBUG
#define KNNDEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#define KNNDEBUG_INLINE(str) do { std::cout << str; } while( false )
#else
#define KNNDEBUG_MSG(str) do { } while ( false )
#define KNNDEBUG_INLINE(str) do { } while ( false )
#endif

#endif // DEBUG_MSG_HPP