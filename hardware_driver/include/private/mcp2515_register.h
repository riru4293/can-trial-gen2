#ifndef MCP2515_REGISTER_H
#define MCP2515_REGISTER_H

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
/* Register address */
#define REG_ADR_RXF0SIDH                ( (uint8_t)0x00U )
#define REG_ADR_RXF0SIDL                ( (uint8_t)0x01U )
#define REG_ADR_RXF0EID8                ( (uint8_t)0x02U )
#define REG_ADR_RXF0EID0                ( (uint8_t)0x03U )
#define REG_ADR_RXF1SIDH                ( (uint8_t)0x04U )
#define REG_ADR_RXF1SIDL                ( (uint8_t)0x05U )
#define REG_ADR_RXF1EID8                ( (uint8_t)0x06U )
#define REG_ADR_RXF1EID0                ( (uint8_t)0x07U )
#define REG_ADR_RXF2SIDH                ( (uint8_t)0x08U )
#define REG_ADR_RXF2SIDL                ( (uint8_t)0x09U )
#define REG_ADR_RXF2EID8                ( (uint8_t)0x0AU )
#define REG_ADR_RXF2EID0                ( (uint8_t)0x0BU )
#define REG_ADR_BFPCTRL                 ( (uint8_t)0x0CU )
#define REG_ADR_TXRTSCTRL               ( (uint8_t)0x0DU )
#define REG_ADR_CANSTAT                 ( (uint8_t)0x0EU )
#define REG_ADR_CANCTRL                 ( (uint8_t)0x0FU )
#define REG_ADR_RXF3SIDH                ( (uint8_t)0x10U )
#define REG_ADR_RXF3SIDL                ( (uint8_t)0x11U )
#define REG_ADR_RXF3EID8                ( (uint8_t)0x12U )
#define REG_ADR_RXF3EID0                ( (uint8_t)0x13U )
#define REG_ADR_RXF4SIDH                ( (uint8_t)0x14U )
#define REG_ADR_RXF4SIDL                ( (uint8_t)0x15U )
#define REG_ADR_RXF4EID8                ( (uint8_t)0x16U )
#define REG_ADR_RXF4EID0                ( (uint8_t)0x17U )
#define REG_ADR_RXF5SIDH                ( (uint8_t)0x18U )
#define REG_ADR_RXF5SIDL                ( (uint8_t)0x19U )
#define REG_ADR_RXF5EID8                ( (uint8_t)0x1AU )
#define REG_ADR_RXF5EID0                ( (uint8_t)0x1BU )
#define REG_ADR_TEC                     ( (uint8_t)0x1CU )
#define REG_ADR_REC                     ( (uint8_t)0x1DU )
#define REG_ADR_RXM0SIDH                ( (uint8_t)0x20U )
#define REG_ADR_RXM0SIDL                ( (uint8_t)0x21U )
#define REG_ADR_RXM0EID8                ( (uint8_t)0x22U )
#define REG_ADR_RXM0EID0                ( (uint8_t)0x23U )
#define REG_ADR_RXM1SIDH                ( (uint8_t)0x24U )
#define REG_ADR_RXM1SIDL                ( (uint8_t)0x25U )
#define REG_ADR_RXM1EID8                ( (uint8_t)0x26U )
#define REG_ADR_RXM1EID0                ( (uint8_t)0x27U )
#define REG_ADR_CNF3                    ( (uint8_t)0x28U )
#define REG_ADR_CNF2                    ( (uint8_t)0x29U )
#define REG_ADR_CNF1                    ( (uint8_t)0x2AU )
#define REG_ADR_CANINTE                 ( (uint8_t)0x2BU )
#define REG_ADR_CANINTF                 ( (uint8_t)0x2CU )
#define REG_ADR_EFLG                    ( (uint8_t)0x2DU )
#define REG_ADR_TXB0CTRL                ( (uint8_t)0x30U )
#define REG_ADR_TXB0SIDH                ( (uint8_t)0x31U )
#define REG_ADR_TXB0SIDL                ( (uint8_t)0x32U )
#define REG_ADR_TXB0EID8                ( (uint8_t)0x33U )
#define REG_ADR_TXB0EID0                ( (uint8_t)0x34U )
#define REG_ADR_TXB0DLC                 ( (uint8_t)0x35U )
#define REG_ADR_TXB0D0                  ( (uint8_t)0x36U )
#define REG_ADR_TXB0D1                  ( (uint8_t)0x37U )
#define REG_ADR_TXB0D2                  ( (uint8_t)0x38U )
#define REG_ADR_TXB0D3                  ( (uint8_t)0x39U )
#define REG_ADR_TXB0D4                  ( (uint8_t)0x3AU )
#define REG_ADR_TXB0D5                  ( (uint8_t)0x3BU )
#define REG_ADR_TXB0D6                  ( (uint8_t)0x3CU )
#define REG_ADR_TXB0D7                  ( (uint8_t)0x3DU )
#define REG_ADR_TXB1CTRL                ( (uint8_t)0x40U )
#define REG_ADR_TXB1SIDH                ( (uint8_t)0x41U )
#define REG_ADR_TXB1SIDL                ( (uint8_t)0x42U )
#define REG_ADR_TXB1EID8                ( (uint8_t)0x43U )
#define REG_ADR_TXB1EID0                ( (uint8_t)0x44U )
#define REG_ADR_TXB1DLC                 ( (uint8_t)0x45U )
#define REG_ADR_TXB1D0                  ( (uint8_t)0x46U )
#define REG_ADR_TXB1D1                  ( (uint8_t)0x47U )
#define REG_ADR_TXB1D2                  ( (uint8_t)0x48U )
#define REG_ADR_TXB1D3                  ( (uint8_t)0x49U )
#define REG_ADR_TXB1D4                  ( (uint8_t)0x4AU )
#define REG_ADR_TXB1D5                  ( (uint8_t)0x4BU )
#define REG_ADR_TXB1D6                  ( (uint8_t)0x4CU )
#define REG_ADR_TXB1D7                  ( (uint8_t)0x4DU )
#define REG_ADR_TXB2CTRL                ( (uint8_t)0x50U )
#define REG_ADR_TXB2SIDH                ( (uint8_t)0x51U )
#define REG_ADR_TXB2SIDL                ( (uint8_t)0x52U )
#define REG_ADR_TXB2EID8                ( (uint8_t)0x53U )
#define REG_ADR_TXB2EID0                ( (uint8_t)0x54U )
#define REG_ADR_TXB2DLC                 ( (uint8_t)0x55U )
#define REG_ADR_TXB2D0                  ( (uint8_t)0x56U )
#define REG_ADR_TXB2D1                  ( (uint8_t)0x57U )
#define REG_ADR_TXB2D2                  ( (uint8_t)0x58U )
#define REG_ADR_TXB2D3                  ( (uint8_t)0x59U )
#define REG_ADR_TXB2D4                  ( (uint8_t)0x5AU )
#define REG_ADR_TXB2D5                  ( (uint8_t)0x5BU )
#define REG_ADR_TXB2D6                  ( (uint8_t)0x5CU )
#define REG_ADR_TXB2D7                  ( (uint8_t)0x5DU )
#define REG_ADR_RXB0CTRL                ( (uint8_t)0x60U )
#define REG_ADR_RXB0SIDH                ( (uint8_t)0x61U )
#define REG_ADR_RXB0SIDL                ( (uint8_t)0x62U )
#define REG_ADR_RXB0EID8                ( (uint8_t)0x63U )
#define REG_ADR_RXB0EID0                ( (uint8_t)0x64U )
#define REG_ADR_RXB0DLC                 ( (uint8_t)0x65U )
#define REG_ADR_RXB0D0                  ( (uint8_t)0x66U )
#define REG_ADR_RXB0D1                  ( (uint8_t)0x67U )
#define REG_ADR_RXB0D2                  ( (uint8_t)0x68U )
#define REG_ADR_RXB0D3                  ( (uint8_t)0x69U )
#define REG_ADR_RXB0D4                  ( (uint8_t)0x6AU )
#define REG_ADR_RXB0D5                  ( (uint8_t)0x6BU )
#define REG_ADR_RXB0D6                  ( (uint8_t)0x6CU )
#define REG_ADR_RXB0D7                  ( (uint8_t)0x6DU )
#define REG_ADR_RXB1CTRL                ( (uint8_t)0x70U )
#define REG_ADR_RXB1SIDH                ( (uint8_t)0x71U )
#define REG_ADR_RXB1SIDL                ( (uint8_t)0x72U )
#define REG_ADR_RXB1EID8                ( (uint8_t)0x73U )
#define REG_ADR_RXB1EID0                ( (uint8_t)0x74U )
#define REG_ADR_RXB1DLC                 ( (uint8_t)0x75U )
#define REG_ADR_RXB1D0                  ( (uint8_t)0x76U )
#define REG_ADR_RXB1D1                  ( (uint8_t)0x77U )
#define REG_ADR_RXB1D2                  ( (uint8_t)0x78U )
#define REG_ADR_RXB1D3                  ( (uint8_t)0x79U )
#define REG_ADR_RXB1D4                  ( (uint8_t)0x7AU )
#define REG_ADR_RXB1D5                  ( (uint8_t)0x7BU )
#define REG_ADR_RXB1D6                  ( (uint8_t)0x7CU )
#define REG_ADR_RXB1D7                  ( (uint8_t)0x7DU )

/* Register value */
#define REG_VAL_00                      ( (uint8_t)0x00U )
#define REG_VAL_FF                      ( (uint8_t)0xFFU )

/* Register SIDL_IDE value */
#define REG_VAL_CAN_KIND_STD            (uint8_t)( REG_VAL_00 & REG_MASK_SIDL_IDE )
#define REG_VAL_CAN_KIND_EXT            (uint8_t)( REG_VAL_FF & REG_MASK_SIDL_IDE )

/* Register SIDL_SRR value */
#define REG_VAL_STD_CAN_DATA            (uint8_t)( REG_VAL_00 & REG_MASK_SIDL_SRR )
#define REG_VAL_STD_CAN_RMT             (uint8_t)( REG_VAL_FF & REG_MASK_SIDL_SRR )

/* Register DLC_RTR value */
#define REG_VAL_EXT_CAN_DATA            (uint8_t)( REG_VAL_00 & REG_MASK_DLC_RTR )
#define REG_VAL_EXT_CAN_RMT             (uint8_t)( REG_VAL_FF & REG_MASK_DLC_RTR )

/* Mask of the register value */
#define REG_MASK_OPMOD                  ( (uint8_t)0xE0U )
#define REG_MASK_CANSTAT_ICOD           ( (uint8_t)0x0EU )
#define REG_MASK_CANCTRL_ABAT           ( (uint8_t)0x10U )
#define REG_MASK_CANCTRL_OSM            ( (uint8_t)0x08U )
#define REG_MASK_CANINT_MERRF           ( (uint8_t)0x80U )
#define REG_MASK_CANINT_WAKIF           ( (uint8_t)0x40U )
#define REG_MASK_CANINT_ERRIF           ( (uint8_t)0x20U )
#define REG_MASK_CANINT_TX2IF           ( (uint8_t)0x10U )
#define REG_MASK_CANINT_TX1IF           ( (uint8_t)0x08U )
#define REG_MASK_CANINT_TX0IF           ( (uint8_t)0x04U )
#define REG_MASK_CANINT_RX1IF           ( (uint8_t)0x02U )
#define REG_MASK_CANINT_RX0IF           ( (uint8_t)0x01U )
#define REG_MASK_EFLG_RX1OVR            ( (uint8_t)0x80U )
#define REG_MASK_EFLG_RX0OVR            ( (uint8_t)0x40U )
#define REG_MASK_EFLG_TXBO              ( (uint8_t)0x20U )
#define REG_MASK_EFLG_TXEP              ( (uint8_t)0x10U )
#define REG_MASK_EFLG_RXEP              ( (uint8_t)0x08U )
#define REG_MASK_EFLG_TXWAR             ( (uint8_t)0x04U )
#define REG_MASK_EFLG_RXWAR             ( (uint8_t)0x02U )
#define REG_MASK_EFLG_EWARN             ( (uint8_t)0x01U )
#define REG_MASK_TXBCTRL_ABTF           ( (uint8_t)0x40U )
#define REG_MASK_TXBCTRL_MLOA           ( (uint8_t)0x20U )
#define REG_MASK_TXBCTRL_TXERR          ( (uint8_t)0x10U )
#define REG_MASK_TXBCTRL_TXREQ          ( (uint8_t)0x08U )
#define REG_MASK_TXBCTRL_TXP            ( (uint8_t)0x03U )
#define REG_MASK_RXBCTRL_RXM            ( (uint8_t)0x60U )
#define REG_MASK_RXBCTRL_RXRTR          ( (uint8_t)0x08U )
#define REG_MASK_RXB0CTRL_BUKT          ( (uint8_t)0x04U )
#define REG_MASK_RXB0CTRL_FILHIT        ( (uint8_t)0x01U )
#define REG_MASK_RXB1CTRL_FILHIT        ( (uint8_t)0x07U )
#define REG_MASK_SIDL_EID               ( (uint8_t)0x03U )
#define REG_MASK_SIDL_IDE               ( (uint8_t)0x08U )
#define REG_MASK_SIDL_SRR               ( (uint8_t)0x10U )
#define REG_MASK_SIDL_SID               ( (uint8_t)0xE0U )
#define REG_MASK_DLC_RTR                ( (uint8_t)0x40U )
#define REG_MASK_DLC_DLC                ( (uint8_t)0x0FU )

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
void mcp2515_begin_spi( void );
void mcp2515_end_spi( void );

void mcp2515_write_spi_array( const size_t len, const uint8_t const *p_buff );
void mcp2515_write_spi( const uint8_t val );
void mcp2515_read_spi_array( const size_t len, uint8_t *p_buff );
uint8_t mcp2515_read_spi( void );

void mcp2515_write_reg_array( const uint8_t addr, const size_t len, const uint8_t const *p_buff );
void mcp2515_write_reg( const uint8_t addr, const uint8_t val );
void mcp2515_modify_reg( const uint8_t addr, const uint8_t mask, const uint8_t val );
void mcp2515_read_reg_array( const uint8_t addr, const size_t len, uint8_t *p_buff );
uint8_t mcp2515_read_reg( const uint8_t addr );

void mcp2515_read_rx1( const size_t len, uint8_t *p_buff );
void mcp2515_read_rx2( const size_t len, uint8_t *p_buff );

#endif /* MCP2515_REGISTER_H */
