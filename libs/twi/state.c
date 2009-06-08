#include "libs/twi/state.h"

const uint8	state_response_table[32] PROGMEM =
{
    //TWI_BUS_ERROR              = 0x00,
    // Bus error due to an illegal START or STOP condition
    0x00,

    //TWI_START                  = 0x08,
    // START has been transmitted
    TWI_RESET_CURRENT, // | TWI_WRITE_BYTE,

    //TWI_REP_START              = 0x10,
    // Repeated START has been transmitted
    TWI_RESET_CURRENT, // | TWI_WRITE_BYTE,

    //TWI_MTX_ADR_ACK            = 0x18,
    // SLA+W has been tramsmitted and ACK received
    TWI_WRITE_BYTE,

    //TWI_MTX_ADR_NACK           = 0x20,
    // SLA+W has been tramsmitted and NACK received
    0x00,

    //TWI_MTX_DATA_ACK           = 0x28,
    // Data byte has been tramsmitted and ACK received
    TWI_WRITE_BYTE,

    //TWI_MTX_DATA_NACK          = 0x30,
    // Data byte has been tramsmitted and NACK received
    0x00,

    //TWI_ARB_LOST               = 0x38,
    // Arbitration lost
    _BV(TWIE) | _BV(TWSTA),

    //TWI_MRX_ADR_ACK            = 0x40,
    // SLA+R has been tramsmitted and ACK received
    _BV(TWIE) | TWI_ACK_BYTE,

    //TWI_MRX_ADR_NACK           = 0x48,
    // SLA+R has been tramsmitted and NACK received
    0x00,

    //TWI_MRX_DATA_ACK           = 0x50,
    // Data byte has been received and ACK tramsmitted
    _BV(TWIE) | TWI_READ_BYTE | TWI_ACK_BYTE,

    //TWI_MRX_DATA_NACK          = 0x58,
    // Data byte has been received and NACK tramsmitted
    _BV(TWSTO) | TWI_READ_BYTE,

    //TWI_SRX_ADR_ACK            = 0x60,
    // Own SLA+W has been received ACK has been returned
    _BV(TWIE) | _BV(TWEA),

    //TWI_SRX_ADR_ACK_M_ARB_LOST = 0x68,
    // Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
    0x00,

    //TWI_SRX_GEN_ACK            = 0x70,
    // General call address has been received; ACK has been returned
    _BV(TWIE) | _BV(TWEA),

    //TWI_SRX_GEN_ACK_M_ARB_LOST = 0x78,
    // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
    0x00,

    //TWI_SRX_ADR_DATA_ACK       = 0x80,
    // Previously addressed with own SLA+W; data has been received; ACK has been returned
    _BV(TWIE) | _BV(TWEA) | TWI_READ_BYTE,

    //TWI_SRX_ADR_DATA_NACK      = 0x88,
    // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
    0x00,

    //TWI_SRX_GEN_DATA_ACK       = 0x90,
    // Previously addressed with general call; data has been received; ACK has been returned
    _BV(TWIE) | _BV(TWEA) | TWI_READ_BYTE,

    //TWI_SRX_GEN_DATA_NACK      = 0x98,
    // Previously addressed with general call; data has been received; NOT ACK has been returned
    0x00,

    //TWI_SRX_STOP_RESTART       = 0xA0,
    // A STOP condition or repeated START condition has been received while still addressed as Slave
    0x00,

    //TWI_STX_ADR_ACK            = 0xA8,
    // Own SLA+R has been received; ACK has been returned
    _BV(TWIE) | _BV(TWEA) | TWI_RESET_CURRENT | TWI_WRITE_BYTE,

    //TWI_STX_ADR_ACK_M_ARB_LOST = 0xB0,
    // Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
    0x00,

    //TWI_STX_DATA_ACK           = 0xB8,
    // Data byte in TWDR has been transmitted; ACK has been received
    _BV(TWIE) | _BV(TWEA) | TWI_WRITE_BYTE,

    //TWI_STX_DATA_NACK          = 0xC0,
    // Data byte in TWDR has been transmitted; NOT ACK has been received
    0x00,

    //TWI_STX_DATA_ACK_LAST_BYTE = 0xC8,
    // Last data byte in TWDR has been transmitted (TWEA = 0); ACK has been received
    0x00,

    //NOTHING                    = 0xD0,
    0x00,

    //NOTHING                    = 0xD8,
    0x00,

    //NOTHING                    = 0xE0,
    0x00,

    //NOTHING                    = 0xE8,
    0x00,

    //NOTHING                    = 0xF0,
    0x00,

    //TWI_NO_STATE               = 0xF8,
    // No relevant state information available; TWINT = 0
    0x00
};
