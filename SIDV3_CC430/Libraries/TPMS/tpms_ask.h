/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
* File Name          : tpms_ask.h
* Description        : Module pour le decodage/encodage des capteurs TPMS_LDL.
********************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef _TPMS_ASK_H_
#define _TPMS_ASK_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
#ifdef TPMS_USE_DECODER
TagTypes_t 	TPMS_ASK_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *decodedData);
#endif /* TPMS_USE_DECODER */

#ifdef __cplusplus
}
#endif

#endif /* _TPMS_ASK_H_ */
