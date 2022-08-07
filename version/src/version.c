#include "version.h"
#include <stdio.h>

int version_major()
{
    return ZCCHEN_CLIBS__VERSION_MAJOR;
}

int version_minor()
{
    return ZCCHEN_CLIBS__VERSION_MINOR;
}

int version_patch()
{
    return ZCCHEN_CLIBS__VERSION_PATCH;
}

const char* version_full()
{
    static char ret[64];
    snprintf(ret, 64, "%d.%d.%d",
            ZCCHEN_CLIBS__VERSION_MAJOR,
            ZCCHEN_CLIBS__VERSION_MINOR,
            ZCCHEN_CLIBS__VERSION_PATCH);
    return ret;
}
