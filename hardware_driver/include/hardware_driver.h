#ifndef HARDWARE_DRIVER_H
#define HARDWARE_DRIVER_H

/* RP2040 */
#include <pico/types.h>

/* My standard library */
#include <my_can.h>

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef void (*can_irq_callback_t)( void );

typedef enum en_can_rx
{
    E_CAN_RX1,
    E_CAN_RX2
} can_rx_t;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
void hwdrv_init_hardware( void );
void hwdrv_light_led_1( const bool lit );
void hwdrv_reset_can_controller( void );
void hwdrv_begin_can_communication( void );
void hwdrv_set_can_irq_callback( const can_irq_callback_t callback );
void hwdrv_enable_can_irq( const bool enabled );
void hwdrv_get_can_msg( const can_rx_t can_rx, can_frame_t *p_msg );
#endif /* HARDWARE_DRIVER_H */
