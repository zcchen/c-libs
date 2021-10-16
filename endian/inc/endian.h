#ifndef ZCCHEN_CLIBS__ENDIAN_H
#define ZCCHEN_CLIBS__ENDIAN_H

#ifdef __cplusplus
extern "C" {
#endif

// If selecting endian detection at build time,
// it will use compiler's global definations to check the host endian.
#define ENDIAN_SETUP_AT_BUILD_TIME 0

// Uncomment the below macro, if you know your platform endian and the compiler macro incorrect
//#define BIG_ENDIAN
//#define LITTLE_ENDIAN

// Usually, little endian platforms are below:
//      X86, AMD64, some ARM platform, ...
// Big endian platforms are below:
//      network protocol, PowerPC, C51, ...

#include <stdint.h>

uint16_t clibs_htobe16(uint16_t host_16bits);
uint16_t clibs_htole16(uint16_t host_16bits);
uint16_t clibs_be16toh(uint16_t big_endian_16bits);
uint16_t clibs_le16toh(uint16_t little_endian_16bits);

uint32_t clibs_htobe32(uint32_t host_32bits);
uint32_t clibs_htole32(uint32_t host_32bits);
uint32_t clibs_be32toh(uint32_t big_endian_32bits);
uint32_t clibs_le32toh(uint32_t little_endian_32bits);

uint64_t clibs_htobe64(uint64_t host_64bits);
uint64_t clibs_htole64(uint64_t host_64bits);
uint64_t clibs_be64toh(uint64_t big_endian_64bits);
uint64_t clibs_le64toh(uint64_t little_endian_64bits);

// network protocol is big endian
#define clibs_htons(hostshort) clibs_htobe16(hostshort)
#define clibs_htonl(hostlong) clibs_htobe32(hostlong)
#define clibs_htonll(hostlonglong) clibs_htobe64(hostlonglong)
#define clibs_ntohs(netshort) clibs_be16toh(netshort)
#define clibs_ntohl(netlong) clibs_be32toh(netlong)
#define clibs_ntohll(netlonglong) clibs_be64toh(netlonglong)

// clibs_host_endian() will return 'l' if platform is little-endian
// clibs_host_endian() will return 'b' if platform is big-endian
#define clibs_host_endian() __get_host_endian()


#if defined(ENDIAN_SETUP_AT_BUILD_TIME) && (ENDIAN_SETUP_AT_BUILD_TIME != 0)
// Setup the endian during build time
#define __ENDIAN_SETUP_AT_BUILD_TIME    // The macro for the C libs
#if defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)
    #error "CANNOT set BIG_ENDIAN & LITTLE_ENDIAN at the same time."
#elif !defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN) // auto detection by compiler's macro
    //  ref: https://stackoverflow.com/questions/4239993/determining-endianness-at-compile-time
    #if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __BIG_ENDIAN || \
                                    __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || \
            defined(__BIG_ENDIAN__) || \
            defined(__ARMEB__) || \
            defined(__THUMBEB__) || \
            defined(__AARCH64EB__) || \
            defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
        #define BIG_ENDIAN
    #elif (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __LITTLE_ENDIAN || \
                                      __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
            defined(__LITTLE_ENDIAN__) || \
            defined(__ARMEL__) || \
            defined(__THUMBEL__) || \
            defined(__AARCH64EL__) || \
            defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
        #define LITTLE_ENDIAN
    #else
        #error "I don't know what architecture this is!"
    #endif
#endif /* if defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN) */
#ifdef BIG_ENDIAN
#define __get_host_endian() ('b')
#endif
#ifdef LITTLE_ENDIAN
#define __get_host_endian() ('l')
#endif

#else
// In this union: least addr first, then, biggest addr at the end.
// When getting the ul type, it will return its system-endian unsigned long data.
// At little-endian platform, the first char of the `ul` data is `c[0]`;
// At big-endian platform, the first char of the `ul` data is `c[3]`.
// Base on that method, it is easy to detect the endian of system.
union endian_checker_t {
    char c[4];
    unsigned long ul;
};
extern const union endian_checker_t endian_checker;
#define __get_host_endian() ((char)(endian_checker.ul))

#endif /* if defined(ENDIAN_SETUP_AT_BUILD_TIME) && (ENDIAN_SETUP_AT_BUILD_TIME != 0) */

#ifdef __cplusplus
extern }
#endif


#endif /* ifndef ZCCHEN_CLIBS__ENDIAN_H */
