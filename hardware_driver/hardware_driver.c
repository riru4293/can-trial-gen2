/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/stdlib.h>

/* Driver */
#include <hwd_api.h>
#include <private/rp2040.h>
#include <private/mcp2515.h>

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
void hwd_init( void )
{
    /* Initialize standard I/O */
    stdio_init_all();

    /* Initialize LED */
    rp2040_init_led_1_hw();

    /* Initialize SPI */
    rp2040_init_spi_1_hw();
}

void hwd_turn_on_led_1( const bool is_on )
{
    rp2040_turn_on_led_1( is_on );
}

void hwd_reset_can_ctrl( void )
{
    mcp2515_reset();
}

void hwd_set_can_irq_cbk( const fn_hwd_can_irq_cbk cbk )
{
    mcp2515_set_can_irq_cbk( cbk );
}

void hwd_enable_irq_handling( const bool enabled )
{
    rp2040_enable_irq_handling( enabled );
}

void hwd_start_can_comm( void )
{
    mcp2515_start_can_comm();
}

void hwd_get_can_msg( const en_hwd_can_rx can_rx, st_cdf_can_msg *p_can_msg )
{
    mcp2515_get_can_msg( can_rx, p_can_msg );
}

void hwd_enable_can_irq_fact( const uint8_t fact )
{
    mcp2515_enable_can_irq_fact( fact );
}