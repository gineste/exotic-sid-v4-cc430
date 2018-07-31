/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
* File Name          : tpms_ldl.h
* Description        : Module pour le decodage/encodage des capteurs TPMS_LDL.
********************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef __TPMS_LDL_H__
#define __TPMS_LDL_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
 typedef struct _TAG_LDL_ {
   uint8_t id[4];              /* Identifier ............................... */
   uint8_t stbabl;             /* Status flag .............................. */
   uint8_t pressure;           /* = P real / 1400 * 255 .................... */
   uint8_t temperature;        /* = T real + 50°C .......................... */
   uint8_t apos;               /* Unknown data ............................. */
   uint8_t crc;                /* crc tag .................................. */
 } s_TPMS_LDL_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
#ifdef TPMS_USE_ENCODER
TagTypes_t 	TPMS_LDL_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *decodedData);
#endif /* TPMS_USE_ENCODER */

#ifdef TPMS_USE_ENCODER
uint8_t     TPMS_LDL_CalculateCRC(uint8_t *buffer, uint16_t len);
uint16_t 	TPMS_LDL_EncodeFrameToSend(CaptureData_t *captureData, bytesData_t *encodedData);
#endif /* TPMS_USE_ENCODER */
#ifdef __cplusplus
}
#endif

#endif /* __TPMS_LDL_H__ */
