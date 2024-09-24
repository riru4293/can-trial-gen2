#ifndef MCP2515_SPI_CMD_H
#define MCP2515_SPI_CMD_H

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
/* SPI command */
#define SPI_CMD_INVALID                 ( (uint8_t)0xFFU )
#define SPI_CMD_WRITE_REG               ( (uint8_t)0x02U )
#define SPI_CMD_READ_REG                ( (uint8_t)0x03U )
#define SPI_CMD_MODIFY_REG              ( (uint8_t)0x05U )
#define SPI_CMD_WRITE_TX_1              ( (uint8_t)0x40U )
#define SPI_CMD_WRITE_TX_2              ( (uint8_t)0x42U )
#define SPI_CMD_WRITE_TX_3              ( (uint8_t)0x44U )
#define SPI_CMD_REQ_TX_1                ( (uint8_t)0x81U )
#define SPI_CMD_REQ_TX_2                ( (uint8_t)0x82U )
#define SPI_CMD_REQ_TX_3                ( (uint8_t)0x83U )
#define SPI_CMD_READ_RX_1               ( (uint8_t)0x90U )
#define SPI_CMD_READ_RX_2               ( (uint8_t)0x94U )
#define SPI_CMD_READ_STAT               ( (uint8_t)0xA0U )
#define SPI_CMD_READ_RX_STAT            ( (uint8_t)0xB0U )
#define SPI_CMD_RESET                   ( (uint8_t)0xC0U )

#endif /* MCP2515_SPI_CMD_H */
