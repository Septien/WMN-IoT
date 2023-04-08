#include <string.h>

#include "helpers.h"
#include "printbinary.h"


void execute_rema(void)
{
    REMA_t rema, *prema;

    prema = &rema;
    rema_init(&prema);
    int end = 0;
    while (!end) {
        // Run the REMA protocol
        rema_dummy(prema);
        break;
    }

    rema_terminate(&prema);
}
