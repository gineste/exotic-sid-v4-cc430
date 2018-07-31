/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
 * File Name          : tpms_mems.c
 * Description        : Module pour le decodage/encodage des capteurs TPMS_MEMS.
 *******************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef _TPMS_MEMS_C_
#define _TPMS_MEMS_C_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "boardconfig.h"
#include "tpms.h"
#include "tpms_mems.h"

/* Private constants ---------------------------------------------------------*/
#define TPMS_MEMS_BAUDRATE	5000

// wrong timing
#define TPMS_MEMS_HALF_BIT	 TPMS_HB_CALCULATOR(TPMS_MEMS_BAUDRATE)//630 						//308 //461 //308
#define TPMS_MEMS_FULL_BIT	 (2*TPMS_MEMS_HALF_BIT) 	//615 //923 //615

#define TPMS_MEMS_HB_THD	TPMS_HB_THD_CALCULATOR(TPMS_MEMS_HALF_BIT)//(920u)

/* Private macro -------------------------------------------------------------*/


#if defined(TPMS_USE_ENCODER) || defined (TPMS_USE_DECODER)
const uint16_t MEMS_CRCTAB16[256] =          ///< CRC 16-bit polynomial 0x1021
{ 		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
		0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
		0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
		0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
		0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
		0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
		0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
		0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
		0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
		0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
		0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
		0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
		0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
		0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
		0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
		0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
		0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
		0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
		0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
		0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
		0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
		0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
		0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
		0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
		0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};
#endif /* TPMS_USE_ENCODER || TPMS_USE_DECODER*/

/* Private types -------------------------------------------------------------*/


/* Private Variables ---------------------------------------------------------*/
#ifdef TPMS_USE_DECODER
static uint16_t bit_limit_high = TPMS_MEMS_FULL_BIT + TPMS_MEMS_HALF_BIT; //3u * TPMS_MEMS_HALF_BIT / 2u;
static uint16_t bit_limit_low = TPMS_MEMS_HALF_BIT / 2u;
static uint16_t bit_thd = TPMS_MEMS_HB_THD;
#endif /* TPMS_USE_DECODER */
//static uint8_t  l_u8count = 0u;


/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
#ifdef TPMS_USE_ENCODER
uint16_t TPMS_MEMS_CalculateCRC(uint8_t *buffer, uint16_t len)
{
	uint16_t  crc = 0xFFFF;
	uint16_t doInvertMask = 0;
	uint8_t doInvert, i;
	int8_t j;

	for (i=0; i < len; ++i)
	{
		for ( j=7; j >= 0; j-- )
		{
			doInvert = ((buffer[i] >> j) & 0x01u) ^ ((crc >> 15) & 0x01);
			doInvertMask = (doInvert << 12u) | (doInvert << 5u);
			crc = (crc << 1) + doInvert;
			crc ^= doInvertMask;
		}
	}

	return crc;
}

/**
 * @brief  Encode frame to output structure.
 *
 * @param  outputTimer struct describing data to output.
 * @param  mems_dataToEncode pointer on buffer for encoded data.
 * @param  len		   encoded data len.
 * @return 0.
 */
uint16_t 	TPMS_MEMS_EncodeFrameToSend(CaptureData_t *captureData, bytesData_t *encodedData)
{
	uint16_t mems_encode_ptr = 0u;
	int16_t i = 0u;

	captureData->sz = 0u;

	captureData->data[captureData->sz++] = 10 * TPMS_MEMS_FULL_BIT;

	// Add the frame sync word 0xFFFE
	for(i = 0; i < 15u; i++)
	{
		captureData->data[captureData->sz++] = TPMS_MEMS_FULL_BIT;
	}
	captureData->data[captureData->sz++] = TPMS_MEMS_HALF_BIT;
	captureData->data[captureData->sz++] = TPMS_MEMS_HALF_BIT;

	// Premier bit utile : on decode le manchester, croyez moi ca marche
	while(mems_encode_ptr < encodedData->sz)
	{
		for(i = 0; i < 8; i++)
		{
			if((encodedData->data[mems_encode_ptr] & (1u << (7u - i)))) // l
			{
				captureData->data[captureData->sz++] = TPMS_MEMS_FULL_BIT;
			}
			else // 0
			{
				captureData->data[captureData->sz++] = TPMS_MEMS_HALF_BIT;
				captureData->data[captureData->sz++] = TPMS_MEMS_HALF_BIT;
			}
		}
		mems_encode_ptr++;
	}
	captureData->data[captureData->sz++] = 2 * TPMS_MEMS_FULL_BIT;
	return 0u;
}
#endif /* TPMS_USE_ENCODER */

#ifdef TPMS_USE_DECODER
static uint16_t TPMS_MEMS_FindFrameLenWithCRC(bytesData_t *data)
{
	uint16_t retVal_ 	= 0u;
	uint16_t crcMsg, crc;
	uint16_t doInvertMask = 0;
	uint8_t doInvert, i;
	int8_t j;

	if ( data->sz > 2 )
	{
		crc = 0xFFFFu;

		for (i=0; i < (data->sz - 2); ++i)
		{
			crcMsg = data->data[i + 1] * 256 + data->data[i + 2];

			for ( j=7; j >= 0; j-- )
			{
				doInvert = ((data->data[i] >> j) & 0x01u) ^ ((crc >> 15) & 0x01);
				doInvertMask = (doInvert << 12u) | (doInvert << 5u);
				crc = (crc << 1) + doInvert;
				crc ^= doInvertMask;
			}

			if (( crc == crcMsg ) && (i >= 8))
			{
				break;
			}
		}
	}

	retVal_ = i + 2;

	if (( crc != crcMsg ) || ( retVal_ < 10 ))
	{
		data->sz = 0;
		retVal_ = 0u;
	}else{
		data->crcC = crc;
		data->crcR = crcMsg;
		data->sz = retVal_;
		data->type = MEMS_TAG;
	}

	return retVal_;
}

static uint8_t TPMS_MEMS_CheckCRC(bytesData_t *data)
{
	//static uint16_t mems_checksum_ = 0xffu, mems_checksum2_;
	/* mems_checksum2_ */
	data->crcR = data->data[data->sz-2]*256 + data->data[data->sz-1];
	if(data->crcR != 0x00)
	{
		data->crcC = TPMS_MEMS_CalculateCRC(data->data, data->sz-2);
	}

	return (data->crcC == data->crcR);
}

static Timing_Types_e TPMS_MEMS_SpecifyBit(uint16_t timing, uint16_t l, uint16_t th, uint16_t h)
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
 * @return MEMS_TAG if decoded, else NO_TAG.
 */
TagTypes_t TPMS_MEMS_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *encodedData)
{
	TagTypes_t mems_retVal_ = NO_TAG;
	TagDecodeStates_e state = TAG_START;

	uint16_t previous_time = 0u;
	uint16_t current_time = 0u;
	uint16_t next_time = 0u;

	uint16_t mems_decode_ptr_ = 0u;
	uint8_t  mems_endOfFrame = 0u;

	uint8_t current_byte = 0u;
	uint8_t bit_idx = 0u;
	uint8_t bit_valid = 0;

	Timing_Types_e bit_type = RF_BIT_UNKNOWN;
	Timing_Types_e last_bit_type = RF_BIT_UNKNOWN;

	encodedData->sz = 0u;

	while(mems_endOfFrame == 0u)
	{
		/* Get timing */
		previous_time = current_time;
		current_time = captureData->data[mems_decode_ptr_];
		next_time = captureData->data[mems_decode_ptr_ + 1];

		/* Specify bit type */
		bit_type = TPMS_MEMS_SpecifyBit(current_time, bit_limit_low, bit_thd, bit_limit_high);
		if((bit_type == RF_BIT_SHORT) && (last_bit_type == RF_BIT_SHORT))
		{
			bit_type = RF_BIT_SHORT2;
		}

		/* Play with bit */
		switch(state)
		{
		case TAG_START:
			/* Search for two consecutive long time : start of preamble 0xFFFE */
			if((last_bit_type == RF_BIT_LONG) && (bit_type == RF_BIT_LONG))
			{
				state = TAG_PREAMBLE;
				bit_valid = 1u;
			}
			break;

		case TAG_PREAMBLE:
			/* Search for long bit 0xFFFE
			 * l l l l l l l cc
			 * 1 1 1 1 1 1 1 0
			 */
			if(bit_type == RF_BIT_LONG)
			{
				/* Long byte */
				bit_valid = 1u;
			}else if(bit_type == RF_BIT_SHORT)
			{
				bit_valid = 0u;
			}else if(bit_type == RF_BIT_SHORT2)
			{
				/* sync found */
				state = TAG_DATA;
				bit_valid = 1u;
			}else{
			}
			break;

		case TAG_DATA:

			/* DBG_ELA */
			if (bit_type != RF_BIT_UNKNOWN)
			{
				if((bit_type == RF_BIT_EXIT) || ((mems_decode_ptr_) >= (captureData->sz)))
				{
					state = TAG_CRC;
					//l_u8count++;
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
						captureData->data[mems_decode_ptr_ + 1] = next_time;

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
					current_byte = current_byte << 1;
					current_byte += 1;
					bit_idx++;
					bit_valid = 1u;
				}else if(bit_type == RF_BIT_SHORT)
				{
					bit_valid = 0u;
				}else if(bit_type == RF_BIT_SHORT2)
				{
					current_byte = current_byte << 1;
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
			}
			else
			{
				/* Nothing to do */
			}

			break;

		case TAG_CRC:
			mems_endOfFrame = 1u;
			encodedData->decodesz = encodedData->sz;
			if (TPMS_MEMS_FindFrameLenWithCRC(encodedData) > 0u)
			{
				mems_retVal_ = MEMS_TAG;
				encodedData->type = MEMS_TAG;

			}else{
				encodedData->sz = 0u;
				mems_retVal_ = NO_TAG;
			}
			break;

		default:
			break;
		}


		last_bit_type = bit_type;
		mems_decode_ptr_++;

		if((state != TAG_DATA) && ((mems_decode_ptr_) >= (captureData->sz)))
		{
			mems_endOfFrame = 1u;
		}
	}

	/* Check CRC */
	if (state != TAG_CRC)
	{
		encodedData->sz = 0u;
		mems_retVal_ = NO_TAG;
	}


	if((mems_retVal_ == MEMS_TAG) && (encodedData->sz < 10))
	{
		__no_operation();
	}
	return mems_retVal_;
}
#endif /* TPMS_USE_DECODER */


#ifdef __cplusplus
}
#endif

#endif /* _TPMS_MEMS_C_ */
