#include "nfc.h"
#include "pn532_uart.h"

void nfc_open(const char * connstring)
{
	pn532_uart_open(connstring);
}

static struct sErrorMessage {
	int     iErrorCode;
	const char *pcErrorMsg;
} sErrorMessages[] = {
	/* Chip-level errors (internal errors, RF errors, etc.) */
	{ NFC_SUCCESS, "Success" },
	{ NFC_EIO, "Input / Output Error" },
	{ NFC_EINVARG, "Invalid argument(s)" },
	{ NFC_EDEVNOTSUPP, "Not Supported by Device" },
	{ NFC_ENOTSUCHDEV, "No Such Device" },
	{ NFC_EOVFLOW, "Buffer Overflow" },
	{ NFC_ETIMEOUT, "Timeout" },
	{ NFC_EOPABORTED, "Operation Aborted" },
	{ NFC_ENOTIMPL, "Not (yet) Implemented" },
	{ NFC_ETGRELEASED, "Target Released" },
	{ NFC_EMFCAUTHFAIL, "Mifare Authentication Failed" },
	{ NFC_ERFTRANS, "RF Transmission Error" },
	{ NFC_ECHIP, "Device's Internal Chip Error" },
};

/** @ingroup error
* @brief Return the last error string
* @return Returns a string
*
* @param pnd \a nfc_device struct pointer that represent currently used device
*/
const char * nfc_strerror(const uint8_t errcode)
{
	const char *pcRes = "Unknown error";
	int i;
	for (i = 0; i < (sizeof(sErrorMessages) / sizeof(struct sErrorMessage)); i++) {
		if (sErrorMessages[i].iErrorCode == errcode) {
			pcRes = sErrorMessages[i].pcErrorMsg;
			break;
		}
	}

	return pcRes;
}