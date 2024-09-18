/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/stdlib.h>
#include <hardware/spi.h>

/* Driver */
#include <private/rp2040.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
/* GPIO status */
#define GPIO_LEVEL_LOW      ( false )
#define GPIO_LEVEL_HIGH     ( true )

/* GPIO pin number */
#define GPIO_01             ( (uint) 1U )
#define GPIO_02             ( (uint) 2U )
#define GPIO_03             ( (uint) 3U )
#define GPIO_04             ( (uint) 4U )
#define GPIO_05             ( (uint) 5U )
#define GPIO_06             ( (uint) 6U )
#define GPIO_07             ( (uint) 7U )
#define GPIO_08             ( (uint) 8U )
#define GPIO_09             ( (uint) 9U )
#define GPIO_10             ( (uint)10U )
#define GPIO_11             ( (uint)11U )
#define GPIO_12             ( (uint)12U )
#define GPIO_13             ( (uint)13U )
#define GPIO_14             ( (uint)14U )
#define GPIO_15             ( (uint)15U )
#define GPIO_16             ( (uint)16U )
#define GPIO_17             ( (uint)17U )
#define GPIO_18             ( (uint)18U )
#define GPIO_19             ( (uint)19U )
#define GPIO_20             ( (uint)20U )
#define GPIO_21             ( (uint)21U )
#define GPIO_22             ( (uint)22U )
#define GPIO_23             ( (uint)23U )
#define GPIO_24             ( (uint)24U )
#define GPIO_25             ( (uint)25U )
#define GPIO_26             ( (uint)26U )
#define GPIO_27             ( (uint)27U )
#define GPIO_28             ( (uint)28U )

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
static void irq_callback( const uint gpio, const uint32_t events );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static can_irq_callback_t g_can_irq_callback = NULL;

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

void rp2040_light_led_1( const bool lit )
{
    const bool lv = ( true == lit ) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;

    gpio_put( LED_1_PORT, lv );
}

void rp2040_set_can_irq_callback( const can_irq_callback_t const callback )
{
    g_can_irq_callback = callback;
}

void rp2040_enable_can_irq( const bool enabled )
{
    gpio_set_irq_enabled_with_callback(
        CAN_CTRL_PORT_IRQ, GPIO_IRQ_LEVEL_LOW, enabled, irq_callback );
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

void rp2040_write_spi_1_array( const size_t len, const uint8_t const *p_buff )
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
static void irq_callback( const uint gpio, const uint32_t events )
{
    switch( gpio )
    {
    case CAN_CTRL_PORT_IRQ:
        if( NULL != g_can_irq_callback )
        {
            /* Disable CAN IRQ                           */
            /* Because prevent continuous occurs CAN IRQ */
            /* Must re-enable interrupts in the callback */
            rp2040_enable_can_irq( false );

            g_can_irq_callback();
        }
        break;
    default:
        // Do nothing
        break;
    }
}
