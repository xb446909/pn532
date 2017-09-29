#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pn532_uart.h"
#include "uart.h"
#include "logger.h"
#include "nfc.h"

const uint8_t pn532_ack_frame[] = { 0x00, 0x00, 0xff, 0x00, 0xff, 0x00 };
const uint8_t pn532_nack_frame[] = { 0x00, 0x00, 0xff, 0xff, 0x00, 0x00 };

ppn532_status pn532_uart_open(const char* connstring)
{
	ppn532_status ps = (ppn532_status)malloc(sizeof(pn532_status));
    char* pos = strrchr(connstring, ':');
    if (pos != NULL)
    {
        *pos = '\0';
        pos++;
		if ((ps->sp = uart_open(connstring)) == NULL)
		{
			LOG_ERROR("Open serial port failed!");
		}
		uart_flush_input(ps->sp);
        uart_set_speed(ps->sp, atoi(pos));
    }
    else
    {
		ps->sp = uart_open(connstring);
		uart_flush_input(ps->sp);
		uart_set_speed(ps->sp, DEFAULT_UART_SPEED);
    }
    if (ps->sp == NULL)
    {
		free(ps);
		return NULL;
    }

	ps->power_mode = LOWVBAT;
	if (pn532_check_communication(ps) != 0)
	{

	}
    return ps;
}

void pn532_uart_close(ppn532_status ps)
{
	uart_close(ps->sp);
	free(ps);
}

int pn532_check_communication(ppn532_status ps)
{
    const uint8_t abtCmd[] = { Diagnose, 0x00, 'l', 'i', 'b', 'n', 'f', 'c' };
    const uint8_t abtExpectedRx[] = { 0x00, 'l', 'i', 'b', 'n', 'f', 'c' };
    uint8_t abtRx[sizeof(abtExpectedRx)];
    size_t szRx = sizeof(abtRx);
    int res = 0;
    
    if ((res = pn532_transceive(ps, abtCmd, sizeof(abtCmd), abtRx, szRx, 500)) < 0)
        return res;
    szRx = (size_t) res;
    if ((sizeof(abtExpectedRx) == szRx) && 
            (0 == memcmp(abtRx, abtExpectedRx, sizeof(abtExpectedRx))))
        return 0;
    
    return -1;
}


int pn532_transceive(ppn532_status ps, const uint8_t *pbtTx, const size_t szTx, uint8_t *pbtRx, const size_t szRxLen, int timeout)
{
	bool mi = false;
	int res = 0;
	uint8_t  abtRx[PN53x_EXTENDED_FRAME__DATA_MAX_LEN];
	size_t  szRx = sizeof(abtRx);

	// Check if receiving buffers are available, if not, replace them
	if (szRxLen == 0 || !pbtRx) 
	{
		pbtRx = abtRx;
	}
	else 
	{
		szRx = szRxLen;
	}
	if ((res = pn532_uart_send(ps, pbtTx, szTx, timeout)) < 0)
	{
		return res;
	}
	ps->last_command = pbtTx[0];
	if (TgInitAsTarget == pbtTx[0])
	{
		ps->power_mode = POWERDOWN;
	}
	if ((res = pn532_uart_receive(ps, pbtRx, szRx, timeout)) < 0)
	{
		return res;
	}
	if (TgInitAsTarget == pbtTx[0])
	{
		ps->power_mode = NORMAL;
	}

	switch (pbtTx[0]) {
	case PowerDown:
	case InDataExchange:
	case InCommunicateThru:
	case InJumpForPSL:
	case InPSL:
	case InATR:
	case InSelect:
	case InJumpForDEP:
	case TgGetData:
	case TgGetInitiatorCommand:
	case TgSetData:
	case TgResponseToInitiator:
	case TgSetGeneralBytes:
	case TgSetMetaData:
		if (pbtRx[0] & 0x80) { abort(); } // NAD detected
										  //      if (pbtRx[0] & 0x40) { abort(); } // MI detected
		mi = pbtRx[0] & 0x40;
		ps->last_status_byte = pbtRx[0] & 0x3f;
		break;
	case Diagnose:
		if (pbtTx[1] == 0x06) // Diagnose: Card presence detection
		{ 
			ps->last_status_byte = pbtRx[0] & 0x3f;
		}
		else 
		{
			ps->last_status_byte = 0;
		};
		break;
	case InDeselect:
	case InRelease:
		ps->last_status_byte = pbtRx[0] & 0x3f;
		break;
	case ReadRegister:
	case WriteRegister:
		ps->last_status_byte = 0;
		break;
	default:
		ps->last_status_byte = 0;
	}

	while (mi) {
		int res2;
		uint8_t  abtRx2[PN53x_EXTENDED_FRAME__DATA_MAX_LEN];
		// Send empty command to card
		if ((res2 = pn532_uart_send(ps, pbtTx, 2, timeout)) < 0) {
			return res2;
		}
		if ((res2 = pn532_uart_receive(ps, abtRx2, sizeof(abtRx2), timeout)) < 0) {
			return res2;
		}
		mi = abtRx2[0] & 0x40;
		if ((size_t)(res + res2 - 1) > szRx) {
			ps->last_status_byte = ESMALLBUF;
			break;
		}
		memcpy(pbtRx + res, abtRx2 + 1, res2 - 1);
		// Copy last status byte
		pbtRx[0] = abtRx2[0];
		res += res2 - 1;
	}

	szRx = (size_t)res;

	switch (ps->last_status_byte) {
	case 0:
		res = (int)szRx;
		break;
	case ETIMEOUT:
	case ECRC:
	case EPARITY:
	case EBITCOUNT:
	case EFRAMING:
	case EBITCOLL:
	case ERFPROTO:
	case ERFTIMEOUT:
	case EDEPUNKCMD:
	case EDEPINVSTATE:
	case ENAD:
	case ENFCID3:
	case EINVRXFRAM:
	case EBCC:
	case ECID:
		res = NFC_ERFTRANS;
		break;
	case ESMALLBUF:
	case EOVCURRENT:
	case EBUFOVF:
	case EOVHEAT:
	case EINBUFOVF:
		res = NFC_ECHIP;
		break;
	case EINVPARAM:
	case EOPNOTALL:
	case ECMD:
	case ENSECNOTSUPP:
		res = NFC_EINVARG;
		break;
	case ETGREL:
	case ECDISCARDED:
		res = NFC_ETGRELEASED;
		//pn53x_current_target_free(pnd); @TODO Free target
		break;
	case EMFAUTH:
		// When a MIFARE Classic AUTH fails, the tag is automatically in HALT state
		res = NFC_EMFCAUTHFAIL;
		break;
	default:
		res = NFC_ECHIP;
		break;
	};

	if (res < 0) {
		ps->last_error = res;
		LOG_ERROR("Chip error: \"%s\" (%02x), returned error: \"%s\" (%d))", pn53x_strerror(ps->last_status_byte), ps->last_status_byte, nfc_strerror(res), res);
	}
	else {
		ps->last_error = 0;
	}
	return res;
}

int pn532_uart_wakeup(ppn532_status ps)
{
	/* High Speed Unit (HSU) wake up consist to send 0x55 and wait a "long" delay for PN532 being wakeup. */
	const uint8_t pn532_wakeup_preamble[] = { 0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	int res = uart_send(ps->sp, pn532_wakeup_preamble, sizeof(pn532_wakeup_preamble), 0);
	ps->power_mode = NORMAL; // PN532 should now be awake
	return res;
}

/**
* @brief Build a PN53x frame
*
* @param pbtData payload (bytes array) of the frame, will become PD0, ..., PDn in PN53x frame
* @note The first byte of pbtData is the Command Code (CC)
*/
int pn532_build_frame(uint8_t *pbtFrame, size_t *pszFrame, const uint8_t *pbtData, const size_t szData)
{
	if (szData <= PN53x_NORMAL_FRAME__DATA_MAX_LEN) {
		// LEN - Packet length = data length (len) + checksum (1) + end of stream marker (1)
		pbtFrame[3] = szData + 1;
		// LCS - Packet length checksum
		pbtFrame[4] = 256 - (szData + 1);
		// TFI
		pbtFrame[5] = 0xD4;
		// DATA - Copy the PN53X command into the packet buffer
		memcpy(pbtFrame + 6, pbtData, szData);

		// DCS - Calculate data payload checksum
		uint8_t btDCS = (256 - 0xD4);
		for (size_t szPos = 0; szPos < szData; szPos++) {
			btDCS -= pbtData[szPos];
		}
		pbtFrame[6 + szData] = btDCS;

		// 0x00 - End of stream marker
		pbtFrame[szData + 7] = 0x00;

		(*pszFrame) = szData + PN53x_NORMAL_FRAME__OVERHEAD;
	}
	else if (szData <= PN53x_EXTENDED_FRAME__DATA_MAX_LEN) 
	{
		// Extended frame marker
		pbtFrame[3] = 0xff;
		pbtFrame[4] = 0xff;
		// LENm
		pbtFrame[5] = (szData + 1) >> 8;
		// LENl
		pbtFrame[6] = (szData + 1) & 0xff;
		// LCS
		pbtFrame[7] = 256 - ((pbtFrame[5] + pbtFrame[6]) & 0xff);
		// TFI
		pbtFrame[8] = 0xD4;
		// DATA - Copy the PN53X command into the packet buffer
		memcpy(pbtFrame + 9, pbtData, szData);

		// DCS - Calculate data payload checksum
		uint8_t btDCS = (256 - 0xD4);
		for (size_t szPos = 0; szPos < szData; szPos++) {
			btDCS -= pbtData[szPos];
		}
		pbtFrame[9 + szData] = btDCS;

		// 0x00 - End of stream marker
		pbtFrame[szData + 10] = 0x00;

		(*pszFrame) = szData + PN53x_EXTENDED_FRAME__OVERHEAD;
	}
	else 
	{
		LOG_ERROR("We can't send more than %d bytes in a raw (requested: %s )", PN53x_EXTENDED_FRAME__DATA_MAX_LEN, szData);
		return NFC_ECHIP;
	}
	return NFC_SUCCESS;
}

int pn532_SAMConfiguration(ppn532_status ps, const pn532_sam_mode sam_mode, int timeout)
{
	uint8_t abtCmd[] = { SAMConfiguration, sam_mode, 0x00, 0x00 };
	size_t szCmd = sizeof(abtCmd);

	switch (sam_mode) {
	case PSM_NORMAL: // Normal mode
	case PSM_WIRED_CARD: // Wired card mode
		szCmd = 2;
		break;
	case PSM_VIRTUAL_CARD: // Virtual card mode
	case PSM_DUAL_CARD: // Dual card mode
						// TODO Implement timeout handling
		szCmd = 3;
		break;
	default:
		ps->last_error = NFC_EINVARG;
		return ps->last_error;
	}
	ps->sam_mode = sam_mode;
	return (pn532_transceive(ps, abtCmd, szCmd, NULL, 0, timeout));
}

int pn532_check_ack_frame(ppn532_status ps, const uint8_t *pbtRxFrame, const size_t szRxFrameLen)
{
	if (szRxFrameLen >= sizeof(pn532_ack_frame)) {
		if (0 == memcmp(pbtRxFrame, pn532_ack_frame, sizeof(pn532_ack_frame))) {
			LOG_DEBUG("PN53x ACKed");
			return NFC_SUCCESS;
		}
	}
	ps->last_error = NFC_EIO;
	LOG_ERROR("Unexpected PN53x reply!");
	return ps->last_error;
}

#define PN532_BUFFER_LEN (PN53x_EXTENDED_FRAME__DATA_MAX_LEN + PN53x_EXTENDED_FRAME__OVERHEAD)

int pn532_uart_receive(ppn532_status ps, uint8_t * pbtData, const size_t szDataLen, int timeout)
{
	uint8_t  abtRxBuf[5];
	size_t len;
	void *abort_p = NULL;

	ps->last_error = uart_receive(ps->sp, abtRxBuf, 5, abort_p, timeout);

	if (abort_p && (NFC_EOPABORTED == ps->last_error)) {
		pn532_uart_ack(ps);
		return NFC_EOPABORTED;
	}

	if (ps->last_error < 0) {
		goto error;
	}

	const uint8_t pn53x_preamble[3] = { 0x00, 0x00, 0xff };
	if (0 != (memcmp(abtRxBuf, pn53x_preamble, 3))) {
		LOG_ERROR("Frame preamble+start code mismatch");
		ps->last_error = NFC_EIO;
		goto error;
	}

	if ((0x01 == abtRxBuf[3]) && (0xff == abtRxBuf[4])) {
		// Error frame
		uart_receive(ps->sp, abtRxBuf, 3, 0, timeout);
		LOG_ERROR("Application level error detected");
		ps->last_error = NFC_EIO;
		goto error;
	}
	else if ((0xff == abtRxBuf[3]) && (0xff == abtRxBuf[4])) {
		// Extended frame
		ps->last_error = uart_receive(ps->sp, abtRxBuf, 3, 0, timeout);
		if (ps->last_error != 0) {
			LOG_ERROR("Unable to receive data. (RX)");
			goto error;
		}
		// (abtRxBuf[0] << 8) + abtRxBuf[1] (LEN) include TFI + (CC+1)
		len = (abtRxBuf[0] << 8) + abtRxBuf[1] - 2;
		if (((abtRxBuf[0] + abtRxBuf[1] + abtRxBuf[2]) % 256) != 0) {
			LOG_ERROR("Length checksum mismatch");
			ps->last_error = NFC_EIO;
			goto error;
		}
	}
	else {
		// Normal frame
		if (256 != (abtRxBuf[3] + abtRxBuf[4])) {
			// TODO: Retry
			LOG_ERROR("Length checksum mismatch");
			ps->last_error = NFC_EIO;
			goto error;
		}

		// abtRxBuf[3] (LEN) include TFI + (CC+1)
		len = abtRxBuf[3] - 2;
	}

	if (len > szDataLen) {
		LOG_ERROR("Unable to receive data: buffer too small. (szDataLen: %d, len: %d)", szDataLen, len);
		ps->last_error = NFC_EIO;
		goto error;
	}

	// TFI + PD0 (CC+1)
	ps->last_error = uart_receive(ps->sp, abtRxBuf, 2, 0, timeout);
	if (ps->last_error != 0) {
		LOG_ERROR("Unable to receive data. (RX)");
		goto error;
	}

	if (abtRxBuf[0] != 0xD5) {
		LOG_ERROR("TFI Mismatch");
		ps->last_error = NFC_EIO;
		goto error;
	}

	if (abtRxBuf[1] != ps->last_command + 1) {
		LOG_ERROR("Command Code verification failed");
		ps->last_error = NFC_EIO;
		goto error;
	}

	if (len) {
		ps->last_error = uart_receive(ps->sp, pbtData, len, 0, timeout);
		if (ps->last_error != 0) {
			LOG_ERROR("Unable to receive data. (RX)");
			goto error;
		}
	}

	ps->last_error = uart_receive(ps->sp, abtRxBuf, 2, 0, timeout);
	if (ps->last_error != 0) {
		LOG_ERROR("Unable to receive data. (RX)");
		goto error;
	}

	uint8_t btDCS = (256 - 0xD5);
	btDCS -= ps->last_command + 1;
	for (size_t szPos = 0; szPos < len; szPos++) {
		btDCS -= pbtData[szPos];
	}

	if (btDCS != abtRxBuf[0]) {
		LOG_ERROR("Data checksum mismatch");
		ps->last_error = NFC_EIO;
		goto error;
	}

	if (0x00 != abtRxBuf[1]) {
		LOG_ERROR("Frame postamble mismatch");
		ps->last_error = NFC_EIO;
		goto error;
	}
	// The PN53x command is done and we successfully received the reply
	return len;
error:
	uart_flush_input(ps->sp);
	return ps->last_error;
}

int pn532_uart_send(ppn532_status ps, const uint8_t * pbtData, const size_t szData, int timeout)
{
	int res = 0;
	// Before sending anything, we need to discard from any junk bytes
	uart_flush_input(ps->sp);

	switch (ps->power_mode) {
	case LOWVBAT: {
		/** PN532C106 wakeup. */
		if ((res = pn532_uart_wakeup(ps)) < 0) 
		{
			return res;
		}
		// According to PN532 application note, C106 appendix: to go out Low Vbat mode and enter in normal mode we need to send a SAMConfiguration command
		if ((res = pn532_SAMConfiguration(ps, PSM_NORMAL, 1000)) < 0) 
		{
			return res;
		}
	}
		break;
	case POWERDOWN: {
		if ((res = pn532_uart_wakeup(ps)) < 0) 
		{
			return res;
		}
	}
		break;
	case NORMAL:
		// Nothing to do :)
		break;
	};

	uint8_t  abtFrame[PN532_BUFFER_LEN] = { 0x00, 0x00, 0xff };       // Every packet must start with "00 00 ff"
	size_t szFrame = 0;

	if ((res = pn532_build_frame(abtFrame, &szFrame, pbtData, szData)) < 0) 
	{
		ps->last_error = res;
		return ps->last_error;
	}

	res = uart_send(ps->sp, abtFrame, szFrame, timeout);
	if (res != 0) {
		LOG_ERROR("Unable to transmit data. (TX)");
		ps->last_error = res;
		return ps->last_error;
	}

	uint8_t abtRxBuf[PN53x_ACK_FRAME__LEN];
	res = uart_receive(ps->sp, abtRxBuf, sizeof(abtRxBuf), 0, timeout);
	if (res != 0) {
		LOG_ERROR("Unable to read ACK");
		ps->last_error = res;
		return ps->last_error;
	}

	if (pn532_check_ack_frame(ps, abtRxBuf, sizeof(abtRxBuf)) == 0) 
	{
		// The PN53x is running the sent command
	}
	else 
	{
		return ps->last_error;
	}
	return NFC_SUCCESS;
}


static struct sErrorMessage {
	int     iErrorCode;
	const char *pcErrorMsg;
} sErrorMessages[] = {
	/* Chip-level errors (internal errors, RF errors, etc.) */
	{ 0x00, "Success" },
	{ ETIMEOUT, "Timeout" },      // Time Out, the target has not answered
	{ ECRC,     "CRC Error" },      // A CRC error has been detected by the CIU
	{ EPARITY,  "Parity Error" },     // A Parity error has been detected by the CIU
	{ EBITCOUNT, "Erroneous Bit Count" },   // During an anti-collision/select operation (ISO/IEC14443-3 Type A and ISO/IEC18092 106 kbps passive mode), an erroneous Bit Count has been detected
	{ EFRAMING, "Framing Error" },    // Framing error during MIFARE operation
	{ EBITCOLL, "Bit-collision" },    // An abnormal bit-collision has been detected during bit wise anti-collision at 106 kbps
	{ ESMALLBUF, "Communication Buffer Too Small" },  // Communication buffer size insufficient
	{ EBUFOVF, "Buffer Overflow" },   // RF Buffer overflow has been detected by the CIU (bit BufferOvfl of the register CIU_Error)
	{ ERFPROTO, "RF Protocol Error" },    // RF Protocol error (see PN53x manual)
	{ EOVHEAT, "Chip Overheating" },    // Temperature error: the internal temperature sensor has detected overheating, and therefore has automatically switched off the antenna drivers
	{ EINBUFOVF, "Internal Buffer overflow." },  // Internal buffer overflow
	{ EINVPARAM, "Invalid Parameter" },    // Invalid parameter (range, format, бн)
	{ EOPNOTALL, "Operation Not Allowed" }, // Operation not allowed in this configuration (host controller interface)
	{ ECMD, "Command Not Acceptable" },   // Command is not acceptable due to the current context
	{ EOVCURRENT, "Over Current" },
	/* DEP errors */
	{ ERFTIMEOUT, "RF Timeout" },     // In active communication mode, the RF field has not been switched on in time by the counterpart (as defined in NFCIP-1 standard)
	{ EDEPUNKCMD, "Unknown DEP Command" },
	{ EDEPINVSTATE, "Invalid DEP State" },  // DEP Protocol: Invalid device state, the system is in a state which does not allow the operation
	{ ENAD, "NAD Missing in DEP Frame" },
	/* MIFARE */
	{ EMFAUTH, "Mifare Authentication Error" },
	/* Misc */
	{ EINVRXFRAM, "Invalid Received Frame" }, // DEP Protocol, Mifare or ISO/IEC14443-4: The data format does not match to the specification.
	{ ENSECNOTSUPP, "NFC Secure not supported" }, // Target or Initiator does not support NFC Secure
	{ EBCC, "Wrong UID Check Byte (BCC)" }, // ISO/IEC14443-3: UID Check byte is wrong
	{ ETGREL, "Target Released" },    // Target have been released by initiator
	{ ECID, "Card ID Mismatch" },     // ISO14443 type B: Card ID mismatch, meaning that the expected card has been exchanged with another one.
	{ ECDISCARDED, "Card Discarded" },    // ISO/IEC14443 type B: the card previously activated has disappeared.
	{ ENFCID3, "NFCID3 Mismatch" },
};

const char *pn53x_strerror(uint8_t errcode)
{
	const char *pcRes = "Unknown error";
	size_t  i;

	for (i = 0; i < (sizeof(sErrorMessages) / sizeof(struct sErrorMessage)); i++) 
	{
		if (sErrorMessages[i].iErrorCode == errcode) {
			pcRes = sErrorMessages[i].pcErrorMsg;
			break;
		}
	}

	return pcRes;
}