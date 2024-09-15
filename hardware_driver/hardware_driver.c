/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/stdlib.h>

/* Driver */
#include <hardware_driver.h>
#include <private/rp2040.h>
#include <private/mcp2515.h>

void hwdrv_init_hardware( void )
{
    /* Initialize standard I/O */
    stdio_init_all();

    /* Initialize LED */
    rp2040_init_led_1_hw();

    /* Initialize SPI */
    rp2040_init_spi_1_hw();
}

void hwdrv_light_led_1( const bool lit )
{
    rp2040_light_led_1( lit );
}

void hwdrv_reset_can_controller( void )
{
    mcp2515_reset();
}

void hwdrv_set_can_irq_callback( const can_irq_callback_t callback )
{
    rp2040_set_can_irq_callback( callback );
}

void hwdrv_enable_can_irq( const bool enabled )
{
    rp2040_enable_can_irq( enabled );
}

void hwdrv_begin_can_communication( void )
{
    mcp2515_begin_communication();
}

void hwdrv_get_can_msg( const can_rx_t can_rx, can_frame_t *p_msg )
{
    mcp2515_get_can_msg( can_rx, p_msg );
}
