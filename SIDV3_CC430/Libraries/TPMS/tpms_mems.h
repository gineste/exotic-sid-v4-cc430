/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
* File Name          : tpms_mems.h
* Description        : Module pour le decodage/encodage des capteurs TPMS_MEMS.
********************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef __TPMS_MEMS_H__
#define __TPMS_MEMS_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
//#define TPMS_MEMS_USE_DECODER
#define TPMS_MEMS_USE_ENCODER

/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct _TAG_MEMS_ {
  uint8_t u8Cmd;                    /* Can be 0x01, 0x3B, 0x3C, 0x3F */
  uint8_t au8ID[3];                 /* Serial LSB bytes, see below to deduce TagId */
  uint8_t u8Temperature;            /* temperature */
  uint8_t u8BatteryVoltage;         /* battery */
  uint8_t u8StatusPressionMSB;      /* status + pressure MSB */
  uint8_t u8PressionLSB;            /* pressure LSB */
  uint8_t au8crc[2];                /* crc */
} s_TPMS_MEMS_t;


/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
#ifdef TPMS_USE_DECODER
TagTypes_t 	TPMS_MEMS_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *decodedData);
#endif /* TPMS_MEMS_USE_DECODER */

#ifdef TPMS_USE_ENCODER
uint16_t 	TPMS_MEMS_CalculateCRC(uint8_t *buffer, uint16_t len);
uint16_t 	TPMS_MEMS_EncodeFrameToSend(CaptureData_t *captureData, bytesData_t *encodedData);
#endif /* TPMS_USE_ENCODER */

#ifdef __cplusplus
}
#endif

#endif /* __TPMS_MEMS_H__ */
