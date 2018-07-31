/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
 * File Name          : .c
 * Description        : --.
 *******************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef _TPMS_C_
#define _TPMS_C_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "boardconfig.h"
#include "tpms.h"
#include "tpms_mems.h"
#include "tpms_ldl.h"
#include "tpms_ak.h"
#include "tpms_ask.h"
#include "tpms_sid.h"

/* Private constants ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


/* Public functions ----------------------------------------------------------*/
#ifdef TPMS_USE_DECODER
/**
 * @brief  Decode frame from capture structure.
 *
 * @param  captureData pointer on captured data.
 * @param  decodedData pointer on decoded data.
 * @param  pLen pointer on dtaa len.
 * @return The tag type or NO_TAG.
 */
TagTypes_t TPMS_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *encodedData, uint8_t decodeMask)
{
	TagTypes_t decode_retVal_ = NO_TAG;

	encodedData->type = NO_TAG;

	if((captureData->sz < 100) || (decodeMask == 0u))
	{
		return NO_TAG;
	}

	memset(encodedData, 0, sizeof(bytesData_t));

	if((decodeMask & ((uint8_t) (1 << ASK_TAG))) != 0u)
	{
		// ASK Encoding (SCHRADER or REDI)
		decode_retVal_ = TPMS_ASK_DecodeFrameReceived(captureData, encodedData);
	}else{
		// FSK Encoding
		if((decode_retVal_ == NO_TAG) && ((decodeMask & ((uint8_t) (1 << MEMS_TAG))) != 0u))
		{
			// MEMS
			decode_retVal_ = TPMS_MEMS_DecodeFrameReceived(captureData, encodedData);
		}

		if((decode_retVal_ == NO_TAG) && ((decodeMask & ((uint8_t) (1 << LDL_TAG))) != 0u))
		{
			// LDL
			decode_retVal_ = TPMS_LDL_DecodeFrameReceived(captureData, encodedData);
		}

		if((decode_retVal_ == NO_TAG) && ((decodeMask & ((uint8_t) ((1 << AK_TAG) | (1 << SID_TAG) | (1 << REDI_TAG)))) != 0u))
		{
			// AK ou SID ak
			AK_Info_t ak_tag_info = { 0xFFF2u, 19200u, AK_TAG };
			decode_retVal_ = TPMS_AK_DecodeFrameReceived(captureData, encodedData, &ak_tag_info);
		}

		if((decode_retVal_ == NO_TAG) && ((decodeMask & ((uint8_t) (1 << REDI_TAG))) != 0u))
		{
			// REDI
			/*AK_Info_t ak_tag_info = { 0x0001u, 9600u, REDI_TAG };
			decode_retVal_ = TPMS_AK_DecodeFrameReceived(captureData, encodedData, &ak_tag_info);*/
		}

		if((decode_retVal_ == NO_TAG) && ((decodeMask & ((uint8_t) (1 << SCHRADER_TAG))) != 0u))
		{
			// SCHRADER
			AK_Info_t ak_tag_info = { 0x0001u, 9600u, SCHRADER_TAG };
			decode_retVal_ = TPMS_AK_DecodeFrameReceived(captureData, encodedData, &ak_tag_info);

			if(decode_retVal_ == NO_TAG)
			{
				ak_tag_info.baudrate = 19200u;
				ak_tag_info.preamble = 0xFFF2u;
				decode_retVal_ = TPMS_AK_DecodeFrameReceived(captureData, encodedData, &ak_tag_info);
			}
		}
	}

	return decode_retVal_;
}
#endif /* TPMS_USE_DECODER */

#ifdef TPMS_USE_ENCODER
/**
 * @brief  Decode frame from capture structure.
 *
 * @param  captureData pointer on captured data.
 * @param  decodedData pointer on decoded data.
 * @param  pLen pointer on dtaa len.
 * @return The tag type or NO_TAG.
 */
uint16_t 	TPMS_EncodeFrameToSend(CaptureData_t *captureData, bytesData_t *encodedData)
{
	switch ( encodedData->type )
	{
	case MEMS_TAG:
		TPMS_MEMS_EncodeFrameToSend(captureData, encodedData);
		break;
	case LDL_TAG:
		TPMS_LDL_EncodeFrameToSend(captureData, encodedData);
		break;
	case AK_TAG:
		TPMS_AK_EncodeFrameToSend(captureData, encodedData);
		break;
	case SCHRADER_TAG:
		TPMS_AK_EncodeFrameToSend(captureData, encodedData);
		break;
	case REDI_TAG:
		TPMS_AK_EncodeFrameToSend(captureData, encodedData);
		break;
	case HUF_TAG:
		TPMS_AK_EncodeFrameToSend(captureData, encodedData);
		break;
	case SID_TAG:
        TPMS_SID_EncodeFrameToSend(captureData, encodedData);
		break;
	default:
		break;
	}

	return 0u;
}
#endif /* TPMS_USE_ENCODER */

#ifdef __cplusplus
}
#endif

#endif /* _TPMS_C_ */

