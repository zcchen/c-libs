#include "testing_helper.h"
#include "endian.h"

#include <stdio.h>

int test__print_endian()
{
    printf("System Endian is: %c.\n", clibs_host_endian());
    return 0;
}

int main(void)
{
    TEST_RETURN(test__print_endian());
    return 0;
}
