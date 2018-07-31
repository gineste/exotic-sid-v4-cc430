/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
* File Name          : es_interfaces.h
* Description        : EXOTIC SYSTEMS Protocol interface.
********************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef __ES_INTERFACES_H__
#define __ES_INTERFACES_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>


/* Exported constants --------------------------------------------------------*/
     
#define ESI_LIBRARY_VERSION     "00.05.00"
     
typedef enum _ESI_RETURNCODE_{
    ESI_Failed          = 0x00u,
    ESI_Succeed         = 0x01u,
    ESI_Overflow        = 0xFEu,
    ESI_BadFrame        = 0xFFu,
}ESI_ReturnCode_t;

/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef uint8_t 	(*ESI_WriteBuffer_f)(uint8_t buffer[], uint16_t len);
typedef uint8_t* 	(*ESI_ReadBuffer_f) (uint16_t len);
typedef uint8_t 	(*ESI_ReleaseBuffer_f) (void);

typedef ESI_ReturnCode_t 	(*ESI_lock_f) (void);
typedef ESI_ReturnCode_t 	(*ESI_unlock_f) (void);

typedef void 		(*ESI_RecvFrame_f)(uint8_t opCode,
                                     uint32_t expdt,
                                     uint32_t recpt,
                                     uint8_t *payload, uint16_t len);

typedef struct _ESI_PACKET_ {
    uint8_t   (*data)[]; 	/*< Buffer */
    uint16_t  len;			/*< Len of data in buffer */
    uint16_t  size;			/*< Max size of buffer */
} ESI_Packet_t;

typedef struct _ESI_IO_ {
    ESI_Packet_t        Out;				/* Buffer with data to send */
    
    ESI_lock_f          lock;
    ESI_unlock_f        unlock;

    ESI_WriteBuffer_f   writeBuffer;     /* Write data on the interface */
    ESI_ReadBuffer_f    readBuffer;      /* Read data on the interface */
    ESI_RecvFrame_f     recvFrame;       /* Receive payload from interface */
    ESI_ReleaseBuffer_f    releaseBuffer;       /* Release buffer in stack from interface */
}ESI_IO_t;

typedef struct _ESI_INTERFACE_ {
	uint16_t           timeOut;
	uint8_t            physicalItf;
	ESI_IO_t           *io;
}ESI_Interface_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void ESI_Init(ESI_Interface_t * interfaces, uint32_t itf_count);
ESI_ReturnCode_t ESI_AddInterface(uint8_t itf, ESI_IO_t * io);
ESI_ReturnCode_t ESI_SendFrame( uint8_t itf,
                                uint8_t opCode,
                                uint32_t expdt,
                                uint32_t recpt,
								const uint8_t payload[], uint16_t len);
ESI_ReturnCode_t ESI_CheckForFrame(uint8_t itf);

#ifdef __cplusplus
}
#endif

#endif /* __ES_INTERFACES_H__ */

