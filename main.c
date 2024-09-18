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


void test_irq( const uint8_t fact );

void test_irq( const uint8_t fact )
{
    
}

int main( void )
{
    can_frame_t can_frame = {
        .id = CAN_ID_INVALID,
        .kind = E_CAN_FRAME_INVALID,
        .dlc = CAN_DLC_INVALID,
        .is_data = false,
        .data = { 0U }
    };

    hwd_init_hardware();

    hwd_reset_can_controller();

    hwd_set_can_irq_callback( test_irq );

    hwd_enable_can_irq( true );

    hwd_begin_can_communication();

    cam_create_task();

    vTaskStartScheduler();

    while ( true ) {
        hwd_get_can_frame( E_CAN_RX1, &can_frame );

        printf("CANID:%03Xh %02X %02X %02X %02X %02X %02X %02X %02X\n", can_frame.id,
            can_frame.data[E_CAN_DATA_0],
            can_frame.data[E_CAN_DATA_1],
            can_frame.data[E_CAN_DATA_2],
            can_frame.data[E_CAN_DATA_3],
            can_frame.data[E_CAN_DATA_4],
            can_frame.data[E_CAN_DATA_5],
            can_frame.data[E_CAN_DATA_6],
            can_frame.data[E_CAN_DATA_7]
        );
    }

    while ( true ) {
        // NOP
    }
}
