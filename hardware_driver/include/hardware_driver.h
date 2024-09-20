#ifndef HARDWARE_DRIVER_H
#define HARDWARE_DRIVER_H

/* RP2040 */
#include <pico/types.h>

/* My standard library */
#include <my_can.h>

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum en_can_irq_fact
{
    E_CAN_IRQ_FACT_MER = 0x80U,
    E_CAN_IRQ_FACT_WAK = 0x40U,
    E_CAN_IRQ_FACT_ERR = 0x20U,
    E_CAN_IRQ_FACT_TX3 = 0x10U,
    E_CAN_IRQ_FACT_TX2 = 0x08U,
    E_CAN_IRQ_FACT_TX1 = 0x04U,
    E_CAN_IRQ_FACT_RX2 = 0x02U,
    E_CAN_IRQ_FACT_RX1 = 0x01U
} can_irq_fact_t;

typedef void (*can_irq_callback_t)( const uint8_t fact );

typedef enum en_can_rx
{
    E_CAN_RX1,
    E_CAN_RX2
} can_rx_t;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
void hwd_init_hardware( void );
void hwd_light_led_1( const bool lit );
void hwd_reset_can_controller( void );
void hwd_begin_can_communication( void );
void hwd_set_can_irq_callback( const can_irq_callback_t callback );
void hwd_enable_can_irq( const bool enabled );
void hwd_get_can_msg( const can_rx_t can_rx, can_msg_t *p_can_msg );
void hwd_enable_can_irq_fact( const uint8_t fact );

#endif /* HARDWARE_DRIVER_H */
