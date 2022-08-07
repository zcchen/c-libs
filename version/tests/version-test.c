#include "testing_helper.h"
#include "version.h"

#include <stdio.h>
#include <assert.h>

int test_version()
{
    printf("---- testing version: ------\n");
    printf("version with digital func: %d.%d.%d\n",
            version_major(), version_minor(), version_patch());
    printf("version with string func : %s\n", version_full());
    printf("----------------------------\n");
    return 0;
}

int main(void)
{
    TEST_RETURN(test_version());
    return 0;
}
