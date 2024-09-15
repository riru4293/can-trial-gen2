/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <string.h>

/* Driver */
#include <private/rp2040.h>
#include <private/mcp2515.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
/* SPI command */
#define SPICMD_WRITE_REG                ( (uint8_t)0x02U )
#define SPICMD_READ_REG                 ( (uint8_t)0x03U )
#define SPICMD_MODBITS_REG              ( (uint8_t)0x05U )
#define SPICMD_WRITE_TX0_ID             ( (uint8_t)0x40U )
#define SPICMD_WRITE_TX0_CONTENT        ( (uint8_t)0x41U )
#define SPICMD_WRITE_TX1_ID             ( (uint8_t)0x42U )
#define SPICMD_WRITE_TX1_CONTENT        ( (uint8_t)0x43U )
#define SPICMD_WRITE_TX2_ID             ( (uint8_t)0x44U )
#define SPICMD_WRITE_TX2_CONTENT        ( (uint8_t)0x45U )
#define SPICMD_REQ_TX0                  ( (uint8_t)0x81U )
#define SPICMD_REQ_TX1                  ( (uint8_t)0x82U )
#define SPICMD_REQ_TX2                  ( (uint8_t)0x83U )
#define SPICMD_READ_RX0_HDR             ( (uint8_t)0x90U )
#define SPICMD_READ_RX0_BODY            ( (uint8_t)0x92U )
#define SPICMD_READ_RX1_HDR             ( (uint8_t)0x94U )
#define SPICMD_READ_RX1_BODY            ( (uint8_t)0x96U )
#define SPICMD_READ_STAT                ( (uint8_t)0xA0U )
#define SPICMD_READ_RXSTAT              ( (uint8_t)0xB0U )
#define SPICMD_RESET                    ( (uint8_t)0xC0U )

/* Register */
#define REG_RXF0SIDH                    ( (uint8_t)0x00U )
#define REG_RXF0SIDL                    ( (uint8_t)0x01U )
#define REG_RXF0EID8                    ( (uint8_t)0x02U )
#define REG_RXF0EID0                    ( (uint8_t)0x03U )
#define REG_RXF1SIDH                    ( (uint8_t)0x04U )
#define REG_RXF1SIDL                    ( (uint8_t)0x05U )
#define REG_RXF1EID8                    ( (uint8_t)0x06U )
#define REG_RXF1EID0                    ( (uint8_t)0x07U )
#define REG_RXF2SIDH                    ( (uint8_t)0x08U )
#define REG_RXF2SIDL                    ( (uint8_t)0x09U )
#define REG_RXF2EID8                    ( (uint8_t)0x0AU )
#define REG_RXF2EID0                    ( (uint8_t)0x0BU )
#define REG_BFPCTRL                     ( (uint8_t)0x0CU )
#define REG_TXRTSCTRL                   ( (uint8_t)0x0DU )
#define REG_CANSTAT                     ( (uint8_t)0x0EU )
#define REG_CANCTRL                     ( (uint8_t)0x0FU )
#define REG_RXF3SIDH                    ( (uint8_t)0x10U )
#define REG_RXF3SIDL                    ( (uint8_t)0x11U )
#define REG_RXF3EID8                    ( (uint8_t)0x12U )
#define REG_RXF3EID0                    ( (uint8_t)0x13U )
#define REG_RXF4SIDH                    ( (uint8_t)0x14U )
#define REG_RXF4SIDL                    ( (uint8_t)0x15U )
#define REG_RXF4EID8                    ( (uint8_t)0x16U )
#define REG_RXF4EID0                    ( (uint8_t)0x17U )
#define REG_RXF5SIDH                    ( (uint8_t)0x18U )
#define REG_RXF5SIDL                    ( (uint8_t)0x19U )
#define REG_RXF5EID8                    ( (uint8_t)0x1AU )
#define REG_RXF5EID0                    ( (uint8_t)0x1BU )
#define REG_TEC                         ( (uint8_t)0x1CU )
#define REG_REC                         ( (uint8_t)0x1DU )
#define REG_RXM0SIDH                    ( (uint8_t)0x20U )
#define REG_RXM0SIDL                    ( (uint8_t)0x21U )
#define REG_RXM0EID8                    ( (uint8_t)0x22U )
#define REG_RXM0EID0                    ( (uint8_t)0x23U )
#define REG_RXM1SIDH                    ( (uint8_t)0x24U )
#define REG_RXM1SIDL                    ( (uint8_t)0x25U )
#define REG_RXM1EID8                    ( (uint8_t)0x26U )
#define REG_RXM1EID0                    ( (uint8_t)0x27U )
#define REG_CNF3                        ( (uint8_t)0x28U )
#define REG_CNF2                        ( (uint8_t)0x29U )
#define REG_CNF1                        ( (uint8_t)0x2AU )
#define REG_CANINTE                     ( (uint8_t)0x2BU )
#define REG_CANINTF                     ( (uint8_t)0x2CU )
#define REG_EFLG                        ( (uint8_t)0x2DU )
#define REG_TXB0CTRL                    ( (uint8_t)0x30U )
#define REG_TXB0SIDH                    ( (uint8_t)0x31U )
#define REG_TXB0SIDL                    ( (uint8_t)0x32U )
#define REG_TXB0EID8                    ( (uint8_t)0x33U )
#define REG_TXB0EID0                    ( (uint8_t)0x34U )
#define REG_TXB0DLC                     ( (uint8_t)0x35U )
#define REG_TXB0D0                      ( (uint8_t)0x36U )
#define REG_TXB0D1                      ( (uint8_t)0x37U )
#define REG_TXB0D2                      ( (uint8_t)0x38U )
#define REG_TXB0D3                      ( (uint8_t)0x39U )
#define REG_TXB0D4                      ( (uint8_t)0x3AU )
#define REG_TXB0D5                      ( (uint8_t)0x3BU )
#define REG_TXB0D6                      ( (uint8_t)0x3CU )
#define REG_TXB0D7                      ( (uint8_t)0x3DU )
#define REG_TXB1CTRL                    ( (uint8_t)0x40U )
#define REG_TXB1SIDH                    ( (uint8_t)0x41U )
#define REG_TXB1SIDL                    ( (uint8_t)0x42U )
#define REG_TXB1EID8                    ( (uint8_t)0x43U )
#define REG_TXB1EID0                    ( (uint8_t)0x44U )
#define REG_TXB1DLC                     ( (uint8_t)0x45U )
#define REG_TXB1D0                      ( (uint8_t)0x46U )
#define REG_TXB1D1                      ( (uint8_t)0x47U )
#define REG_TXB1D2                      ( (uint8_t)0x48U )
#define REG_TXB1D3                      ( (uint8_t)0x49U )
#define REG_TXB1D4                      ( (uint8_t)0x4AU )
#define REG_TXB1D5                      ( (uint8_t)0x4BU )
#define REG_TXB1D6                      ( (uint8_t)0x4CU )
#define REG_TXB1D7                      ( (uint8_t)0x4DU )
#define REG_TXB2CTRL                    ( (uint8_t)0x50U )
#define REG_TXB2SIDH                    ( (uint8_t)0x51U )
#define REG_TXB2SIDL                    ( (uint8_t)0x52U )
#define REG_TXB2EID8                    ( (uint8_t)0x53U )
#define REG_TXB2EID0                    ( (uint8_t)0x54U )
#define REG_TXB2DLC                     ( (uint8_t)0x55U )
#define REG_TXB2D0                      ( (uint8_t)0x56U )
#define REG_TXB2D1                      ( (uint8_t)0x57U )
#define REG_TXB2D2                      ( (uint8_t)0x58U )
#define REG_TXB2D3                      ( (uint8_t)0x59U )
#define REG_TXB2D4                      ( (uint8_t)0x5AU )
#define REG_TXB2D5                      ( (uint8_t)0x5BU )
#define REG_TXB2D6                      ( (uint8_t)0x5CU )
#define REG_TXB2D7                      ( (uint8_t)0x5DU )
#define REG_RXB0CTRL                    ( (uint8_t)0x60U )
#define REG_RXB0SIDH                    ( (uint8_t)0x61U )
#define REG_RXB0SIDL                    ( (uint8_t)0x62U )
#define REG_RXB0EID8                    ( (uint8_t)0x63U )
#define REG_RXB0EID0                    ( (uint8_t)0x64U )
#define REG_RXB0DLC                     ( (uint8_t)0x65U )
#define REG_RXB0D0                      ( (uint8_t)0x66U )
#define REG_RXB0D1                      ( (uint8_t)0x67U )
#define REG_RXB0D2                      ( (uint8_t)0x68U )
#define REG_RXB0D3                      ( (uint8_t)0x69U )
#define REG_RXB0D4                      ( (uint8_t)0x6AU )
#define REG_RXB0D5                      ( (uint8_t)0x6BU )
#define REG_RXB0D6                      ( (uint8_t)0x6CU )
#define REG_RXB0D7                      ( (uint8_t)0x6DU )
#define REG_RXB1CTRL                    ( (uint8_t)0x70U )
#define REG_RXB1SIDH                    ( (uint8_t)0x71U )
#define REG_RXB1SIDL                    ( (uint8_t)0x72U )
#define REG_RXB1EID8                    ( (uint8_t)0x73U )
#define REG_RXB1EID0                    ( (uint8_t)0x74U )
#define REG_RXB1DLC                     ( (uint8_t)0x75U )
#define REG_RXB1D0                      ( (uint8_t)0x76U )
#define REG_RXB1D1                      ( (uint8_t)0x77U )
#define REG_RXB1D2                      ( (uint8_t)0x78U )
#define REG_RXB1D3                      ( (uint8_t)0x79U )
#define REG_RXB1D4                      ( (uint8_t)0x7AU )
#define REG_RXB1D5                      ( (uint8_t)0x7BU )
#define REG_RXB1D6                      ( (uint8_t)0x7CU )
#define REG_RXB1D7                      ( (uint8_t)0x7DU )

/* Register value */
#define REG_VAL_00                      ( (uint8_t)0x00U )
#define REG_VAL_FF                      ( (uint8_t)0xFFU )

/* Mask of the register value */
#define MASKOF_OPMOD                    ( (uint8_t)0xE0U )
#define MASKOF_CANSTAT_ICOD             ( (uint8_t)0x0EU )
#define MASKOF_CANCTRL_ABAT             ( (uint8_t)0x10U )
#define MASKOF_CANCTRL_OSM              ( (uint8_t)0x08U )
#define MASKOF_CANINT_MERRF             ( (uint8_t)0x80U )
#define MASKOF_CANINT_WAKIF             ( (uint8_t)0x40U )
#define MASKOF_CANINT_ERRIF             ( (uint8_t)0x20U )
#define MASKOF_CANINT_TX2IF             ( (uint8_t)0x10U )
#define MASKOF_CANINT_TX1IF             ( (uint8_t)0x08U )
#define MASKOF_CANINT_TX0IF             ( (uint8_t)0x04U )
#define MASKOF_CANINT_RX1IF             ( (uint8_t)0x02U )
#define MASKOF_CANINT_RX0IF             ( (uint8_t)0x01U )
#define MASKOF_EFLG_RX1OVR              ( (uint8_t)0x80U )
#define MASKOF_EFLG_RX0OVR              ( (uint8_t)0x40U )
#define MASKOF_EFLG_TXBO                ( (uint8_t)0x20U )
#define MASKOF_EFLG_TXEP                ( (uint8_t)0x10U )
#define MASKOF_EFLG_RXEP                ( (uint8_t)0x08U )
#define MASKOF_EFLG_TXWAR               ( (uint8_t)0x04U )
#define MASKOF_EFLG_RXWAR               ( (uint8_t)0x02U )
#define MASKOF_EFLG_EWARN               ( (uint8_t)0x01U )
#define MASKOF_TXBCTRL_ABTF             ( (uint8_t)0x40U )
#define MASKOF_TXBCTRL_MLOA             ( (uint8_t)0x20U )
#define MASKOF_TXBCTRL_TXERR            ( (uint8_t)0x10U )
#define MASKOF_TXBCTRL_TXREQ            ( (uint8_t)0x08U )
#define MASKOF_TXBCTRL_TXP              ( (uint8_t)0x03U )
#define MASKOF_RXBCTRL_RXM              ( (uint8_t)0x60U )
#define MASKOF_RXBCTRL_RXRTR            ( (uint8_t)0x08U )
#define MASKOF_RXB0CTRL_BUKT            ( (uint8_t)0x04U )
#define MASKOF_RXB0CTRL_FILHIT          ( (uint8_t)0x01U )
#define MASKOF_RXB1CTRL_FILHIT          ( (uint8_t)0x07U )
#define MASKOF_SIDH_CANID               ( (uint8_t)0XFFU )
#define MASKOF_SIDL_CANID               ( (uint8_t)0XE0U )
#define MASKOF_SIDL_IDE                 ( (uint8_t)0x08U )
#define MASKOF_RTR                      ( (uint8_t)0x40U )
#define MASKOF_DLC                      ( (uint8_t)0x0FU )

/* MCP2515 operation mode */
#define OPMOD_NORMAL                    ( (uint8_t)0x00U )
#define OPMOD_SLEEP                     ( (uint8_t)0x20U )
#define OPMOD_LOOPBACK                  ( (uint8_t)0x40U )
#define OPMOD_LISTENONLY                ( (uint8_t)0x60U )
#define OPMOD_CONFIG                    ( (uint8_t)0x80U )

#define CAN_HDR_SIDH                    ( (uint8_t)0U )
#define CAN_HDR_SIDL                    ( (uint8_t)1U )
#define CAN_HDR_EID8                    ( (uint8_t)2U )
#define CAN_HDR_EID0                    ( (uint8_t)3U )
#define CAN_HDR_DLC                     ( (uint8_t)4U )
#define CAN_HDR_LEN                     ( (uint8_t)5U )
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
static uint32_t build_std_canid( const size_t len, uint8_t *p_hdr );

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

void mcp2515_get_can_msg( const EN_HWDRV_CAN_RX can_rx, can_frame_t *p_msg )
{
    uint8_t hdr[ CAN_HDR_LEN ] = { 0U };
    uint8_t msg[ CAN_DLC_MAX ] = { 0U };
    uint32_t can_id;

    uint8_t tttt;

    tttt = read_reg( REG_RXB0D0 );
    tttt = 0U;

    /* Read CAN header from register. */
    begin_spi();
    write_spi( SPICMD_READ_RX0_HDR );
    read_spi_array( CAN_HDR_LEN, hdr );
    end_spi();

    can_id = build_std_canid( CAN_HDR_LEN, hdr );

    begin_spi();
    write_spi( SPICMD_READ_RX0_BODY );
    read_spi_array( CAN_DLC_MAX, msg );
    end_spi();


    p_msg->id = can_id;
    memcpy( p_msg->data, msg, CAN_DLC_MAX );

    /* Clear interruption by received. */
    write_reg( REG_CANINTF, REG_VAL_00 );
}

static uint32_t build_std_canid( const size_t len, uint8_t *p_hdr ) {
#define SIDH_CANID_OFFSET ( (uint8_t)3U )
#define SIDL_CANID_OFFSET ( (uint8_t)5U )

    return (uint32_t)(
        (uint16_t)( (uint16_t)p_hdr[ CAN_HDR_SIDH ] << SIDH_CANID_OFFSET )
      | (uint16_t)( (uint8_t)( p_hdr[ CAN_HDR_SIDL ] & MASKOF_SIDL_CANID ) >> SIDL_CANID_OFFSET )
    );
}
