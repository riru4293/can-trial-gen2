/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <stdio.h>

/* RP2040 */
#include <pico/types.h>

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <event_groups.h>
#include <queue.h>

/* My standard library */
#include <cdf_can.h>

/* Driver */
#include <hwd_api.h>

/* My module */
#include <ntm_task.h>

int main( void )
{
    hwd_init();

    ntm_create_task();

    vTaskStartScheduler();

    while ( true ) {
        /* Unreachable */
    }
}
