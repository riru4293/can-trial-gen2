#ifndef MCP2515_SPICMD_H
#define MCP2515_SPICMD_H

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

#endif /* MCP2515_SPICMD_H */
