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
void rp2040_init_led_1_hw( void );
void rp2040_init_spi_1_hw( void );
void rp2040_light_led_1( const bool lit );
void rp2040_set_can_irq_callback( const can_irq_callback_t const callback );
void rp2040_enable_can_irq( const bool enabled );
void rp2040_begin_spi_1( void );
void rp2040_end_spi_1( void );
void rp2040_read_spi_1_array( const size_t len, uint8_t *p_buff );
void rp2040_write_spi_1_array( const size_t len, const uint8_t const *p_buff );
uint8_t rp2040_read_spi_1( void );
void rp2040_write_spi_1( const uint8_t val );

#endif /* RP2040_H */
