/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <string.h>

/* My standard library */
#include <my_error_code.h>

/* Driver */
#include <private/rp2040.h>
#include <private/mcp2515.h>
#include <private/mcp2515_spicmd.h>
#include <private/mcp2515_register.h>
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
static void begin_spi( void );
static void end_spi( void );
static void write_spi( const uint8_t val );
static void write_spi_array( const size_t len, const uint8_t const *p_buff );
static void write_reg_array( const uint8_t addr, const size_t len, const uint8_t const *p_buff );
static void write_reg( const uint8_t addr, const uint8_t val );
static void modify_reg( const uint8_t addr, const uint8_t mask, const uint8_t val );
static uint8_t read_spi( void );
static void read_spi_array( const size_t len, uint8_t *p_buff );
static void read_reg_array( const uint8_t addr, const size_t len, uint8_t *p_buff );
static uint8_t read_reg( const uint8_t addr );
static void exec_cmd_reset( void );
static void configure_can_baudrate( void );
static void wait_until_change_opmod( const uint8_t expect_opmod );
static uint8_t get_opmod( void );
static void set_opmod( const uint8_t opmod );
static void wakeup( void );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static can_irq_callback_t g_can_irq_callback = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
void mcp2515_reset( void )
{
    /* Execute reset command */
    exec_cmd_reset();

    /* Wait reset completion */
    wait_until_change_opmod( OPMOD_CONFIG );

    /* Configure CAN baudrate 125Kbps */
    configure_can_baudrate();

    /* 受信バッファ１設定。すべて受信。RX1への切り替え禁止。*/
    //configure_rx_1_filter
    write_reg( REG_RXB0CTRL, 0x60 );

    /* 受信バッファ２設定。フィルタ一致のみ受信。 */
    //configure_rx_2_filter
    write_reg( REG_RXB1CTRL, 0x00 );
}

void mcp2515_begin_communication( void )
{
    set_opmod( OPMOD_NORMAL );
}

void mcp2515_set_can_irq_callback( const can_irq_callback_t callback )
{
    g_can_irq_callback = callback;
}

void mcp2515_enable_can_irq_fact( const uint8_t fact )
{
    /* Enable IRQ factor */
    modify_reg( REG_CANINTE, fact, REG_VAL_FF );
}

/* -------------------------------------------------------------------------- */
/* Friend functions                                                           */
/* -------------------------------------------------------------------------- */
void mcp2515_can_irq_callback( void )
{
    uint8_t ocurred;

    if( NULL != g_can_irq_callback )
    {
        /* Get occurred IRQ factor */
        ocurred = read_reg( REG_CANINTF );

        /* Disable re-cause occurred IRQ factor */
        modify_reg( REG_CANINTE, ocurred, REG_VAL_00 );

        /* Clear occurred IRQ factor */
        modify_reg( REG_CANINTF, ocurred, REG_VAL_00 );

        /* Indicate IRQ factor */
        g_can_irq_callback( ocurred );
    }
}

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
static void begin_spi( void )
{
    rp2040_begin_spi_1();
}

static void end_spi( void )
{
    rp2040_end_spi_1();
}

static void write_spi( const uint8_t val )
{
    rp2040_write_spi_1( val );
}

static void write_spi_array( const size_t len, const uint8_t const *p_buff )
{
    rp2040_write_spi_1_array( len, p_buff );
}

static void write_reg_array( const uint8_t addr, const size_t len, const uint8_t const *p_buff )
{
    /* Begin SPI communication */
    begin_spi();

    /* Write to MCP2515 registers */
    write_spi( SPICMD_WRITE_REG );
    write_spi( addr );
    write_spi_array( len, p_buff );

    /* End SPI communication */
    end_spi();
}

static void write_reg( const uint8_t addr, const uint8_t val )
{
    write_reg_array( addr, sizeof( uint8_t ), &val );
}

static void modify_reg( const uint8_t addr, const uint8_t mask, const uint8_t val ) {

    /* Begin SPI communication */
    begin_spi();
    
    /* Write to MCP2515 register with bit mask */
    write_spi( SPICMD_MODBITS_REG );
    write_spi( addr );
    write_spi( mask );
    write_spi( val );

    /* End SPI communication */
    end_spi();
}

static uint8_t read_spi( void )
{
    return rp2040_read_spi_1();
}

static void read_spi_array( const size_t len, uint8_t *p_buff )
{
    rp2040_read_spi_1_array( len, p_buff );
}

static void read_reg_array( const uint8_t addr, const size_t len, uint8_t *p_buff )
{
    /* Begin SPI communication */
    begin_spi();

    /* Write to MCP2515 registers */
    write_spi( SPICMD_READ_REG );
    write_spi( addr );
    read_spi_array( len, p_buff );

    /* End SPI communication */
    end_spi();
}

static uint8_t read_reg( const uint8_t addr )
{
    uint8_t val = 0U;

    read_reg_array( addr, sizeof( uint8_t ), &val );

    return val;
}

static void exec_cmd_reset( void )
{
    /* Begin SPI communication */
    begin_spi();

    /* Execute reset command */
    write_spi( SPICMD_RESET );

    /* End SPI communication */
    end_spi();
}

static void configure_can_baudrate( void )
{
    const uint8_t baudrate[] = {
        /* +-------+-------+-------+-----------+-----+----+-----+-----+---------+------+------+ */
        /* |  CNF3 |  CNF2 |  CNF1 |  BAUDLATE | BRP | TQ | PS1 | PS2 | Smpl Pt | Sync | Prop | */
        /* +-------+-------+-------+-----------+-----+----+-----+-----+---------+------+------+ */
             0x02U , 0x89U , 0x07U /*  125KBPS |   7 |  8 |   2 |   3 |  62.50% |   1  |    2 | */
        /*   0x02U , 0x89U , 0x03U /*  250KBPS |   3 |  8 |   2 |   3 |  62.50% |   1  |    2 | */
        /*   0x02U , 0x89U , 0x01U /*  500KBPS |   1 |  8 |   2 |   3 |  62.50% |   1  |    2 | */
        /*   0x02U , 0x89U , 0x00U /* 1000KBPS |   0 |  8 |   2 |   3 |  62.50% |   1  |    2 | */
        /* +-------+-------+-------+-----------+-----+----+-----+-----+---------+------+------+ */
    };

    /* Write baudrate configuration to register */
    write_reg_array( REG_CNF3, sizeof( baudrate ), baudrate );
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

    canstat = read_reg( REG_CANSTAT );
    opmod = canstat & MASKOF_OPMOD;

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
            modify_reg( REG_CANINTF, MASKOF_CANINT_WAKIF, REG_VAL_00 );
        }

        /* Set operation mode */
        modify_reg( REG_CANCTRL, MASKOF_OPMOD, opmod );

        /* Wait until applied. */
        return wait_until_change_opmod( opmod );
    }
}

static void wakeup( void )
{
    /* Disable wake up IRQ */
    modify_reg( REG_CANINTE, MASKOF_CANINT_WAKIF, REG_VAL_00 );

    /* Clear wake up IRQ */
    modify_reg( REG_CANINTF, MASKOF_CANINT_WAKIF, REG_VAL_00 );

    /* Enable wake up IRQ */
    modify_reg( REG_CANINTE, MASKOF_CANINT_WAKIF, REG_VAL_FF );

    /* Set wake up IRQ. */
    modify_reg( REG_CANINTF, MASKOF_CANINT_WAKIF, REG_VAL_FF );

    /* Temporarily switch to listen-only mode. */
    set_opmod( OPMOD_LISTENONLY );

    /* Clear wake up IRQ */
    modify_reg( REG_CANINTF, MASKOF_CANINT_WAKIF, REG_VAL_00 );
}
