#include "endian.h"

#define FATAL_UINT_RETURN 0     // for those endian cannot be detected.

/* Swap bytes in 16-bit value.  */
#define __bswap_constant_16(x)                                  \
  ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

/* Swap bytes in 32-bit value.  */
#define __bswap_constant_32(x)                                  \
  ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)     \
   | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))


uint16_t clibs_htobe16(uint16_t host_16bits)
{
#ifndef __ENDIAN_SETUP_AT_BUILD_TIME
    if (clibs_host_endian() == 'b') {
        return host_16bits;
    }
    else if (clibs_host_endian() == 'l') {
        return __bswap_constant_16(host_16bits);
    }
    else {
        return FATAL_UINT_RETURN;
    }
#else
#if (clibs_host_endian() == 'b')
    return host_16bits;
#elif (clibs_host_endian() == 'l')
    return __bswap_constant_16(host_16bits);
#else
    #error "I don't know how to convert this!"
#endif
#endif
}

uint16_t clibs_htole16(uint16_t host_16bits)
{
#ifndef __ENDIAN_SETUP_AT_BUILD_TIME
    if (clibs_host_endian() == 'l') {
        return host_16bits;
    }
    else if (clibs_host_endian() == 'b') {
        return __bswap_constant_16(host_16bits);
    }
    else {
        return FATAL_UINT_RETURN;
    }
#else
#if (clibs_host_endian() == 'l')
    return host_16bits;
#elif (clibs_host_endian() == 'b')
    return __bswap_constant_16(host_16bits);
#else
    #error "I don't know how to convert this!"
#endif
#endif
}

uint16_t clibs_be16toh(uint16_t big_endian_16bits)
{
#ifndef __ENDIAN_SETUP_AT_BUILD_TIME
    if (clibs_host_endian() == 'b') {
        return big_endian_16bits;
    }
    else if (clibs_host_endian() == 'l') {
        return __bswap_constant_16(big_endian_16bits);
    }
    else {
        return FATAL_UINT_RETURN;
    }
#else
#if (clibs_host_endian() == 'b')
    return big_endian_16bits;
#elif (clibs_host_endian() == 'l')
    return __bswap_constant_16(big_endian_16bits);
#else
    #error "I don't know how to convert this!"
#endif
#endif
}

uint16_t clibs_le16toh(uint16_t little_endian_16bits)
{
#ifndef __ENDIAN_SETUP_AT_BUILD_TIME
    if (clibs_host_endian() == 'l') {
        return little_endian_16bits;
    }
    else if (clibs_host_endian() == 'b') {
        return __bswap_constant_16(little_endian_16bits);
    }
    else {
        return FATAL_UINT_RETURN;
    }
#else
#if (clibs_host_endian() == 'l')
    return little_endian_16bits;
#elif (clibs_host_endian() == 'b')
    return __bswap_constant_16(little_endian_16bits);
#else
    #error "I don't know how to convert this!"
#endif
#endif
}

uint32_t clibs_htobe32(uint32_t host_32bits)
{
#ifndef __ENDIAN_SETUP_AT_BUILD_TIME
    if (clibs_host_endian() == 'b') {
        return host_32bits;
    }
    else if (clibs_host_endian() == 'l') {
        return __bswap_constant_32(host_32bits);
    }
    else {
        return FATAL_UINT_RETURN;
    }
#else
#if (clibs_host_endian() == 'b')
    return host_32bits;
#elif (clibs_host_endian() == 'l')
    return __bswap_constant_32(host_32bits);
#else
    #error "I don't know how to convert this!"
#endif
#endif
}

uint32_t clibs_htole32(uint32_t host_32bits)
{
#ifndef __ENDIAN_SETUP_AT_BUILD_TIME
    if (clibs_host_endian() == 'l') {
        return host_32bits;
    }
    else if (clibs_host_endian() == 'b') {
        return __bswap_constant_32(host_32bits);
    }
    else {
        return FATAL_UINT_RETURN;
    }
#else
#if (clibs_host_endian() == 'l')
    return host_32bits;
#elif (clibs_host_endian() == 'b')
    return __bswap_constant_32(host_32bits);
#else
    #error "I don't know how to convert this!"
#endif
#endif
}

uint32_t clibs_be32toh(uint32_t big_endian_32bits)
{
#ifndef __ENDIAN_SETUP_AT_BUILD_TIME
    if (clibs_host_endian() == 'b') {
        return big_endian_32bits;
    }
    else if (clibs_host_endian() == 'l') {
        return __bswap_constant_32(big_endian_32bits);
    }
    else {
        return FATAL_UINT_RETURN;
    }
#else
#if (clibs_host_endian() == 'b')
    return big_endian_32bits;
#elif (clibs_host_endian() == 'l')
    return __bswap_constant_32(big_endian_32bits);
#else
    #error "I don't know how to convert this!"
#endif
#endif
}

uint32_t clibs_le32toh(uint32_t little_endian_32bits)
{
#ifndef __ENDIAN_SETUP_AT_BUILD_TIME
    if (clibs_host_endian() == 'l') {
        return little_endian_32bits;
    }
    else if (clibs_host_endian() == 'b') {
        return __bswap_constant_32(little_endian_32bits);
    }
    else {
        return FATAL_UINT_RETURN;
    }
#else
#if (clibs_host_endian() == 'l')
    return little_endian_32bits;
#elif (clibs_host_endian() == 'b')
    return __bswap_constant_32(little_endian_32bits);
#else
    #error "I don't know how to convert this!"
#endif
#endif
}

