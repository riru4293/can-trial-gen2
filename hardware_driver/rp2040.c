/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/stdlib.h>
#include <hardware/spi.h>

/* Driver */
#include <private/rp2040.h>
#include <private/rp2040_gpio.h>
#include <private/can_irq.h>

/* SPI hardware */
#define SPI_1_HW            ( spi0 )
#define SPI_2_HW            ( spi1 )

/* SPI 1 port */
#define SPI_1_PORT_MISO     ( GPIO_04 )
#define SPI_1_PORT_CS       ( GPIO_05 )
#define SPI_1_PORT_SCLK     ( GPIO_06 )
#define SPI_1_PORT_MOSI     ( GPIO_07 )

/* Maximum configurable SPI clock: 8.928571MHz.                                     */
/* The maximum SPI clock for the MCP2515 is 10MHz, but this is not configurable.    */
/* Because the SPI clock cannot be set to any value other than the division value   */
/* of the RP2040 frequency that 125 MHz.                                            */
#define SPI_1_BAUDRATE      ( (uint)8928571U )

/* The value that will be set in the SPI send buffer when reading the SPI receive buffer */
#define SPI_1_REPEATED_TX_DATA  ( (uint8_t)0U )

/* CAN contorller port */
#define CAN_CTRL_PORT_IRQ   ( GPIO_21 )

/* Internal LED port */
#define LED_1_PORT          ( GPIO_25 )

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void irq_cbk( const uint gpio, const uint32_t events );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
void rp2040_init_led_1_hw( void )
{
    gpio_init( LED_1_PORT );
    gpio_set_dir( LED_1_PORT, GPIO_OUT );
}

void rp2040_init_spi_1_hw( void )
{
    /* Configute SPI baudrate*/
    (void)spi_init( SPI_1_HW, SPI_1_BAUDRATE );

    /* Initialize MISO, MOSI and SCLK port */
    gpio_set_function( SPI_1_PORT_MISO, GPIO_FUNC_SPI );
    gpio_set_function( SPI_1_PORT_MOSI, GPIO_FUNC_SPI );
    gpio_set_function( SPI_1_PORT_SCLK, GPIO_FUNC_SPI );

    /* Initialize CS port */
    (void)gpio_init( SPI_1_PORT_CS );
    gpio_set_dir( SPI_1_PORT_CS, GPIO_OUT );
}

void rp2040_light_led_1( const bool on )
{
    const bool lv = ( true == on ) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;

    gpio_put( LED_1_PORT, lv );
}

void rp2040_enable_irq_handling( const bool enabled )
{
    gpio_set_irq_enabled_with_callback(
        CAN_CTRL_PORT_IRQ, GPIO_IRQ_LEVEL_LOW, enabled, irq_cbk );
}

void rp2040_begin_spi_1( void )
{
    gpio_put( SPI_1_PORT_CS, GPIO_LEVEL_LOW );
}

void rp2040_end_spi_1( void )
{
    gpio_put( SPI_1_PORT_CS, GPIO_LEVEL_HIGH );
}

void rp2040_read_spi_1_array( const size_t len, uint8_t *p_buff )
{
    if( ( 0U < len ) && ( NULL != p_buff ) )
    {
        (void)spi_read_blocking( SPI_1_HW, SPI_1_REPEATED_TX_DATA, p_buff, len );
    }
}

void rp2040_write_spi_1_array( const size_t len, const uint8_t *p_buff )
{
    if( ( 0U < len ) && ( NULL != p_buff ) )
    {
        (void)spi_write_blocking( SPI_1_HW, p_buff, len );
    }
}

uint8_t rp2040_read_spi_1( void )
{
    uint8_t val = 0U;

    rp2040_read_spi_1_array( sizeof( uint8_t ), &val );

    return val;
}

void rp2040_write_spi_1( const uint8_t val )
{
    rp2040_write_spi_1_array( sizeof( uint8_t ), &val );
}

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
static void irq_cbk( const uint gpio, const uint32_t events )
{
    switch( gpio )
    {
    case CAN_CTRL_PORT_IRQ:
        mcp2515_can_irq_cbk();
        break;
    default:
        // Do nothing
        break;
    }
}
