/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
 * File Name          : tpms.h
 * Description        : Module pour le decodage/encodage des capteurs TPMS.
 ********************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef __TPMS_H__
#define __TPMS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define TPMS_VERSION			"01.02.00"

//#define RF_CLK_FREQ       (BOARD_SMCCLK_FREQ)

#ifndef RF_CLK_FREQ
//#warning "RF_CLK_FREQ set to default"
#define RF_CLK_FREQ (26000000u / 4u)
#endif

#define TPMS_HB_CALCULATOR(b) ( RF_CLK_FREQ / ( 2u * (b) ) )
#define TPMS_HB_THD_CALCULATOR(hb) ( (hb) * 3u / 2u)

#define TPMS_CAPTURE_BUFFER_SIZE 			500
#define WAND_BYTES_BUFFER_SIZE 				150

/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

typedef enum _TIMING_TYPES_ {
    RF_BIT_UNKNOWN  = 0u,
    RF_BIT_SHORT    = 1u,
    RF_BIT_SHORT2   = 2u,
    RF_BIT_LONG     = 3u,
    RF_BIT_ERROR    = 4u,
    RF_BIT_EXIT      = 5u
} Timing_Types_e;

/* Packed is necessary to have TagTypes_t size = 1 */
#ifdef __TI_COMPILER_VERSION__
typedef enum __attribute__((__packed__)) _TAG_TYPE_
#elif __IAR_SYSTEMS_ICC__
typedef __packed enum _TAG_TYPE_
#endif
{
	MEMS_TAG 		= ((uint8_t)0x00u),
	LDL_TAG 		= ((uint8_t)0x01u),
	AK_TAG 			= ((uint8_t)0x02u),
	SCHRADER_TAG 	= ((uint8_t)0x03u),
	SID_TAG 		= ((uint8_t)0x04u),
	REDI_TAG 		= ((uint8_t)0x05u),
	ASK_TAG 		= ((uint8_t)0x06u),
	HUF_TAG	        = ((uint8_t)0x07u),
	MAX_TAG 		= ((uint8_t)0x08u), /* Must be after all tags identifiers and before NO_TAG */
	NO_TAG 			= ((uint8_t)0xFFu)
} TagTypes_t;

typedef enum {
	TAG_START,
	TAG_PREAMBLE,
	TAG_DATA,
	TAG_CRC
} TagDecodeStates_e;

typedef struct _CAPTURE_DATA_ {
	uint8_t ready;
	uint8_t rssi;
	uint8_t overflow;
	uint16_t sz;
	uint16_t cov_cnt;
	uint16_t lastCCR;
	uint16_t data[TPMS_CAPTURE_BUFFER_SIZE];
} CaptureData_t;

#ifdef __TI_COMPILER_VERSION__
typedef struct __attribute__((__packed__)) _DECODED_DATA_
#elif __IAR_SYSTEMS_ICC__
typedef __packed struct _ESP_HEADER_
#endif
{
	TagTypes_t type;
	uint16_t sz;
	uint8_t data[WAND_BYTES_BUFFER_SIZE];

	uint16_t crcC, crcR;
	uint16_t decodesz;
} bytesData_t;

		/* Exported variables --------------------------------------------------------*/

		/* Exported functions --------------------------------------------------------*/

#ifdef TPMS_USE_DECODER
TagTypes_t TPMS_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *encodedData, uint8_t decodeMask);
#endif /* TPMS_USE_DECODER */

#ifdef TPMS_USE_ENCODER
uint16_t 	TPMS_EncodeFrameToSend(CaptureData_t *captureData, bytesData_t *encodedData);
#endif /* TPMS_USE_ENCODER */

#ifdef __cplusplus
}
#endif

#endif /* __TPMS_H__ */
