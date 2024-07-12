#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <string.h>
#include "main.h"
#include "httpd.h"

#ifdef __APPLE__
#include <libproc.h>
#endif

int main(int argc, char *argv[]) {
    
    httpd_main();

    return 0;
}
