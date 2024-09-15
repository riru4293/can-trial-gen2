#ifndef HARDWARE_DRIVER_H
#define HARDWARE_DRIVER_H

/* My standard library */
#include <my_types.h>

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef VOID (*HWDRV_CAN_IRQ_CALLBACK_T)( VOID );

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
VOID hwdrv_init_hardware( VOID );
VOID hwdrv_light_led_1( const BOOL lit );
ERR_CD_T hwdrv_reset_can_controller( VOID );
VOID hwdrv_set_can_irq_callback( const HWDRV_CAN_IRQ_CALLBACK_T const callback );
VOID hwdrv_enable_can_irq( const BOOL enabled );

#endif /* HARDWARE_DRIVER_H */
