/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* My standard library */
#include <my_types.h>

/* Driver */
#include <hardware_driver.h>

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <event_groups.h>
#include <queue.h>

INT main( VOID )
{
    hwdrv_init_hardware();

    hwdrv_reset_can_controller();

    vTaskStartScheduler();

    while ( TRUE ) {
        // NOP
    }
}
