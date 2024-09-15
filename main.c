/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <stdio.h>

/* RP2040 */
#include <pico/types.h>

/* My standard library */
#include <my_can.h>

/* Driver */
#include <hardware_driver.h>

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <event_groups.h>
#include <queue.h>

int main( void )
{
    can_frame_t msg = {
        .id = CAN_ID_INVALID,
        .kind = E_CAN_FRAME_INVALID,
        .dlc = CAN_DLC_INVALID,
        .data = { 0U }
    };

    hwdrv_init_hardware();

    hwdrv_reset_can_controller();

    hwdrv_begin_can_communication();

    // vTaskStartScheduler();

    while ( true ) {
        hwdrv_get_can_msg( E_CAN_RX1, &msg );

        printf("CANID:%03Xh %02X %02X %02X %02X %02X %02X %02X %02X\n", msg.id,
            msg.data[0],
            msg.data[1],
            msg.data[2],
            msg.data[3],
            msg.data[4],
            msg.data[5],
            msg.data[6],
            msg.data[7]
        );
    }

    while ( true ) {
        // NOP
    }
}
