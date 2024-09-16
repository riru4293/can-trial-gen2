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

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
/* MCP2515 operation mode */
#define OPMOD_NORMAL                    ( (uint8_t)0x00U )
#define OPMOD_SLEEP                     ( (uint8_t)0x20U )
#define OPMOD_LOOPBACK                  ( (uint8_t)0x40U )
#define OPMOD_LISTENONLY                ( (uint8_t)0x60U )
#define OPMOD_CONFIG                    ( (uint8_t)0x80U )

typedef enum en_can_frame_buff
{
    E_CAN_FRAME_SIDH = 0U,
    E_CAN_FRAME_SIDL,
    E_CAN_FRAME_EID8,
    E_CAN_FRAME_EID0,
    E_CAN_FRAME_DLC,
    E_CAN_FRAME_DATA_1,
    E_CAN_FRAME_DATA_2,
    E_CAN_FRAME_DATA_3,
    E_CAN_FRAME_DATA_4,
    E_CAN_FRAME_DATA_5,
    E_CAN_FRAME_DATA_6,
    E_CAN_FRAME_DATA_7,
    E_CAN_FRAME_DATA_8,
    E_CAN_FRAME_QTY
} can_frame_buff_t;

/* To standard CAN ID */
#define SIDH_L_SHIFT_TO_STD_CANID       ( (uint8_t)3U )
#define SIDL_R_SHIFT_TO_STD_CANID       ( (uint8_t)5U )

/* From standard CAN ID */
#define STD_CANID_MASK_TO_SIDH          ( (uint32_t)0x000007F8U )
#define STD_CANID_MASK_TO_SIDL          ( (uint32_t)0x00000007U )

#define STD_CANID_R_SHIFT_TO_SIDH       ( SIDH_L_SHIFT_IF_STD_CANID )
#define STD_CANID_L_SHIFT_TO_SIDL       ( SIDL_R_SHIFT_TO_STD_CANID )


/* To Extended CAN ID */
#define SIDL_E0_MASK_TO_STD_CANID       ( (uint8_t)0xE0U )
#define SIDL_03_MASK_TO_STD_CANID       ( (uint8_t)0x03U )

#define SIDH_L_SHIFT_TO_EXT_CANID       ( (uint8_t)21U )
#define SIDL_E0_L_SHIFT_TO_EXT_CANID    ( (uint8_t)13U )
#define SIDL_03_L_SHIFT_TO_EXT_CANID    ( (uint8_t)16U )
#define EID8_L_SHIFT_TO_EXT_CANID       ( (uint8_t) 8U )

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
static uint32_t build_std_canid( const uint8_t sidh, const uint8_t sidl );
static uint32_t build_ext_canid( const uint8_t sidh, const uint8_t sidl, const uint8_t eid8, const uint8_t eid0 );

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

void get_rx_buff( const can_rx_t can_rx, size_t len, uint8_t *p_buff )
{
    uint8_t reg_addr;

    switch( can_rx )
    {
    case E_CAN_RX1:
        reg_addr = REG_RXB0SIDH;
        break;
    case E_CAN_RX2:
        reg_addr = REG_RXB1SIDH;
        break;
    default:
        // Do nothing
        break;
    };

    if( ( NULL!= p_buff ) && ( E_CAN_FRAME_QTY == len ) )
    {
        read_reg_array( reg_addr, len, p_buff );
    }
}

void mcp2515_get_can_frame( const can_rx_t can_rx, can_frame_t *p_can_frame )
{
    uint8_t rx_buff[ E_CAN_FRAME_QTY ] = { 0U };

    uint32_t can_id;
    can_frame_kind_t can_kind;
    uint8_t can_dlc = CAN_DLC_INVALID;
    uint8_t can_data[ CAN_DLC_MAX ] = { 0 };

    // Get DLC
    p_can_frame->dlc = rx_buff[ E_CAN_FRAME_DLC ] & MASKOF_DLC;

    // Get Data
    if( ( 0U < p_can_frame->dlc ) && ( CAN_DLC_MAX >= p_can_frame->dlc ) )
    {
        memcpy( p_can_frame->data, &rx_buff[ E_CAN_FRAME_DATA_1 ], can_dlc );
    }

    // Is Standard? Extended?
    bool is_std;
    uint8_t sidl_ide;
    sidl_ide = (uint8_t)( rx_buff[ E_CAN_FRAME_SIDL ] & MASKOF_SIDL_IDE );
    is_std = ( REG_VAL_SIDL_IDE_STD != sidl_ide ) ? true : false;

    // Is Remote?
    bool is_remote;
    uint8_t sidl_srr;
    uint8_t dlc_rtr;
    if( true == is_std )
    {
        sidl_srr = (uint8_t)( rx_buff[ E_CAN_FRAME_SIDL ] & MASKOF_SIDL_SRR );
        is_remote = ( REG_VAL_SIDL_SRR_RMT == sidl_srr ) ? true : false;
    }
    else
    {
        dlc_rtr = (uint8_t)( rx_buff[ E_CAN_FRAME_DLC ] & MASKOF_RTR );
        is_remote = ( REG_VAL_RTR_RMT == sidl_srr ) ? true : false;
    }

    if( true == is_remote)
    {
        p_can_frame->kind = E_CAN_FRAME_REMOTE;
    }

    if( true == is_std )
    {
        p_can_frame->id = build_std_canid(
            rx_buff[ E_CAN_FRAME_SIDH ],
            rx_buff[ E_CAN_FRAME_SIDL ]
        );
    }
    else
    {
        p_can_frame->id = build_ext_canid(
            rx_buff[ E_CAN_FRAME_SIDH ],
            rx_buff[ E_CAN_FRAME_SIDL ],
            rx_buff[ E_CAN_FRAME_EID8 ],
            rx_buff[ E_CAN_FRAME_EID0 ]
        );
    }
}

static uint32_t build_std_canid( const uint8_t sidh, const uint8_t sidl )
{
    uint32_t can_id;

    can_id  = (uint32_t)(  (uint8_t)sidl >> SIDL_R_SHIFT_TO_STD_CANID );
    can_id |= (uint32_t)( (uint16_t)sidh << SIDH_L_SHIFT_TO_STD_CANID );

    return can_id;
}

static uint32_t build_ext_canid( const uint8_t sidh, const uint8_t sidl, const uint8_t eid8, const uint8_t eid0 )
{
    uint32_t can_id = CAN_ID_INVALID;
    uint32_t sidl_e0;
    uint32_t sidl_03;

    sidl_e0 = (uint32_t)( sidl & SIDL_E0_MASK_TO_STD_CANID );
    sidl_03 = (uint32_t)( sidl & SIDL_03_MASK_TO_STD_CANID );

    can_id  = (uint32_t)eid0;
    can_id |= (uint32_t)( (uint32_t)   eid8 <<    EID8_L_SHIFT_TO_EXT_CANID );
    can_id |= (uint32_t)(           sidl_03 << SIDL_03_L_SHIFT_TO_EXT_CANID );
    can_id |= (uint32_t)(           sidl_e0 << SIDL_E0_L_SHIFT_TO_EXT_CANID );
    can_id |= (uint32_t)( (uint32_t)   sidh <<    SIDH_L_SHIFT_TO_EXT_CANID );

    return can_id;
}

// static void set_can_id( const can_frame_kind_t kind, const uint32_t can_id,
//     uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

//     /* Fails if illegal arguments. */
//     if ( is_invalid_canid_kind( kind ) || ( NULL == hdr ) )
//         return CD_FAILURE;

//     if ( CD_CANID_KIND_STD == kind ) {
//         /*--------------------------*/
//         /* Case of standard format. */
//         /*--------------------------*/

//         /* Fails if too large. */
//         if ( CD_MAXOF_STD_CANID < can_id )
//             return CD_FAILURE;

//         hdr[ MCP2515_CANHDR_EID0 ] = 0U;
//         hdr[ MCP2515_CANHDR_EID8 ] = 0U;
//         hdr[ MCP2515_CANHDR_SIDL ] = (uint8_t)( ( can_id << 5U ) & 0xE0U );
//         hdr[ MCP2515_CANHDR_SIDH ] = (uint8_t)( can_id >> 3U );
//     } 
//     else {
//         /*--------------------------*/
//         /* Case of extended format. */
//         /*--------------------------*/

//         /* Fails if too large. */
//         if ( CD_MAXOF_EXT_CANID < can_id )
//             return CD_FAILURE;

//         hdr[ MCP2515_CANHDR_EID0 ] = (uint8_t)can_id;
//         hdr[ MCP2515_CANHDR_EID8 ] = (uint8_t)( can_id >> 8U );
//         hdr[ MCP2515_CANHDR_SIDL ] = (uint8_t)( ( ( can_id >> 16U ) & 0x03U )
//             | MASKOF_SIDL_IDE | ( ( can_id >> 13U ) & 0xE0U ) );
//         hdr[ MCP2515_CANHDR_SIDH ] = (uint8_t)( can_id >> 21U );
//     }
// }

// static cd_result_t set_can_dlc( const uint8_t dlc, const bool is_remote,
//     uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

//     uint8_t reg_val = 0U;

//     /* Fails if illegal arguments. */
//     if ( ( CD_CANDLC_MAXOF_LEN < dlc ) || ( NULL == hdr ) )
//         return CD_FAILURE;
    
//     reg_val = (uint8_t)( reg_val | (uint8_t)( dlc & 0xF ) );

//     if( true == is_remote ) {

//         reg_val = (uint8_t)( reg_val | MASKOF_RTR );
//     }

//     hdr[ MCP2515_CANHDR_DLC ] = reg_val;

//     return CD_SUCCESS;
// }

