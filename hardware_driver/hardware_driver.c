/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/stdlib.h>

/* Driver */
#include <hardware_driver.h>
#include <private/rp2040.h>
#include <private/mcp2515.h>

VOID hwdrv_init_hardware( VOID )
{
    /* Initialize standard I/O */
    stdio_init_all();

    /* Initialize LED */
    rp2040_init_led_1_hw();

    /* Initialize SPI */
    rp2040_init_spi_1_hw();
}

VOID hwdrv_light_led_1( const BOOL lit )
{
    rp2040_light_led_1( lit );
}

VOID hwdrv_reset_can_controller( VOID )
{
    mcp2515_reset();
}

VOID hwdrv_set_can_irq_callback( const HWDRV_CAN_IRQ_CALLBACK_T const callback )
{
    rp2040_set_can_irq_callback( callback );
}

VOID hwdrv_enable_can_irq( const BOOL enabled )
{
    rp2040_enable_can_irq( enabled );
}

VOID hwdrv_begin_can_communication( VOID )
{
    mcp2515_begin_communication();
}