/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <string.h>

#include <app_errno.h>
#include <private/rp2040.h>
#include <private/mcp2515.h>
#include <private/mcp2515_spi_cmd.h>
#include <private/mcp2515_reg.h>
#include <private/can_irq.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
/* MCP2515 operation mode */
#define OPMOD_NORMAL                    ( (uint8_t)0x00U )
#define OPMOD_SLEEP                     ( (uint8_t)0x20U )
#define OPMOD_LOOPBACK                  ( (uint8_t)0x40U )
#define OPMOD_LISTENONLY                ( (uint8_t)0x60U )
#define OPMOD_CONFIG                    ( (uint8_t)0x80U )

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void reset_controller( void );
static void set_can_baudrate( void );
static void wait_until_change_opmod( const uint8_t expect_opmod );
static uint8_t get_opmod( void );
static void set_opmod( const uint8_t opmod );
static void wakeup( void );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static fn_can_irq_cbk g_can_irq_cbk = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
void mcp2515_reset( void )
{
    /* Execute reset CAN controller */
    reset_controller();

    /* Wait reset completion */
    wait_until_change_opmod( OPMOD_CONFIG );

    /* Set CAN baudrate 125Kbps */
    set_can_baudrate();

    /* 受信バッファ１設定。すべて受信。RX1への切り替え禁止。*/
    /* Set RX1 filter */
    mcp2515_write_reg( REG_ADDR_RXB0CTRL, 0x60 );

    /* 受信バッファ２設定。フィルタ一致のみ受信。 */
    /* Set RX2 filter */
    mcp2515_write_reg( REG_ADDR_RXB1CTRL, 0x00 );
}

void mcp2515_start_comm( void )
{
    uint8_t opmod;

    opmod = get_opmod();

    if( OPMOD_CONFIG == opmod )
    {
        set_opmod( OPMOD_NORMAL );
    }
}

void mcp2515_set_irq_cbk( const fn_can_irq_cbk cbk )
{
    /* Allow set NULL */
    g_can_irq_cbk = cbk;
}

void mcp2515_enable_irq_fact( const uint8_t fact )
{
    /* Enable specified IRQ factor */
    mcp2515_modify_reg( REG_ADDR_CANINTE, fact, REG_VAL_FF );
}

/* -------------------------------------------------------------------------- */
/* Friend functions                                                           */
/* -------------------------------------------------------------------------- */
void mcp2515_can_irq_cbk( void )
{
    uint8_t ocurred;

    if( NULL != g_can_irq_cbk )
    {
        /* Get occurred IRQ factor */
        ocurred = mcp2515_read_reg( REG_ADDR_CANINTF );

        /* Disable re-cause occurred IRQ factor */
        mcp2515_modify_reg( REG_ADDR_CANINTE, ocurred, REG_VAL_00 );

        /* Clear occurred IRQ factor */
        mcp2515_modify_reg( REG_ADDR_CANINTF, ocurred, REG_VAL_00 );

        /* Indicate IRQ factor */
        g_can_irq_cbk( ocurred );
    }
}

void mcp2515_begin_spi( void )
{
    rp2040_begin_spi_1();
}

void mcp2515_end_spi( void )
{
    rp2040_end_spi_1();
}

void mcp2515_write_spi( const uint8_t val )
{
    rp2040_write_spi_1( val );
}

void mcp2515_write_spi_array( const size_t n, const uint8_t const buff[n] )
{
    rp2040_write_spi_1_array( n, buff );
}

void mcp2515_write_reg_array( const uint8_t addr, const size_t n, const uint8_t const buff[n] )
{
    /* Begin SPI communication */
    mcp2515_begin_spi();

    /* Write to MCP2515 registers */
    mcp2515_write_spi( SPI_CMD_WRITE_REG );
    mcp2515_write_spi( addr );
    mcp2515_write_spi_array( n, buff );

    /* End SPI communication */
    mcp2515_end_spi();
}

void mcp2515_write_reg( const uint8_t addr, const uint8_t val )
{
    mcp2515_write_reg_array( addr, sizeof( uint8_t ), &val );
}

void mcp2515_modify_reg( const uint8_t addr, const uint8_t mask, const uint8_t val ) {

    /* Begin SPI communication */
    mcp2515_begin_spi();
    
    /* Write to MCP2515 register with bit mask */
    mcp2515_write_spi( SPI_CMD_MODIFY_REG );
    mcp2515_write_spi( addr );
    mcp2515_write_spi( mask );
    mcp2515_write_spi( val );

    /* End SPI communication */
    mcp2515_end_spi();
}

uint8_t mcp2515_read_spi( void )
{
    return rp2040_read_spi_1();
}

void mcp2515_read_spi_array( const size_t n, uint8_t buff[n] )
{
    rp2040_read_spi_1_array( n, buff );
}

void mcp2515_read_reg_array( const uint8_t addr, const size_t n, uint8_t buff[n] )
{
    /* Begin SPI communication */
    mcp2515_begin_spi();

    /* Write to MCP2515 registers */
    mcp2515_write_spi( SPI_CMD_READ_REG );
    mcp2515_write_spi( addr );
    mcp2515_read_spi_array( n, buff );

    /* End SPI communication */
    mcp2515_end_spi();
}

uint8_t mcp2515_read_reg( const uint8_t addr )
{
    uint8_t val = 0U;

    mcp2515_read_reg_array( addr, sizeof( val ), &val );

    return val;
}

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
static void reset_controller( void )
{
    /* Begin SPI communication */
    mcp2515_begin_spi();

    /* Execute reset command */
    mcp2515_write_spi( SPI_CMD_RESET );

    /* End SPI communication */
    mcp2515_end_spi();
}

static void set_can_baudrate( void )
{
    const uint8_t baudrate[] = {
        /* +-------+-------+-------+-----------+-----+----+-----+-----+---------+------+------+ */
        /* |  CNF3 |  CNF2 |  CNF1 |  BAUDLATE | BRP | TQ | PS1 | PS2 | Smpl Pt | Sync | Prop | */
        /* +-------+-------+-------+-----------+-----+----+-----+-----+---------+------+------+ */
             0x02U , 0x89U , 0x07U /*  125KBPS |   7 |  8 |   2 |   3 |  62.50% |   1  |    2 | */
        /*   0x02U , 0x89U , 0x03U |   250KBPS |   3 |  8 |   2 |   3 |  62.50% |   1  |    2 | */
        /*   0x02U , 0x89U , 0x01U |   500KBPS |   1 |  8 |   2 |   3 |  62.50% |   1  |    2 | */
        /*   0x02U , 0x89U , 0x00U |  1000KBPS |   0 |  8 |   2 |   3 |  62.50% |   1  |    2 | */
        /* +-------+-------+-------+-----------+-----+----+-----+-----+---------+------+------+ */
    };

    /* Write baudrate configuration to register */
    mcp2515_write_reg_array( REG_ADDR_CNF3, sizeof( baudrate ), baudrate );
}

static void wait_until_change_opmod( const uint8_t expect_opmod )
{
    uint8_t current_opmod;

    do
    {
        current_opmod = get_opmod();
    } while ( expect_opmod != current_opmod );
}

static uint8_t get_opmod( void )
{
    uint8_t canstat;
    uint8_t opmod;

    canstat = mcp2515_read_reg( REG_ADDR_CANSTAT );
    opmod = canstat & REG_MASK_CANSTAT_OPMOD;

    return opmod;
}

static void set_opmod( const uint8_t opmod )
{
    const uint8_t current_opmod = get_opmod();

    /* Do nothing if the operation mode does not change. */
    if( current_opmod != opmod )
    {
        /* Wake up if current mode is sleep. */
        if( OPMOD_SLEEP == current_opmod )
        {
            wakeup();
        }

        /* Clear waked up interruption if to be sleep. */
        if ( OPMOD_SLEEP == opmod )
        {
            mcp2515_modify_reg( REG_ADDR_CANINTF, REG_MASK_CANINT_WAKIF, REG_VAL_00 );
        }

        /* Set operation mode */
        mcp2515_modify_reg( REG_ADDR_CANCTRL, REG_MASK_CANSTAT_OPMOD, opmod );

        /* Wait until applied. */
        return wait_until_change_opmod( opmod );
    }
}

static void wakeup( void )
{
    /* Disable wake up IRQ */
    mcp2515_modify_reg( REG_ADDR_CANINTE, REG_MASK_CANINT_WAKIF, REG_VAL_00 );

    /* Clear wake up IRQ */
    mcp2515_modify_reg( REG_ADDR_CANINTF, REG_MASK_CANINT_WAKIF, REG_VAL_00 );

    /* Enable wake up IRQ */
    mcp2515_modify_reg( REG_ADDR_CANINTE, REG_MASK_CANINT_WAKIF, REG_VAL_FF );

    /* Set wake up IRQ. */
    mcp2515_modify_reg( REG_ADDR_CANINTF, REG_MASK_CANINT_WAKIF, REG_VAL_FF );

    /* Temporarily switch to listen-only mode. */
    set_opmod( OPMOD_LISTENONLY );

    /* Clear wake up IRQ */
    mcp2515_modify_reg( REG_ADDR_CANINTF, REG_MASK_CANINT_WAKIF, REG_VAL_00 );
}
