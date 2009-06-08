/*
 * Copyright 2009 Anton Staaf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef DEBUG_SDCARD
#define DEBUG
#endif

#include "libs/stdio/debug.h"
#include "libs/stdio/console.h"
#include "libs/error/error.h"
#include "libs/block/block.h"
#include "libs/partition/partition.h"
#include "libs/sdcard/sdcard.h"
#include "libs/sdcard/csd.h"
#include "libs/spi/spi.h"
#include "libs/os/os.h"

SetupError();

typedef enum
{
    response_r1  = 0,
    response_r1b = 1,
    response_r2  = 2,
    response_r3  = 3,
    response_r7  = 4,
    response_last
} ResponseType;

typedef struct
{
    uint8		length;
    bool		busy_wait;
} ResponseData;

static const ResponseData	response_table[response_last] PROGMEM =
{
    {1, false},
    {1, true},
    {2, false},
    {5, false},
    {5, false}
};

typedef struct 
{
    uint8		command;
    ResponseType	response_type;
    bool		disable_spi;
} CommandData;

static const CommandData	command_table[cmd_last] PROGMEM =
{
    {0x40 |  0, response_r1, true},	//GO_IDLE_STATE
    {0x40 |  1, response_r1, true},	//SEND_OP_COND
    {0x40 |  8, response_r7, true},	//SEND_IF_COND
    {0x40 |  9, response_r1, false},	//SEND_CSD
    {0x40 | 10, response_r1, false},	//SEND_CID
    {0x40 | 13, response_r2, true},	//SEND_STATUS
    {0x40 | 16, response_r1, true},	//SET_BLOCKLEN
    {0x40 | 17, response_r1, false},	//READ_SINGLE_BLOCK
    {0x40 | 24, response_r1, false},	//WRITE_BLOCK
    {0x40 | 55, response_r1, true},	//APP_CMD
    {0x40 | 58, response_r3, true},	//READ_OCR
    {0x40 | 41, response_r1, true},	//SD_SEND_OP_COND
    {0x40 | 42, response_r1, true}	//SET_CLR_CARD_DETECT
};

#define	ACMD41_HCS	(((uint32) 1) << 30)

#define OCR_CCS		(((uint32) 1) << 30)

typedef enum
{
    version_unknown,
    version_mmc,
    version_1xx,
    version_200
} SDCardVersion;

typedef struct
{
    bool		initialized;
    bool		compatible;
    bool		high_capacity;
    bool		idle;
    SDCardVersion	version;
    uint8		response_timeout;
    uint8		read_timeout;
    uint8		write_timeout;
} SDCard;

typedef struct
{
    SDCardIndex		index;
} SDCardBlock;

static const SDCard	sdcard_initializer PROGMEM =
{
    false,
    false,
    false,
    true,
    version_unknown,
    SDCARD_RESPONSE_TIMEOUT,
    SDCARD_READ_TIMEOUT,
    SDCARD_WRITE_TIMEOUT
};

static SDCard		_sdcard[SDCARD_MAX_CARDS];
static SDCardBlock	_sdcard_block[SDCARD_MAX_CARDS];

/*********************************************************************************************************************/
static void sd_cycle(uint8 bytes)
{
    for(uint8 i = 0; i < bytes; i++)
    	spi_rw(0xff);
}
/*********************************************************************************************************************
 * Check that the correct bits in the OCR are set.  These bits correspond to the voltage range 2.7 - 3.0 volts.
 */
static bool sd_check_voltage(uint8 *response)
{
    return (((response[1] & 0x80)) &&
	    ((response[2] & 0x03) == 0x03));
}
/*********************************************************************************************************************/
static void sd_enable(SDCardIndex card)
{
    spi_enable();
}
/*********************************************************************************************************************/
static void sd_disable(SDCardIndex card)
{
    spi_rw(0xff);
    spi_disable();
    spi_rw(0xff);

    /*
     * The Crucial 2GB MicroSD cards need a second bytes worth of clocks after the chip select
     * is deasserted.
     */
    spi_rw(0xff);
}
/*********************************************************************************************************************/
Error sd_command(SDCardIndex card, CommandIndex index, uint32 argument, uint8* response)
{
    debug4("arg", argument);

    Error		check_error      = success;
    uint8		command          = pgm_read_byte(&(command_table[index].command));
    ResponseType	response_type    = pgm_read_byte(&(command_table[index].response_type));
    bool		disable_spi      = pgm_read_byte(&(command_table[index].disable_spi));
    uint8		response_length  = pgm_read_byte(&(response_table[response_type].length));
    bool		busy_wait        = pgm_read_byte(&(response_table[response_type].busy_wait));
    uint8		response_timeout = _sdcard[card].response_timeout;
    uint8		byte             = 0xff;
    uint8		crc;
    uint8		i;

    switch (index)
    {
    case cmd_0: crc = 0x95; break;
    case cmd_8: crc = 0x87; break;
    default:    crc = 0x01; break;
    }

    sd_enable(card);

    debug1("\r\nCommand ", command);
    debug1("CRC     ", crc);

    spi_rw(command);
    spi_rw((argument >> 24) & 0xff);
    spi_rw((argument >> 16) & 0xff);
    spi_rw((argument >>  8) & 0xff);
    spi_rw((argument >>  0) & 0xff);
    spi_rw(crc);

    for (i = 0; (i < response_timeout) && (byte & 0x80); ++i)
	byte = spi_rw(0xff);

    debug1("Count   ", i);
    debug1("Response", byte);

    CheckCleanupB((byte & 0x80) == 0x00, failure);

    response[response_length - 1] = byte;

    for (int8 j = (response_length - 2); j >= 0; j--)
        response[j] = spi_rw(0xff);

    if (busy_wait)
    {
	byte = 0;

	for (i = 0; (i < response_timeout) && (byte == 0); ++i)
	    byte = spi_rw(0xff);
    }

    if (disable_spi)
    {
	/*
	 * SD card always requires an extra 8 clock cycles.
	 */
	spi_rw(0xff);

	sd_disable(card);
    }

    return success;

 failure:
    sd_disable(card);
    return check_error;
}
/*********************************************************************************************************************
 * Put the SD card into SPI mode.
 */
Error sd_init(SDCardIndex card)
{
    uint8	response[5];

    CheckB(card < SDCARD_MAX_CARDS);
    CheckB(_sdcard[card].initialized == false);

    memcpy_P(&(_sdcard[card]), &sdcard_initializer, sizeof(SDCard));

    sd_enable(card);
    sd_cycle(100);
    sd_disable(card);

    /*
     * FIX FIX FIX: Why do we output two extra bytes worth of clock cycles.
     */
    sd_cycle(2);

    /*
     * Place the SDCard into SPI transfer mode with CMD0.
     */
    Check(sd_command(card, cmd_0, 0, response));
    Check(sd_command(card, cmd_8, 0x000001aa, response));

    /*
     * If CMD8 is invalid for this card then perhaps it is an older SD card.
     */
    _sdcard[card].version = (response[4] & 0x04) ? version_1xx : version_200;

    if (_sdcard[card].version >= version_200)
	debugx("R7", response, 4);

    Check(sd_command(card, cmd_58, 0, response));

    debugx("OCR", response, 4);

    _sdcard[card].compatible = sd_check_voltage(response);

    CheckB(_sdcard[card].compatible);

    Check(sd_command(card, cmd_55,  0, response));
    Check(sd_command(card, acmd_41, ACMD41_HCS, response));

    if (response[0] & 0x04)
	_sdcard[card].version = version_mmc;

    for (uint8 i = 0; i < _sdcard[card].response_timeout; ++i)
    {
	if (_sdcard[card].version == version_mmc)
	{
	    Check(sd_command(card, cmd_1, 0, response));
	}
	else
	{
	    Check(sd_command(card, cmd_55,  0, response));
	    Check(sd_command(card, acmd_41, 0, response));
	}

	if ((response[0] & 0x01) == 0x00)
	{
	    _sdcard[card].idle = false;
	    break;
	}

	os_sleep_ms(10);
    }

    CheckB(_sdcard[card].idle == false);

    /*
     * Get the cards OCR again.  This is used to get the cards Card Capacity State (CCS).
     */
    Check(sd_command(card, cmd_58, 0, response));

    debugx("OCR", response, 4);

    if ((response[3] & 0xc0) == 0xc0)
	_sdcard[card].high_capacity = true;

    /*
     * Disable the 10-90KOhm Card Detect pull-up resistor.
     */
    Check(sd_command(card, cmd_55,  0, response));
    Check(sd_command(card, acmd_42, 0, response));

    _sdcard[card].initialized = true;
    _sdcard_block[card].index = card;

    return success;
}
/*********************************************************************************************************************/
Error sd_shutdown(SDCardIndex card)
{
    CheckB(card < SDCARD_MAX_CARDS);

    _sdcard[card].initialized = false;

    return success;
}
/*********************************************************************************************************************/
Error sd_read_base(SDCardIndex card, uint8 *data, uint16 count)
{
    Error	check_error = success;
    uint8	byte        = 0xff;

    for (uint8 i = 0; i < _sdcard[card].read_timeout; ++i)
    {
	byte = spi_rw(0xff);

	debug1("byte", byte);

	if (byte == 0xfe)
	    break;

	os_sleep_ms(1);
    }

    CheckCleanupB(byte == 0xfe, failure);

    for (uint16 j = 0; j < count; ++j)
	data[j] = spi_rw(0xff);

    /*
     * Read the 16bit checksum from the card.
     */
    spi_rw(0xff);
    spi_rw(0xff);

 failure:
    sd_disable(card);
    return check_error;
}
/*********************************************************************************************************************/
static Error sd_block_read(void *data, uint32 address, uint8 *buffer)
{
    SDCardBlock	*sdcard_block = (SDCardBlock *)data;
    SDCardIndex	card          = sdcard_block->index;
    uint8	response;

    if (_sdcard[card].high_capacity == false)
	address *= BYTES_PER_BLOCK;

    CheckB(card < SDCARD_MAX_CARDS);
    CheckB(_sdcard[card].initialized);
    Check(sd_command(card, cmd_17, address, &response));
    Check(sd_read_base(card, buffer, BYTES_PER_BLOCK));

    return success;
}
/*********************************************************************************************************************/
static Error sd_block_write(void *data, uint32 address, uint8 *buffer)
{
    Error	check_error   = success;
    SDCardBlock	*sdcard_block = (SDCardBlock *)data;
    SDCardIndex	card          = sdcard_block->index;
    uint8	response;
    uint8	byte;

    if (_sdcard[card].high_capacity == false)
	address *= BYTES_PER_BLOCK;

    CheckB(card < SDCARD_MAX_CARDS);
    CheckB(_sdcard[card].initialized);
    Check(sd_command(card, cmd_24, address, &response));

    spi_rw(0xfe);

    for (uint16 i = 0; i < 512; i++)
    	spi_rw(buffer[i]);

    /*
     * Write an invalid checksum, the SDCard will ignore it anyway.
     */
    spi_rw(0xff);
    spi_rw(0xff);

    /*
     * Read the Data Response.  Only the bottom five bits are defined.  A value of 0x05 indicates success.
     */
    byte = spi_rw(0xff);

    CheckCleanupB((byte & 0x1f) == 0x05, read_failure);

    /*
     * Wait for the SDCard to finish the write operation.  This could be done later, but we would first have to send
     * at least one byte of 0xff to let the card start its internal write operation.
     */
    for (uint8 i = 0; i < _sdcard[card].write_timeout; ++i)
    {
	if ((byte = spi_rw(0xff)) == 0xff)
	    break;

	os_sleep_ms(1);
    }

    CheckCleanupB(byte == 0xff, timeout_failure);

    sd_disable(card);
    return success;

  timeout_failure:
    debug1("WR2", byte);
    goto failure;

  read_failure:
    debug1("WR1", byte);

  failure:
    sd_disable(card);
    return check_error;
}
/*********************************************************************************************************************/
static BlockOps	sd_block_ops =
{
    sd_block_read,
    sd_block_write
};
/*********************************************************************************************************************/
static Error sdcard_init()
{
    BlockIndex	index = 0xff;

    Check(sd_init(0));
    Check(block_allocate(&index, &sd_block_ops, &(_sdcard_block[0])));
    Check(partition_block_device(index));

    return success;
}
/*********************************************************************************************************************/
const BootModule boot_module_sdcard PROGMEM =
{
    sdcard_init
};
/*********************************************************************************************************************/
