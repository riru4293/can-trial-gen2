/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/stdlib.h>

/* Driver */
#include <hardware_driver.h>
#include <private/rp2040.h>
#include <private/mcp2515.h>

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
void hwd_init_hardware( void )
{
    /* Initialize standard I/O */
    stdio_init_all();

    /* Initialize LED */
    rp2040_init_led_1_hw();

    /* Initialize SPI */
    rp2040_init_spi_1_hw();
}

void hwd_light_led_1( const bool lit )
{
    rp2040_light_led_1( lit );
}

void hwd_reset_can_controller( void )
{
    mcp2515_reset();
}

void hwd_set_can_irq_callback( const can_irq_callback_t callback )
{
    mcp2515_set_can_irq_callback( callback );
}

void hwd_enable_can_irq( const bool enabled )
{
    rp2040_enable_can_irq( enabled );
}

void hwd_begin_can_communication( void )
{
    mcp2515_begin_communication();
}

void hwd_get_can_msg( const can_rx_t rx, can_msg_t *p_can_msg )
{
    mcp2515_get_can_msg( rx, p_can_msg );
}

void hwd_enable_can_irq_fact( const uint8_t fact )
{
    mcp2515_enable_can_irq_fact( fact );
}