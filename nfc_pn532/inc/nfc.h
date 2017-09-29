#ifndef __NFC_H
#define __NFC_H

#include <stdint.h>

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


void nfc_open(const char* connstring);
const char * nfc_strerror(const uint8_t errcode);

#endif /*__NFC_H*/
