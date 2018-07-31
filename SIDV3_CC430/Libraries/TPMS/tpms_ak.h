/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
* File Name          : tpms_ak.h
* Description        : Module pour le decodage/encodage des capteurs TPMS_AK.
********************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef __TPMS_AK_H__
#define __TPMS_AK_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
 typedef struct {
 	uint16_t preamble;
 	uint16_t baudrate;
 	TagTypes_t type;
 } AK_Info_t;

 typedef struct _TAG_AK_ {
  // uint8_t syncLSB;
   uint8_t u8type;
   uint8_t id[4];
   uint8_t pression;					/* pressure not used */
   uint8_t temperature;				/* temperature in °C */
   uint8_t u8crc;
 } s_TPMS_AK_t;
/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
#ifdef TPMS_USE_ENCODER
uint8_t TPMS_AK_CalculateCRC(uint8_t *buffer, uint16_t len, TagTypes_t p_eTag);
uint16_t 	TPMS_AK_EncodeFrameToSend(CaptureData_t *captureData, bytesData_t *encodedData);
#endif /* TPMS_USE_ENCODER */

#ifdef TPMS_USE_DECODER
TagTypes_t 	TPMS_AK_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *decodedData, AK_Info_t * info);
//TagTypes_t  TPMS_REDI_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *encodedData);
//TagTypes_t 	TPMS_SCHRADER_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *encodedData);
#endif /* TPMS_USE_ENCODER */
#ifdef __cplusplus
}
#endif

#endif /* __TPMS_AK_H__ */
