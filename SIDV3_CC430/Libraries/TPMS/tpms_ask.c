/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
 * File Name          : tpms_ask.c
 * Description        : Module pour le decodage/encodage des capteurs TPMS_ASK.
 *******************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef _TPMS_ASK_C_
#define _TPMS_ASK_C_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <cc430f5137.h>
#include <stdint.h>
#include "tpms.h"
#include "tpms_ask.h"

/* Private constants ---------------------------------------------------------*/
#define TPMS_ASK_BAUDRATE	 4200

#define TPMS_ASK_HALF_BIT	 TPMS_HB_CALCULATOR(TPMS_ASK_BAUDRATE) // 150u
#define TPMS_ASK_FULL_BIT	 (2*TPMS_ASK_HALF_BIT)

#define TPMS_ASK_HB_THD		TPMS_HB_THD_CALCULATOR(TPMS_ASK_HALF_BIT) //(400u) //1750

/* Private macro -------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#ifdef TPMS_USE_DECODER
/*static uint16_t TPMS_ASK_FindFrameLenWithCRC(bytesData_t *data)
{
	uint16_t retVal_ 	= 0u;
	uint16_t ask_checksum_;
	uint8_t ask_checksum2_;

	if ( data->sz > 5 )
	{
		ask_checksum_ = 0u;
		for ( retVal_=0u; retVal_<data->sz-1; retVal_++)
		{
			// Read CRC on next byte
			ask_checksum2_ = data->data[retVal_+1];

			// Calculate CRC on frame
			ask_checksum_ += data->data[retVal_];

			// Check CRC
			if ( (ask_checksum_ & 0xFF) == ask_checksum2_ )
			{ // CRC found
				retVal_ += 2; // To include the CRC byte and add 1 for index 0
				break;
			}
		}


		data->crcC = ask_checksum_ & 0xFF;
		data->crcR = ask_checksum2_;

		if ( (ask_checksum_ & 0xFF) == ask_checksum2_ )
		{
			data->sz = retVal_;
		}else{
			retVal_ = 0u;
		}
	}else{
		data->sz = 0u;
		retVal_ = 0u;
	}

	return retVal_;
}*/

static uint8_t TPMS_ASK_CheckCRC(bytesData_t *data)
{
	uint16_t retVal_ 	= 0u;
	uint16_t ask_checksum_= 0, ask_checksum2_;

	ask_checksum2_ = data->data[data->sz-1];

	if(ask_checksum2_ != 0x00)
	{
		//ask_checksum_ = TPMS_ASK_CalculateCRC(data->data, data->sz-1u);
		for ( retVal_ = 0u; retVal_ < (data->sz - 1); retVal_++)
		{
			// Calculate CRC on frame
			ask_checksum_ += data->data[retVal_];
		}
	}

	data->crcC = (ask_checksum_ & 0xFF);
	data->crcR = ask_checksum2_;

	return ( (ask_checksum_ & 0xFF) == ask_checksum2_ );
}

/**
 * @brief  Decode frame from capture structure.
 *
 * @param  captureData struct describing captured data.
 * @param  decodedData pointer on buffer to store decoded data.
 * @param  pLen		   pointer on decoded data len (in input is the max size for decodedData tab.
 * 								in output the decoded data len).
 * @return ask_TAG if decoded, else NO_TAG.
 */
TagTypes_t TPMS_ASK_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *encodedData)
{
	TagTypes_t ask_retVal_ = NO_TAG;
	uint16_t ask_decode_ptr_ = 0u;
	uint8_t  ask_endOfFrame = 0u;

	TagDecodeStates_e state = TAG_START;

	//uint16_t ask_start_ptr_ = 0u;
	uint8_t current_byte = 0u;
	uint8_t bit_state = 0u;
	uint8_t bit_idx = 0u;
	uint8_t first_short = 0u;

	encodedData->sz = 0u;

	uint16_t bit_halfbit_value = TPMS_ASK_HB_THD;
	uint16_t bit_limit_high = TPMS_ASK_FULL_BIT + TPMS_ASK_HALF_BIT; //3u * TPMS_ASK_HALF_BIT / 2u;
	uint16_t bit_limit_low = TPMS_ASK_HALF_BIT / 2u;

	while((ask_endOfFrame == 0u)/* && ((ask_decode_ptr_) < (captureData->sz))*/)
	{

		switch(state)
		{
		case TAG_START:
			/* search for 2 consecutive correct timing */
			if(
					(captureData->data[ask_decode_ptr_] > (bit_limit_low)) &&
					(captureData->data[ask_decode_ptr_] < (bit_limit_high))
			)
			{
				if(first_short == 1u)
				{
					state = TAG_PREAMBLE;
				}else{
					first_short  = 1u;
				}
			}else{
				/* error
				 * bit too short
				 * bit too long
				 */
				first_short  = 0u;
			}

			ask_decode_ptr_++;
			break;

		case TAG_PREAMBLE:
			/* Search start of 0x4C
			 * Should be a long byte
			 */
			if(captureData->data[ask_decode_ptr_] > bit_halfbit_value) //l
			{
				/* Long byte counter */
				state = TAG_DATA;

				/* -3 because first bit has already skipped */
				ask_decode_ptr_ -= 3;

				bit_state = 0u;
				current_byte = 0u;
			}

			ask_decode_ptr_++;
			break;

		case TAG_DATA:
			current_byte = current_byte << 1u;

			if((captureData->data[ask_decode_ptr_] < (bit_limit_low)) || (captureData->data[ask_decode_ptr_] > (bit_limit_high)) || ((ask_decode_ptr_) >= (captureData->sz)))
			{
				state = TAG_CRC;
			}else if(captureData->data[ask_decode_ptr_] < TPMS_ASK_HB_THD) //c
			{
				// skip next value
				ask_decode_ptr_ += 2u;
			}
			else //l
			{
				bit_state = (bit_state == 0);
				ask_decode_ptr_++;
			}

			current_byte += bit_state;
			bit_idx++;

			if(bit_idx == 8u)
			{
				encodedData->data[encodedData->sz] = current_byte;
				encodedData->sz++;

				current_byte = 0u;
				bit_idx = 0;
			}
			break;

		case TAG_CRC:
			ask_endOfFrame = 1u;
			switch(encodedData->data[0])
			{
			case 0x01:
				ask_retVal_ = REDI_TAG;
				encodedData->type = REDI_TAG;
				break;

			case 0x4C:
			case 0x4D:
				//if (TPMS_ASK_FindFrameLenWithCRC(encodedData) > 0u)
				encodedData->sz = 10;
				if(TPMS_ASK_CheckCRC(encodedData))
				{
					ask_retVal_ = SCHRADER_TAG;
					encodedData->type = SCHRADER_TAG;
				}else{
					encodedData->sz = 0u;
					ask_retVal_ = NO_TAG;
				}
				break;

			default:
				encodedData->sz = 0u;
				ask_retVal_ = NO_TAG;
				break;
			}


			break;

		default:
			break;
		}

	}

	return ask_retVal_;
}
#endif /* TPMS_USE_DECODER */

#ifdef __cplusplus
}
#endif

#endif /* _TPMS_ASK_C_ */
