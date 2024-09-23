#ifndef HARDWARE_DRIVER_H
#define HARDWARE_DRIVER_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* My standard library */
#include <my_can.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum
{
    E_CAN_RX_1,
    E_CAN_RX_2
} en_can_rx;

typedef enum
{
    E_CAN_TX_1,
    E_CAN_TX_2,
    E_CAN_TX_3
} en_can_tx;

typedef enum
{
    /* E_CAN_IRQ_FACT_MER = 0x80U, */
    /* E_CAN_IRQ_FACT_WAK = 0x40U, */
    /* E_CAN_IRQ_FACT_ERR = 0x20U, */
    E_CAN_IRQ_FACT_TX3 = 0x10U,
    E_CAN_IRQ_FACT_TX2 = 0x08U,
    E_CAN_IRQ_FACT_TX1 = 0x04U,
    E_CAN_IRQ_FACT_RX2 = 0x02U,
    E_CAN_IRQ_FACT_RX1 = 0x01U,
    E_CAN_IRQ_FACT_ALL = (uint8_t)( E_CAN_IRQ_FACT_RX1 | E_CAN_IRQ_FACT_RX2 |
        E_CAN_IRQ_FACT_TX1 | E_CAN_IRQ_FACT_TX2 | E_CAN_IRQ_FACT_TX3 )
} en_can_irq_fact;

typedef void (*fn_can_irq_cbk)( const uint8_t fact );

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
void hwd_init_hardware( void );
void hwd_reset_can_ctrl( void );

void hwd_turn_on_led_1( const bool is_on );

void hwd_set_can_irq_cbk( const fn_can_irq_cbk cbk );
void hwd_enable_can_irq_fact( const uint8_t fact );
void hwd_enable_irq_handling( const bool enabled );

void hwd_start_can_comm( void );
void hwd_get_can_msg( const en_can_rx can_rx, st_can_msg *p_can_msg );
void hwd_set_can_msg( const en_can_tx can_tx, st_can_msg *p_can_msg );

#endif /* HARDWARE_DRIVER_H */
