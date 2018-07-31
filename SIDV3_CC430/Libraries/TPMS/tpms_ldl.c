/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
 * File Name          : tpms_ldl.c
 * Description        : Module pour le decodage/encodage des capteurs TPMS_LDL.
 *******************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef _TPMS_LDL_C_
#define _TPMS_LDL_C_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include <cc430f5137.h>
#include <stdint.h>
#include "boardconfig.h"
#include "tpms.h"
#include "tpms_ldl.h"

/* Private constants ---------------------------------------------------------*/
#define TPMS_LDL_BAUDRATE	 11364

#define TPMS_LDL_HALF_BIT	 TPMS_HB_CALCULATOR(TPMS_LDL_BAUDRATE) // 150u
#define TPMS_LDL_FULL_BIT	 (2*TPMS_LDL_HALF_BIT)

#define TPMS_LDL_HB_THD		TPMS_HB_THD_CALCULATOR(TPMS_LDL_HALF_BIT) //(400u) //1750

/* Private macro -------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/
#ifdef TPMS_USE_DECODER
static uint16_t bit_limit_high = TPMS_LDL_FULL_BIT + TPMS_LDL_HALF_BIT; //3u * TPMS_MEMS_HALF_BIT / 2u;
static uint16_t bit_limit_low = TPMS_LDL_HALF_BIT / 2u;
static uint16_t bit_thd = TPMS_LDL_HB_THD;
#endif /* TPMS_USE_DECODER */
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#ifdef TPMS_USE_ENCODER
uint8_t TPMS_LDL_CalculateCRC(uint8_t *buffer, uint16_t len)
{
	uint8_t ldl_checksum_;
	uint16_t i;

	ldl_checksum_ = 0u;
	for ( i=0u; i<len; ++i )
	{
		ldl_checksum_ ^= buffer[i];
	}

	return ldl_checksum_;
}
#endif /* TPMS_USE_ENCODER */

#ifdef TPMS_USE_DECODER
/*static uint16_t TPMS_LDL_FindFrameLenWithCRC(bytesData_t *data)
{
	uint16_t retVal_ 	= 0u;
	uint8_t ldl_checksum_, ldl_checksum2_;

	if ( data->sz > 5 )
	{
		ldl_checksum_ = 0u;
		for ( retVal_ = 0u; retVal_ < (data->sz-1); retVal_++)
		{
			// Read CRC on next byte
			ldl_checksum2_ = data->data[retVal_+1];

			// Calculate CRC on frame
			ldl_checksum_ ^= data->data[retVal_];

			// Check CRC
			if ( ldl_checksum_ == ldl_checksum2_ )
			{ // CRC found
				retVal_ += 2; // To include the CRC byte and add 1 for index 0
				break;
			}
		}


		data->crcC = ldl_checksum_;
		data->crcR = ldl_checksum2_;

		if ( ldl_checksum_ == ldl_checksum2_ )
		{
			data->sz = retVal_;
		}else{
			retVal_ = 0u;
			data->sz = 0u;
		}
	}else{
		data->sz = 0u;
		retVal_ = 0u;
	}

	return retVal_;
}*/

static uint8_t TPMS_LDL_CheckCRC(bytesData_t *data)
{
	uint8_t ldl_checksum_= 0xffu, ldl_checksum2_;
	ldl_checksum2_ = data->data[data->sz-1];
	if(ldl_checksum2_ != 0x00)
	{
		ldl_checksum_ = TPMS_LDL_CalculateCRC(data->data, data->sz-1u);
	}


	data->crcC = ldl_checksum_;
	data->crcR = ldl_checksum2_;

	return (ldl_checksum2_ == ldl_checksum_);
}

static Timing_Types_e TPMS_LDL_SpecifyBit(uint16_t timing, uint16_t l, uint16_t th, uint16_t h)
{
	Timing_Types_e retval = RF_BIT_UNKNOWN;

	if((timing > l) && (timing < th))
	{
		retval = RF_BIT_SHORT;
	}
	else if((timing < h) && (timing > th))
	{
		retval = RF_BIT_LONG;
	}
	else if(timing > h)
	{
		retval = RF_BIT_EXIT ;
	}
	else
	{
		retval = RF_BIT_UNKNOWN;
	}

	return retval;
}

/**
 * @brief  Decode frame from capture structure.
 *
 * @param  captureData struct describing captured data.
 * @param  decodedData pointer on buffer to store decoded data.
 * @param  pLen		   pointer on decoded data len (in input is the max size for decodedData tab.
 * 								in output the decoded data len).
 * @return ldl_TAG if decoded, else NO_TAG.
 */
TagTypes_t TPMS_LDL_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *encodedData)
{
	TagTypes_t ldl_retVal_ = NO_TAG;
	TagDecodeStates_e state = TAG_START;

	uint16_t previous_time = 0u;
	uint16_t current_time = 0u;
	uint16_t next_time = 0u;

	uint16_t ldl_decode_ptr_ = 0u;
	uint8_t  ldl_endOfFrame = 0u;

	uint8_t  ldl_preamble_cnt = 0u;

	uint8_t current_bit = 0u;
	uint8_t current_byte = 0u;
	uint8_t bit_idx = 0u;
	uint8_t bit_valid = 0;

	Timing_Types_e bit_type = RF_BIT_UNKNOWN;
	Timing_Types_e last_bit_type = RF_BIT_UNKNOWN;

	encodedData->sz = 0u;

	while(ldl_endOfFrame == 0u)
	{
		/* Get timing */
		previous_time = current_time;
		current_time = captureData->data[ldl_decode_ptr_];
		next_time = captureData->data[ldl_decode_ptr_ + 1];

		/* Specify bit type */
		bit_type = TPMS_LDL_SpecifyBit(current_time, bit_limit_low, bit_thd, bit_limit_high);
		if((bit_type == RF_BIT_SHORT) && (last_bit_type == RF_BIT_SHORT))
		{
			bit_type = RF_BIT_SHORT2;
		}

		/* Play with bit */
		switch(state)
		{
		case TAG_START:
			/* Search for two consecutive long time : start of preamble 0xFFFE */
			if((last_bit_type == RF_BIT_SHORT) && (bit_type == RF_BIT_SHORT2))
			{
				ldl_preamble_cnt++;
				if(ldl_preamble_cnt == 2)
				{
					state = TAG_PREAMBLE;
					bit_valid = 1u;
				}
			}else if(bit_type == RF_BIT_SHORT)
			{

			}else{
				ldl_preamble_cnt = 0;
			}
			break;

		case TAG_PREAMBLE:
			/* Search for long bit 0x01
			 * cc cc cc cc cc cc cc cc l
			 * 0  0  0  0  0  0  0  0  1
			 */
			if(bit_type == RF_BIT_LONG)
			{
				/* Long byte */
				bit_valid = 1u;
				state = TAG_DATA;
				current_bit = 1u;
			}else if(bit_type == RF_BIT_SHORT)
			{
				bit_valid = 0u;
			}else if(bit_type == RF_BIT_SHORT2)
			{
				/* sync found */
				bit_valid = 1u;
			}else{
			}
			break;

		case TAG_DATA:
			if((bit_type == RF_BIT_EXIT) || ((ldl_decode_ptr_) >= (captureData->sz)))
			{
				state = TAG_CRC;
				continue;
			}

			if(bit_type == RF_BIT_UNKNOWN)
			{
				/* may be a glitch */
				/* glitch */
				if(next_time > current_time)
				{
					/* Tn is a glitch */
					next_time += current_time + previous_time;
					captureData->data[ldl_decode_ptr_ + 1] = next_time;

					if(bit_valid) /* Last bit was valid */
					{
						bit_valid = 0u;

						if(bit_idx > 0)
						{
							/* Modify current byte */
							bit_idx--;
						}else{
							/* Modify last byte */
							if(encodedData->sz > 0)
							{
								current_byte = encodedData->data[encodedData->sz - 1];
								encodedData->sz--;
								bit_idx = 7;
							}
						}
						/* Shift left last bit : wrong bit */
						current_byte = current_byte >> 1u;
					}
				}else{
					/* Tn+1 is a glitch */
					/* Do nothing algorithm will get it */
				}

				bit_valid = 0u;
				bit_type = last_bit_type;

			}else if(bit_type == RF_BIT_LONG)
			{
				current_bit = (current_bit == 0);
				current_byte = current_byte << 1;
				current_byte += current_bit;
				bit_idx++;
				bit_valid = 1u;
			}else if(bit_type == RF_BIT_SHORT)
			{
				bit_valid = 0u;
			}else if(bit_type == RF_BIT_SHORT2)
			{
				current_byte = current_byte << 1;
				current_byte += current_bit;
				bit_idx++;
				bit_valid = 1u;
			}else{
				/* other case RF_BIT_EXIT */
			}

			/* Save byte */
			if(bit_idx == 8u)
			{
				encodedData->data[encodedData->sz] = current_byte;
				encodedData->sz++;
				current_byte = 0u;
				bit_idx = 0;
			}

			break;

		case TAG_CRC:
			ldl_endOfFrame = 1u;
			encodedData->decodesz = encodedData->sz;
			if(encodedData->sz < 9u)
			{
				break;
			}

			encodedData->sz = 9u;
			if (TPMS_LDL_CheckCRC(encodedData) > 0u)
			{
				ldl_retVal_ = LDL_TAG;
				encodedData->type = LDL_TAG;

			}else{
				encodedData->sz = 0u;
				ldl_retVal_ = NO_TAG;
			}
			break;

		default:
			break;
		}


		last_bit_type = bit_type;
		ldl_decode_ptr_++;

		if((state != TAG_DATA) && ((ldl_decode_ptr_) >= (captureData->sz)))
		{
			ldl_endOfFrame = 1u;
		}
	}

	/* Check CRC */
	if (state != TAG_CRC)
	{
		encodedData->sz = 0u;
		ldl_retVal_ = NO_TAG;
	}


	if((ldl_retVal_ == LDL_TAG) && (encodedData->sz < 10))
	{
		__no_operation();
	}
	return ldl_retVal_;





	//	TagTypes_t ldl_retVal_ = NO_TAG;
	//	uint16_t ldl_decode_ptr_ = 0u;
	//	uint8_t  ldl_endOfFrame = 0u;
	//
	//	TagDecodeStates_e state = TAG_START;
	//	uint16_t ldl_start_ptr_ = 0u;
	//	uint8_t current_byte = 0u;
	//	uint8_t bit_state = 0u;
	//	uint8_t bit_idx = 0u;
	//	uint16_t bit_halfbit_value = TPMS_LDL_HB_THD;
	//	uint8_t first_short = 0u;
	//	uint8_t preamble_detect = 0u;
	//
	//	encodedData->sz = 0u;
	//	uint16_t bit_limit_high = TPMS_LDL_FULL_BIT + TPMS_LDL_HALF_BIT; //3u * TPMS_LDL_HALF_BIT / 2u;
	//	uint16_t bit_limit_low = TPMS_LDL_HALF_BIT / 2u;
	//
	//	while(ldl_endOfFrame == 0u)
	//	{
	//
	//		switch(state)
	//		{
	//		case TAG_START:
	//			/* search for first short */
	//			if(
	//					(captureData->data[ldl_decode_ptr_] > (bit_limit_low)) &&
	//					(captureData->data[ldl_decode_ptr_] < (bit_limit_high))
	//			)
	//			{
	//				if(first_short == 1u)
	//				{
	//					state = TAG_PREAMBLE;
	//				}else{
	//					first_short  = 1u;
	//				}
	//			}else{
	//				/* error
	//				 * bit too short
	//				 * bit too long
	//				 */
	//				first_short  = 0u;
	//			}
	//
	//			ldl_decode_ptr_++;
	//			if((ldl_decode_ptr_) >= (captureData->sz))
	//			{
	//				ldl_endOfFrame = 1u;
	//			}
	//			break;
	//
	//		case TAG_PREAMBLE:
	//			/* Search for long bit 0x01
	//			 * cc cc cc cc cc cc cc cc l
	//			 * 0  0  0  0  0  0  0  0  1
	//			 */
	//			if(captureData->data[ldl_decode_ptr_] > bit_halfbit_value) //l
	//			{
	//				/* Long byte counter */
	//				state = TAG_DATA;
	//				bit_state = 1u;
	//				current_byte = 0u;
	//				/*encodedData->data[encodedData->sz] = 0x01;
	//				encodedData->sz++;*/
	//			}
	//			ldl_decode_ptr_++;
	//			if((ldl_decode_ptr_) >= (captureData->sz))
	//			{
	//				ldl_endOfFrame = 1u;
	//			}
	//			break;
	//
	//		case TAG_DATA:
	//			current_byte = current_byte << 1u;
	//
	//			if((captureData->data[ldl_decode_ptr_] < (bit_limit_low)) ||
	//					(captureData->data[ldl_decode_ptr_] > (bit_limit_high)) ||
	//					((ldl_decode_ptr_) >= (captureData->sz)))
	//			{
	//				state = TAG_CRC;
	//			}else if(captureData->data[ldl_decode_ptr_] < TPMS_LDL_HB_THD) //c
	//			{
	//				// skip next value
	//				ldl_decode_ptr_ += 2u;
	//			}
	//			else //l
	//			{
	//				bit_state = (bit_state == 0);
	//				ldl_decode_ptr_++;
	//			}
	//
	//			current_byte += bit_state;
	//			bit_idx++;
	//
	//			if(bit_idx == 8u)
	//			{
	//				encodedData->data[encodedData->sz] = current_byte;
	//				encodedData->sz++;
	//
	//				current_byte = 0u;
	//				bit_idx = 0;
	//			}
	//			break;
	//
	//		case TAG_CRC:
	//			ldl_endOfFrame = 1u;
	//			ldl_retVal_ = NO_TAG;
	//
	//			if(encodedData->sz > 5)
	//			{
	//				if (TPMS_LDL_FindFrameLenWithCRC(encodedData) > 0u)
	//				{
	//					ldl_retVal_ = LDL_TAG;
	//					encodedData->type = LDL_TAG;
	//				}
	//			}else{
	//				encodedData->sz = 0u;
	//			}
	//			break;
	//
	//		default:
	//			break;
	//		}
	//
	//	}
	//
	//	return ldl_retVal_;
}
#endif /* TPMS_USE_DECODER */

#ifdef TPMS_USE_ENCODER
/**
 * @brief  Encode frame to output structure.
 *
 * @param  outputTimer struct describing data to output.
 * @param  ldl_dataToEncode pointer on buffer for encoded data.
 * @param  len		   encoded data len.
 * @return 0.
 */
uint16_t 	TPMS_LDL_EncodeFrameToSend(CaptureData_t *captureData, bytesData_t *encodedData)
{
	uint16_t ldl_encode_ptr = 0u;
	int16_t i = 0u;
	uint8_t current_bit = 1u;
	uint8_t tmp_bit = 1u;

	captureData->sz = 0u;

	encodedData->data[encodedData->sz] = TPMS_LDL_CalculateCRC(encodedData->data, encodedData->sz);
	encodedData->sz++;

	captureData->data[captureData->sz++] = 5 * TPMS_LDL_FULL_BIT;

	// Add the frame sync word 0xFFF2
	/* 00000000 */
	/* 00000001 */
	for(i = 0u; i < 15u; i++)
	{
		captureData->data[captureData->sz++] = TPMS_LDL_HALF_BIT;
		captureData->data[captureData->sz++] = TPMS_LDL_HALF_BIT;
	}

	captureData->data[captureData->sz++] = TPMS_LDL_FULL_BIT; // 1

	current_bit = 1u;

	// Premier bit utile : on decode le manchester, croyez moi ca marche
	while(ldl_encode_ptr < encodedData->sz)
	{
		for(i = 0; i < 8; i++)
		{
			tmp_bit = (encodedData->data[ldl_encode_ptr] >> (7u - i) ) & 0x01u;

			if(tmp_bit != current_bit) // l
			{
				captureData->data[captureData->sz++] = TPMS_LDL_FULL_BIT;
				current_bit ^= 1;
			}
			else // 0
			{
				captureData->data[captureData->sz++] = TPMS_LDL_HALF_BIT;
				captureData->data[captureData->sz++] = TPMS_LDL_HALF_BIT;
			}
		}
		ldl_encode_ptr++;
	}

	captureData->data[captureData->sz++] = 5 * TPMS_LDL_FULL_BIT;

	return 0u;
}
#endif /* TPMS_USE_ENCODER */

#ifdef __cplusplus
}
#endif

#endif /* _TPMS_LDL_C_ */
