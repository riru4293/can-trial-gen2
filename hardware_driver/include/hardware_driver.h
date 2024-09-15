#ifndef HARDWARE_DRIVER_H
#define HARDWARE_DRIVER_H

/* RP2040 */
#include <pico/types.h>

/* My standard library */
#include <my_can.h>

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef void (*HWDRV_CAN_IRQ_CALLBACK_T)( void );

typedef enum
{
    hwdrv_can_rx1,
    hwdrv_can_rx2
} EN_HWDRV_CAN_RX;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
void hwdrv_init_hardware( void );
void hwdrv_light_led_1( const bool lit );
void hwdrv_reset_can_controller( void );
void hwdrv_begin_can_communication( void );
void hwdrv_set_can_irq_callback( const HWDRV_CAN_IRQ_CALLBACK_T const callback );
void hwdrv_enable_can_irq( const bool enabled );
void hwdrv_get_can_msg( const EN_HWDRV_CAN_RX can_rx, can_frame_t *p_msg );
#endif /* HARDWARE_DRIVER_H */
