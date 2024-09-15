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
#define GPIO_LEVEL_LOW      ( FALSE )
#define GPIO_LEVEL_HIGH     ( TRUE )

/* GPIO pin number */
#define GPIO_01             ( (UINT) 1U )
#define GPIO_02             ( (UINT) 2U )
#define GPIO_03             ( (UINT) 3U )
#define GPIO_04             ( (UINT) 4U )
#define GPIO_05             ( (UINT) 5U )
#define GPIO_06             ( (UINT) 6U )
#define GPIO_07             ( (UINT) 7U )
#define GPIO_08             ( (UINT) 8U )
#define GPIO_09             ( (UINT) 9U )
#define GPIO_10             ( (UINT)10U )
#define GPIO_11             ( (UINT)11U )
#define GPIO_12             ( (UINT)12U )
#define GPIO_13             ( (UINT)13U )
#define GPIO_14             ( (UINT)14U )
#define GPIO_15             ( (UINT)15U )
#define GPIO_16             ( (UINT)16U )
#define GPIO_17             ( (UINT)17U )
#define GPIO_18             ( (UINT)18U )
#define GPIO_19             ( (UINT)19U )
#define GPIO_20             ( (UINT)20U )
#define GPIO_21             ( (UINT)21U )
#define GPIO_22             ( (UINT)22U )
#define GPIO_23             ( (UINT)23U )
#define GPIO_24             ( (UINT)24U )
#define GPIO_25             ( (UINT)25U )
#define GPIO_26             ( (UINT)26U )
#define GPIO_27             ( (UINT)27U )
#define GPIO_28             ( (UINT)28U )

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
#define SPI_1_BAUDRATE      ( (UINT)8928571U )

/* The value that will be set in the SPI send buffer when reading the SPI receive buffer */
#define SPI_1_REPEATED_TX_DATA  ( (UINT8)0U )

/* CAN contorller port */
#define CAN_CTRL_PORT_IRQ   ( GPIO_21 )

/* Internal LED port */
#define LED_1_PORT          ( GPIO_25 )

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static VOID irq_callback( const UINT gpio, const UINT32 events );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static HWDRV_CAN_IRQ_CALLBACK_T g_can_irq_callback = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
VOID rp2040_init_led_1_hw( VOID )
{
    gpio_init( LED_1_PORT );
    gpio_set_dir( LED_1_PORT, GPIO_OUT );
}

VOID rp2040_init_spi_1_hw( VOID )
{
    /* Configute SPI baudrate*/
    (VOID)spi_init( SPI_1_HW, SPI_1_BAUDRATE );

    /* Initialize MISO, MOSI and SCLK port */
    gpio_set_function( SPI_1_PORT_MISO, GPIO_FUNC_SPI );
    gpio_set_function( SPI_1_PORT_MOSI, GPIO_FUNC_SPI );
    gpio_set_function( SPI_1_PORT_SCLK, GPIO_FUNC_SPI );

    /* Initialize CS port */
    (VOID)gpio_init( SPI_1_PORT_CS );
    gpio_set_dir( SPI_1_PORT_CS, GPIO_OUT );
}

VOID rp2040_light_led_1( const BOOL lit )
{
    const BOOL lv = ( TRUE == lit ) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;

    gpio_put( LED_1_PORT, lv );
}

VOID rp2040_set_can_irq_callback( const HWDRV_CAN_IRQ_CALLBACK_T const callback )
{
    g_can_irq_callback = callback;
}

VOID rp2040_enable_can_irq( const BOOL enabled )
{
    gpio_set_irq_enabled_with_callback(
        CAN_CTRL_PORT_IRQ, GPIO_IRQ_LEVEL_LOW, enabled, irq_callback );
}

VOID rp2040_begin_spi_1( VOID )
{
    gpio_put( SPI_1_PORT_CS, GPIO_LEVEL_LOW );
}

VOID rp2040_end_spi_1( VOID )
{
    gpio_put( SPI_1_PORT_CS, GPIO_LEVEL_HIGH );
}

VOID rp2040_read_spi_1_array( const SIZE_T len, UINT8 *p_buff )
{
    if( ( 0U < len ) && ( NULL != p_buff ) )
    {
        (VOID)spi_read_blocking( SPI_1_HW, SPI_1_REPEATED_TX_DATA, p_buff, len );
    }
}

VOID rp2040_write_spi_1_array( const SIZE_T len, const UINT8 const *p_buff )
{
    if( ( 0U < len ) && ( NULL != p_buff ) )
    {
        (VOID)spi_write_blocking( SPI_1_HW, p_buff, len );
    }
}

UINT8 rp2040_read_spi_1( VOID )
{
    UINT8 val = 0U;

    rp2040_read_spi_1_array( sizeof( UINT8 ), &val );

    return val;
}

VOID rp2040_write_spi_1( const UINT8 val )
{
    rp2040_write_spi_1_array( sizeof( UINT8 ), &val );
}

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
static VOID irq_callback( const UINT gpio, const UINT32 events )
{
    if( ( NULL != g_can_irq_callback ) && ( CAN_CTRL_PORT_IRQ == gpio ) )
    {
        g_can_irq_callback();
    }
}
