#ifndef __PN532_UART_H
#define __PN532_UART_H

#include "uart.h"

#define DEFAULT_UART_SPEED    115200  

// Register addresses
#define PN53X_REG_Control_switch_rng 0x6106
#define PN53X_REG_CIU_Mode 0x6301
#define PN53X_REG_CIU_TxMode 0x6302
#define PN53X_REG_CIU_RxMode 0x6303
#define PN53X_REG_CIU_TxControl 0x6304
#define PN53X_REG_CIU_TxAuto 0x6305
#define PN53X_REG_CIU_TxSel 0x6306
#define PN53X_REG_CIU_RxSel 0x6307
#define PN53X_REG_CIU_RxThreshold 0x6308
#define PN53X_REG_CIU_Demod 0x6309
#define PN53X_REG_CIU_FelNFC1 0x630A
#define PN53X_REG_CIU_FelNFC2 0x630B
#define PN53X_REG_CIU_MifNFC 0x630C
#define PN53X_REG_CIU_ManualRCV 0x630D
#define PN53X_REG_CIU_TypeB 0x630E
// #define PN53X_REG_- 0x630F
// #define PN53X_REG_- 0x6310
#define PN53X_REG_CIU_CRCResultMSB 0x6311
#define PN53X_REG_CIU_CRCResultLSB 0x6312
#define PN53X_REG_CIU_GsNOFF 0x6313
#define PN53X_REG_CIU_ModWidth 0x6314
#define PN53X_REG_CIU_TxBitPhase 0x6315
#define PN53X_REG_CIU_RFCfg 0x6316
#define PN53X_REG_CIU_GsNOn 0x6317
#define PN53X_REG_CIU_CWGsP 0x6318
#define PN53X_REG_CIU_ModGsP 0x6319
#define PN53X_REG_CIU_TMode 0x631A
#define PN53X_REG_CIU_TPrescaler 0x631B
#define PN53X_REG_CIU_TReloadVal_hi 0x631C
#define PN53X_REG_CIU_TReloadVal_lo 0x631D
#define PN53X_REG_CIU_TCounterVal_hi 0x631E
#define PN53X_REG_CIU_TCounterVal_lo 0x631F
// #define PN53X_REG_- 0x6320
#define PN53X_REG_CIU_TestSel1 0x6321
#define PN53X_REG_CIU_TestSel2 0x6322
#define PN53X_REG_CIU_TestPinEn 0x6323
#define PN53X_REG_CIU_TestPinValue 0x6324
#define PN53X_REG_CIU_TestBus 0x6325
#define PN53X_REG_CIU_AutoTest 0x6326
#define PN53X_REG_CIU_Version 0x6327
#define PN53X_REG_CIU_AnalogTest 0x6328
#define PN53X_REG_CIU_TestDAC1 0x6329
#define PN53X_REG_CIU_TestDAC2 0x632A
#define PN53X_REG_CIU_TestADC 0x632B
// #define PN53X_REG_- 0x632C
// #define PN53X_REG_- 0x632D
// #define PN53X_REG_- 0x632E
#define PN53X_REG_CIU_RFlevelDet 0x632F
#define PN53X_REG_CIU_SIC_CLK_en 0x6330
#define PN53X_REG_CIU_Command 0x6331
#define PN53X_REG_CIU_CommIEn 0x6332
#define PN53X_REG_CIU_DivIEn 0x6333
#define PN53X_REG_CIU_CommIrq 0x6334
#define PN53X_REG_CIU_DivIrq 0x6335
#define PN53X_REG_CIU_Error 0x6336
#define PN53X_REG_CIU_Status1 0x6337
#define PN53X_REG_CIU_Status2 0x6338
#define PN53X_REG_CIU_FIFOData 0x6339
#define PN53X_REG_CIU_FIFOLevel 0x633A
#define PN53X_REG_CIU_WaterLevel 0x633B
#define PN53X_REG_CIU_Control 0x633C
#define PN53X_REG_CIU_BitFraming 0x633D
#define PN53X_REG_CIU_Coll 0x633E

#define PN53X_SFR_P3 0xFFB0

#define PN53X_SFR_P3CFGA 0xFFFC
#define PN53X_SFR_P3CFGB 0xFFFD
#define PN53X_SFR_P7CFGA 0xFFF4
#define PN53X_SFR_P7CFGB 0xFFF5
#define PN53X_SFR_P7 0xFFF7

/* PN53x specific errors */
#define ETIMEOUT	0x01
#define ECRC		0x02
#define EPARITY		0x03
#define EBITCOUNT	0x04
#define EFRAMING	0x05
#define EBITCOLL	0x06
#define ESMALLBUF	0x07
#define EBUFOVF		0x09
#define ERFTIMEOUT	0x0a
#define ERFPROTO	0x0b
#define EOVHEAT		0x0d
#define EINBUFOVF	0x0e
#define EINVPARAM	0x10
#define EDEPUNKCMD	0x12
#define EINVRXFRAM	0x13
#define EMFAUTH		0x14
#define ENSECNOTSUPP	0x18	// PN533 only
#define EBCC		0x23
#define EDEPINVSTATE	0x25
#define EOPNOTALL	0x26
#define ECMD		0x27
#define ETGREL		0x29
#define ECID		0x2a
#define ECDISCARDED	0x2b
#define ENFCID3		0x2c
#define EOVCURRENT	0x2d
#define ENAD		0x2e



// Miscellaneous
#define Diagnose 0x00
#define GetFirmwareVersion 0x02
#define GetGeneralStatus 0x04
#define ReadRegister 0x06
#define WriteRegister 0x08
#define ReadGPIO 0x0C
#define WriteGPIO 0x0E
#define SetSerialBaudRate 0x10
#define SetParameters 0x12
#define SAMConfiguration 0x14
#define PowerDown 0x16
#define AlparCommandForTDA 0x18
// RC-S360 has another command 0x18 for reset &..?

// RF communication
#define RFConfiguration 0x32
#define RFRegulationTest 0x58

// Initiator
#define InJumpForDEP 0x56
#define InJumpForPSL 0x46
#define InListPassiveTarget 0x4A
#define InATR 0x50
#define InPSL 0x4E
#define InDataExchange 0x40
#define InCommunicateThru 0x42
#define InQuartetByteExchange 0x38
#define InDeselect 0x44
#define InRelease 0x52
#define InSelect 0x54
#define InActivateDeactivatePaypass 0x48
#define InAutoPoll 0x60

// Target
#define TgInitAsTarget 0x8C
#define TgSetGeneralBytes 0x92
#define TgGetData 0x86
#define TgSetData 0x8E
#define TgSetDataSecure 0x96
#define TgSetMetaData 0x94
#define TgSetMetaDataSecure 0x98
#define TgGetInitiatorCommand 0x88
#define TgResponseToInitiator 0x90
#define TgGetTargetStatus 0x8A

/** @note PN53x's normal frame:
*
*   .-- Start
*   |   .-- Packet length
*   |   |  .-- Length checksum
*   |   |  |  .-- Direction (D4 Host to PN, D5 PN to Host)
*   |   |  |  |  .-- Code
*   |   |  |  |  |  .-- Packet checksum
*   |   |  |  |  |  |  .-- Postamble
*   V   |  |  |  |  |  |
* ----- V  V  V  V  V  V
* 00 FF 02 FE D4 02 2A 00
*/

/** @note PN53x's extended frame:
*
*   .-- Start
*   |     .-- Fixed to FF to enable extended frame
*   |     |     .-- Packet length
*   |     |     |   .-- Length checksum
*   |     |     |   |  .-- Direction (D4 Host to PN, D5 PN to Host)
*   |     |     |   |  |  .-- Code
*   |     |     |   |  |  |  .-- Packet checksum
*   |     |     |   |  |  |  |  .-- Postamble
*   V     V     V   |  |  |  |  |
* ----- ----- ----- V  V  V  V  V
* 00 FF FF FF 00 02 FE D4 02 2A 00
*/

/**
* Start bytes, packet length, length checksum, direction, packet checksum and postamble are overhead
*/
// The TFI is considered part of the overhead
#  define PN53x_NORMAL_FRAME__DATA_MAX_LEN              254
#  define PN53x_NORMAL_FRAME__OVERHEAD                  8
#  define PN53x_EXTENDED_FRAME__DATA_MAX_LEN            264
#  define PN53x_EXTENDED_FRAME__OVERHEAD                11
#  define PN53x_ACK_FRAME__LEN                          6


/* Error codes */
/** @ingroup error
* @hideinitializer
* Success (no error)
*/
#define NFC_SUCCESS			 0
/** @ingroup error
* @hideinitializer
* Input / output error, device may not be usable anymore without re-open it
*/
#define NFC_EIO				-1
/** @ingroup error
* @hideinitializer
* Invalid argument(s)
*/
#define NFC_EINVARG			-2
/** @ingroup error
* @hideinitializer
*  Operation not supported by device
*/
#define NFC_EDEVNOTSUPP			-3
/** @ingroup error
* @hideinitializer
* No such device
*/
#define NFC_ENOTSUCHDEV			-4
/** @ingroup error
* @hideinitializer
* Buffer overflow
*/
#define NFC_EOVFLOW			-5
/** @ingroup error
* @hideinitializer
* Operation timed out
*/
#define NFC_ETIMEOUT			-6
/** @ingroup error
* @hideinitializer
* Operation aborted (by user)
*/
#define NFC_EOPABORTED			-7
/** @ingroup error
* @hideinitializer
* Not (yet) implemented
*/
#define NFC_ENOTIMPL			-8
/** @ingroup error
* @hideinitializer
* Target released
*/
#define NFC_ETGRELEASED			-10
/** @ingroup error
* @hideinitializer
* Error while RF transmission
*/
#define NFC_ERFTRANS			-20
/** @ingroup error
* @hideinitializer
* MIFARE Classic: authentication failed
*/
#define NFC_EMFCAUTHFAIL		-30
/** @ingroup error
* @hideinitializer
* Software error (allocation, file/pipe creation, etc.)
*/
#define NFC_ESOFT			-80
/** @ingroup error
* @hideinitializer
* Device's internal chip error
*/
#define NFC_ECHIP			-90



typedef enum pn53x_power_mode {
	NORMAL,	// In that case, there is no power saved but the PN53x reacts as fast as possible on the host controller interface.
	POWERDOWN,	// Only on PN532, need to be wake up to process commands with a long preamble
	LOWVBAT	// Only on PN532, need to be wake up to process commands with a long preamble and SAMConfiguration command
} pn532_power_mode;

/**
* @enum pn532_sam_mode
* @brief PN532 SAM mode enumeration
*/
typedef enum pn532_sam_mode {
	PSM_NORMAL = 0x01,
	PSM_VIRTUAL_CARD = 0x02,
	PSM_WIRED_CARD = 0x03,
	PSM_DUAL_CARD = 0x04
} pn532_sam_mode;	

typedef struct _tag_pn532_status
{
	pserial_port sp;
	pn532_power_mode power_mode;
	uint8_t last_command;
	uint8_t last_status_byte;
	uint8_t last_error;
	pn532_sam_mode sam_mode;
}pn532_status, *ppn532_status;

ppn532_status pn532_uart_open(const char* connstring);
void pn532_uart_close(ppn532_status ps);
int pn532_check_communication(ppn532_status ps);
int pn532_transceive(ppn532_status ps, const uint8_t *pbtTx, const size_t szTx, uint8_t *pbtRx, const size_t szRxLen, int timeout);
int pn532_uart_wakeup(ppn532_status ps);
int pn532_build_frame(uint8_t * pbtFrame, size_t * pszFrame, const uint8_t * pbtData, const size_t szData);
int pn532_SAMConfiguration(ppn532_status ps, const pn532_sam_mode sam_mode, int timeout);
int pn532_check_ack_frame(ppn532_status ps, const uint8_t * pbtRxFrame, const size_t szRxFrameLen);
int pn532_uart_receive(ppn532_status ps, uint8_t *pbtData, const size_t szDataLen, int timeout);
int pn532_uart_send(ppn532_status ps, const uint8_t *pbtData, const size_t szData, int timeout);
const char *pn53x_strerror(uint8_t errcode);

#endif /*__PN532_UART_H*/
