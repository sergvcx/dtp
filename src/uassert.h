#ifndef __U_ASSERT_H__
#define __U_ASSERT_H__
#include <cstdlib>
#include "stdio.h"

#ifdef NDEBUG
#  define uassert(condition) ((void)0)
#else
#  define uassert(expr) if(!(expr)) { printf("error: assertion \'%s\' failed:%s:%d\n", #expr, __FILE__, __LINE__); fflush(stdout); exit(1); }
#endif

#ifdef NDEBUG
#   define m_uassert(expr, ...) ((void)0)
#else
#   define m_uassert(expr, ...) if(!(expr)) { printf("error: assertion \'%s\' failed:%s:%d\t", #expr, __FILE__, __LINE__); \
                                    printf(__VA_ARGS__); \
                                    printf("\n"); \
                                    fflush(stdout);            \
                                    exit(1); }
#endif

#ifdef __NM__
#define CHECK_EXT_ADDR(addr)  m_uassert(    (int)(addr) > 0x40000 && (int)(addr) < 0xC0000 ||       \
			                                (int)(addr) > 0x20000000 && (int)(addr) < 0x40000000,   \
			                                "%s out of range: %p", #addr, (addr))                   
#define CHECK_INNER_ADDR(addr)  m_uassert(    (int)(addr) > 0x0 && (int)(addr) < 0xA0000, \
			                                "%s out of inner range: %p", #addr, (addr))    
#define CHECK_ADDR_PARITY(addr) m_uassert(    (int)(addr) % 2 == 0, "%s not parity: %p", #addr, (addr) )    
#else 
#define CHECK_EXT_ADDR(addr)
#define CHECK_INNER_ADDR(addr)
#define CHECK_ADDR_PARITY(addr)
#endif

#endif //__U_ASSERT_H__
