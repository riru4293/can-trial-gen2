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
#include <my_can.h>

/* Driver */
#include <hardware_driver.h>

/* My module */
#include <can_manager.h>

int main( void )
{
    hwd_init_hardware();

    cam_create_task();

    vTaskStartScheduler();

    while ( true ) {
        /* Unreachable */
    }
}
