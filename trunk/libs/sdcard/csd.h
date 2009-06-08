#ifndef __sdcard_csd_h__
#define __sdcard_csd_h__

#include "libs/sdcard/sdcard.h"

Error sd_read_csd(SDCardIndex card, uint8 *csd);

/*
 * I don't remember where these #defines came from.  If you know, please let me know so I can give
 * credit where it's due.  Or I could just rewrite them...
 */
#define CSD_STRUCTURE(b)	((b[ 0] >> 6) & 0x03) //2     00b           R      [127:126]
#define TAAC(b)			((b[ 1] >> 0) & 0xff) //8     xxh           R      [119:112]
#define NSAC(b)			((b[ 2] >> 0) & 0xff) //8     xxh           R      [111:104]
#define TRAN_SPEED(b)		((b[ 3] >> 0) & 0xff) //8     32h or 5Ah    R      [103:96]
#define CCC(b)			((b[ 0] >> 0) & 0x00) //12    01x110110101b R      [95:84]
#define READ_BL_LEN(b)		((b[ 5] >> 0) & 0x0f) //4     xh            R      [83:80]
#define READ_BL_PARTIAL(b)	((b[ 6] >> 7) & 0x01) //1     1b            R      [79:79]
#define WRITE_BLK_MISALIGN(b)	((b[ 6] >> 6) & 0x01) //1     xb            R      [78:78]
#define READ_BLK_MISALIGN(b)	((b[ 6] >> 5) & 0x01) //1     xb            R      [77:77]
#define DSR_IMP(b)		((b[ 6] >> 4) & 0x01) //1     xb            R      [76:76]
#define C_SIZE(b)		((b[ 0] >> 0) & 0x00) //12    xxxh          R      [73:62]
#define VDD_R_CURR_MIN(b)	((b[ 8] >> 3) & 0x07) //3     xxxb          R      [61:59]
#define VDD_R_CURR_MAX(b)	((b[ 8] >> 0) & 0x07) //3     xxxb          R      [58:56]
#define VDD_W_CURR_MIN(b)	((b[ 9] >> 5) & 0x07) //3     xxxb          R      [55:53]
#define VDD_W_CURR_MAX(b)	((b[ 9] >> 2) & 0x07) //3     xxxb          R      [52:50]
#define C_SIZE_MULT(b)		((b[ 0] >> 0) & 0x00) //3     xxxb          R      [49:47]
#define ERASE_BLK_EN(b)		((b[10] >> 6) & 0x01) //1     xb            R      [46:46]
#define SECTOR_SIZE(b)		((b[ 0] >> 0) & 0x00) //7     xxxxxxxb      R      [45:39]
#define WP_GRP_SIZE(b)		((b[11] >> 0) & 0x7f) //7     xxxxxxxb      R      [38:32]
#define WP_GRP_ENABLE(b)	((b[12] >> 7) & 0x01) //1     xb            R      [31:31]
#define R2W_FACTOR(b)		((b[12] >> 2) & 0x07) //3     xxxb          R      [28:26]
#define WRITE_BL_LEN(b)		((b[ 0] >> 0) & 0x00) //4     xxxxb         R      [25:22]
#define WRITE_BL_PARTIAL(b)	((b[13] >> 5) & 0x01) //1     xb            R      [21:21]
#define FILE_FORMAT_GRP(b)	((b[14] >> 7) & 0x01) //1     xb            R/W(1) [15:15]
#define COPY(b)			((b[14] >> 6) & 0x01) //1     xb            R/W(1) [14:14]
#define PERM_WRITE_PROTECT(b)	((b[14] >> 5) & 0x01) //1     xb            R/W(1) [13:13]
#define TMP_WRITE_PROTECT(b)	((b[14] >> 4) & 0x01) //1     xb            R/W    [12:12]
#define FILE_FORMAT(b)		((b[14] >> 2) & 0x03) //2     xxb           R/W(1) [11:10]
#define CRC(b)			((b[15] >> 1) & 0x7f) //7     xxxxxxxb      R/W    [7:1]

#endif //__sdcard_csd_h__
