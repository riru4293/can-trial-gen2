/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <stdio.h>

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
    ST_CAN_MSG msg = { 0U, { 0U } };

    hwdrv_init_hardware();

    hwdrv_reset_can_controller();

    hwdrv_begin_can_communication();

    // vTaskStartScheduler();

    while ( TRUE ) {
        hwdrv_get_can_msg( hwdrv_can_rx1, &msg );

        printf("CANID:%03Xh %02X %02X %02X %02X %02X %02X %02X %02X\n", msg.can_id,
            msg.can_msg[0],
            msg.can_msg[1],
            msg.can_msg[2],
            msg.can_msg[3],
            msg.can_msg[4],
            msg.can_msg[5],
            msg.can_msg[6],
            msg.can_msg[7]
        );
    }

    while ( TRUE ) {
        // NOP
    }
}
