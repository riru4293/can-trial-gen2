#ifndef RP2040_H
#define RP2040_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* Driver */
#include <hardware_driver.h>

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
VOID rp2040_init_led_1_hw( VOID );
VOID rp2040_init_spi_1_hw( VOID );
VOID rp2040_light_led_1( const BOOL lit );
VOID rp2040_set_can_irq_callback( const HWDRV_CAN_IRQ_CALLBACK_T const callback );
VOID rp2040_enable_can_irq( const BOOL enabled );
VOID rp2040_begin_spi_1( VOID );
VOID rp2040_end_spi_1( VOID );
VOID rp2040_read_spi_1_array( const SIZE_T len, UINT8 *p_buff );
VOID rp2040_write_spi_1_array( const SIZE_T len, const UINT8 const *p_buff );
UINT8 rp2040_read_spi_1( VOID );
VOID rp2040_write_spi_1( const UINT8 val );

#endif /* RP2040_H */
