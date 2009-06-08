#ifndef __twi_state_h__
#define __twi_state_h__

#include <avr/pgmspace.h>

#include "libs/types/types.h"

typedef enum
{
    // Master General
    TWI_START                  = 0x08, // START has been transmitted  
    TWI_REP_START              = 0x10, // Repeated START has been transmitted
    TWI_ARB_LOST               = 0x38, // Arbitration lost

    // Master Transmitter
    TWI_MTX_ADR_ACK            = 0x18, // SLA+W has been tramsmitted and ACK received
    TWI_MTX_ADR_NACK           = 0x20, // SLA+W has been tramsmitted and NACK received
    TWI_MTX_DATA_ACK           = 0x28, // Data byte has been tramsmitted and ACK received
    TWI_MTX_DATA_NACK          = 0x30, // Data byte has been tramsmitted and NACK received

    // Master Receiver
    TWI_MRX_ADR_ACK            = 0x40, // SLA+R has been tramsmitted and ACK received
    TWI_MRX_ADR_NACK           = 0x48, // SLA+R has been tramsmitted and NACK received
    TWI_MRX_DATA_ACK           = 0x50, // Data byte has been received and ACK tramsmitted
    TWI_MRX_DATA_NACK          = 0x58, // Data byte has been received and NACK tramsmitted

    // Slave Transmitter
    TWI_STX_ADR_ACK            = 0xA8, // Own SLA+R has been received; ACK has been returned
    TWI_STX_ADR_ACK_M_ARB_LOST = 0xB0, // Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
    TWI_STX_DATA_ACK           = 0xB8, // Data byte in TWDR has been transmitted; ACK has been received
    TWI_STX_DATA_NACK          = 0xC0, // Data byte in TWDR has been transmitted; NOT ACK has been received
    TWI_STX_DATA_ACK_LAST_BYTE = 0xC8, // Last data byte in TWDR has been transmitted (TWEA = 0); ACK has been received

    // Slave Receiver
    TWI_SRX_ADR_ACK            = 0x60, // Own SLA+W has been received ACK has been returned
    TWI_SRX_ADR_ACK_M_ARB_LOST = 0x68, // Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
    TWI_SRX_GEN_ACK            = 0x70, // General call address has been received; ACK has been returned
    TWI_SRX_GEN_ACK_M_ARB_LOST = 0x78, // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
    TWI_SRX_ADR_DATA_ACK       = 0x80, // Previously addressed with own SLA+W; data has been received; ACK has been returned
    TWI_SRX_ADR_DATA_NACK      = 0x88, // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
    TWI_SRX_GEN_DATA_ACK       = 0x90, // Previously addressed with general call; data has been received; ACK has been returned
    TWI_SRX_GEN_DATA_NACK      = 0x98, // Previously addressed with general call; data has been received; NOT ACK has been returned
    TWI_SRX_STOP_RESTART       = 0xA0, // A STOP condition or repeated START condition has been received while still addressed as Slave

    // Miscellaneous
    TWI_NO_STATE               = 0xF8, // No relevant state information available; TWINT = 0
    TWI_BUS_ERROR              = 0x00, // Bus error due to an illegal START or STOP condition
} TWIState;

extern const uint8	state_response_table[32] PROGMEM;

#define RESPONSE_MASK		(_BV(TWIE) | _BV(TWSTA) | _BV(TWEA) | _BV(TWSTO))
#define	TWI_RESET_CURRENT	0x02
#define TWI_READ_BYTE		0x04
#define TWI_WRITE_BYTE		0x08
#define TWI_ACK_BYTE		0x80

#if (TWI_RESET_CURRENT & RESPONSE_MASK)
    #error "Invalid TWI_RESET_CURRENT value conflicts with Control Register."
#endif

#if (TWI_READ_BYTE & RESPONSE_MASK)
    #error "Invalid TWI_READ_BYTE value conflicts with Control Register."
#endif

#if (TWI_WRITE_BYTE & RESPONSE_MASK)
    #error "Invalid TWI_WRITE_BYTE value conflicts with Control Register."
#endif

#endif //__twi_state_h__
