/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
* File Name          : es_protocol.c
* Description        : EXOTIC SYSTEMS Protocol for communication with Peripherals.
*******************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef _ES_PROTOCOL_C_
#define _ES_PROTOCOL_C_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <ES_Protocol/ES_Protocol.h>

/* Private constants ---------------------------------------------------------*/
#define ESP_COMMAND_HEADER_LSB ((uint8_t)((ESP_COMMAND_HEADER   )&0x00FFu))
#define ESP_COMMAND_HEADER_MSB ((uint8_t)((ESP_COMMAND_HEADER>>8)&0x00FFu))
#define ESP_HEADER_SIZE        (sizeof(ESP_Header_t))


/* Private macro -------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
#ifdef __TI_COMPILER_VERSION__
typedef struct __attribute__((__packed__)) _ESP_HEADER_
#elif __IAR_SYSTEMS_ICC__
typedef __packed struct _ESP_HEADER_
#elif __ARMCC_VERSION
typedef __packed struct _ESP_HEADER_
#endif
{
    uint8_t                 headerLSB;
    uint8_t                 headerMSB;
#if defined( ESP_USE_ADDR32 )
    uint32_t                expdt;
    uint32_t                recpt;
#elif defined( ESP_USE_ADDR16 )
    uint16_t                expdt;
    uint16_t                recpt;
#elif defined( ESP_USE_ADDR8 )
    uint8_t                expdt;
    uint8_t                recpt;
#else
#error "Please define Address format in ES_Protocol_Config.h"
#endif
    uint16_t                len;
    uint8_t                 opCode;
} ESP_Header_t;

#ifdef __TI_COMPILER_VERSION__
typedef struct __attribute__((__packed__)) _ESP_FRAME_
#elif __IAR_SYSTEMS_ICC__
typedef __packed struct _ESP_FRAME_
#elif __ARMCC_VERSION
typedef __packed struct _ESP_FRAME_
#endif
{
    ESP_Header_t           header;
    uint8_t                 data[1024u];
} ESP_Frame_t;

/* Private Variables ---------------------------------------------------------*/
static uint16_t ESP_StartOfFrame = 0u;

/* Private functions ---------------------------------------------------------*/
static uint8_t ESP_CalculateCRC(const uint8_t buffer[], uint16_t len);

/* Public functions ----------------------------------------------------------*/

/**
 * @brief       Function to calculate CRC.
 * @param       buffer  data
 * @param       len     data length
 * @return      CRC
 */
static uint8_t ESP_CalculateCRC(const uint8_t buffer[], uint16_t len)
{
    uint16_t i;
    uint8_t crc = 0u;
    for ( i=0u; i<len; ++i )
    {
        crc ^= buffer[i];
    }
    return crc;
}

/**
 * @brief Fonction qui analyse le buffer re�u.
 * @param byte la commande recue
 * @return 0x00 if receiving
 * @return 0xFF if failure
 * @return 0x01 if valid frame received
 */
ESP_ReturnCode_t ESP_CheckForValidFrame(uint8_t buffer[], uint16_t len)
{
	ESP_StartOfFrame = 0u;
	ESP_ReturnCode_t ret = ESP_Failed;

	/* Wait for magic */
	for ( ESP_StartOfFrame=0u; ESP_StartOfFrame<(len-1u); ESP_StartOfFrame++ )
	{

		/* Search start of frame */
		if ( (buffer[ESP_StartOfFrame   ] == ESP_COMMAND_HEADER_LSB) &&
				(buffer[ESP_StartOfFrame+1u] == ESP_COMMAND_HEADER_MSB) )
		{
			ret = ESP_Receiving;

#ifdef __TI_COMPILER_VERSION__
#pragma CHECK_MISRA("-10.1")
#endif
			/* @warning Disable MISRA C 2004 rule 10.1 false positive */
			uint8_t * pBuffer_ = &(buffer[ESP_StartOfFrame   ]);

#ifdef __TI_COMPILER_VERSION__
#pragma RESET_MISRA("10.1")
#endif

			ESP_Frame_t *pFrame = (ESP_Frame_t *) pBuffer_;

			uint16_t lenTmp = pFrame->header.len;

#ifdef __big_endian__
			lenTmp = (uint16_t)((uint16_t)(pFrame->header.len >> 8u) | (uint16_t)(pFrame->header.len << 8u));
#endif

			len = len - ESP_StartOfFrame;

			if ( (len >= ESP_HEADER_SIZE) && (len >= lenTmp) )
			{
				/* Check CRC : On the entire frame (including CRC) crc calculated must be zero */
				if ( ESP_CalculateCRC(pBuffer_, lenTmp) == 0u )
				{
#ifdef __big_endian__
					pFrame->header.len = (uint16_t)((uint16_t)(pFrame->header.len >> 8u) | (uint16_t)(pFrame->header.len << 8u));
#endif
					ret = ESP_ValidFrame;
				}
				else
				{
					ret = ESP_Failed;
				}
			}
			break;
		}
	}
	return ret;
}

/**
 * @brief Fonction qui construit une trame.
 * @param pBuffer le buffer de donn�es � remplir
 * @param expdt l'expediteur
 * @param recpt le destinataire
 * @param opCode le code de la commande
 * @param pPayload le pointeur sur la payload
 * @param len la longueur de la payload
 * @return la longueur de la trame construite
 */
uint16_t ESP_BuildCommand(uint8_t *pBuffer, uint32_t expdt, uint32_t recpt,
                         uint8_t opCode, const uint8_t payload[], uint16_t len)
{
    uint16_t i;
    uint16_t lenTmp = 0u;

    ESP_Frame_t *pFrame = (ESP_Frame_t *) pBuffer;
    /* Prepare response header */
    pFrame->header.headerLSB    = ESP_COMMAND_HEADER_LSB;
    pFrame->header.headerMSB    = ESP_COMMAND_HEADER_MSB;
    pFrame->header.expdt        = expdt;
    pFrame->header.recpt        = recpt;
    pFrame->header.len          = ESP_HEADER_SIZE + 1u + len;
    pFrame->header.opCode       = opCode;

    /* Copy payload */
    for ( i=0u; i<len; ++i )
    {
        pFrame->data[i] = payload[i];
    }

    lenTmp = pFrame->header.len;

#ifdef __big_endian__
    pFrame->header.len = (uint16_t)((uint16_t)(pFrame->header.len >> 8u) | (uint16_t)(pFrame->header.len << 8u));
#endif


    /* Add CRC */
    pFrame->data[i]  = ESP_CalculateCRC(pBuffer, lenTmp-1u);

    return lenTmp;
}

/**
 * @brief Fonction qui renvoie la commande recue.
 * @param pBuffer les donn�es recues
 */
uint8_t ESP_GetCommand(const uint8_t buffer[])
{
#ifdef __TI_COMPILER_VERSION__
#pragma CHECK_MISRA("-10.1")
#endif
	/* @warning Disable MISRA C 2004 rule 10.1 false positive */
	const uint8_t * pBuffer_ = &(buffer[ESP_StartOfFrame]);

#ifdef __TI_COMPILER_VERSION__
#pragma RESET_MISRA("10.1")
#endif

	const ESP_Frame_t *pFrame = (const ESP_Frame_t *) pBuffer_;

	return pFrame->header.opCode;
}

/**
 * @brief Fonction qui renvoie la longueur de la commande recue.
 * @param pBuffer les donn�es recues
 */
uint16_t ESP_GetFrameLen(const uint8_t buffer[])
{
#ifdef __TI_COMPILER_VERSION__
#pragma CHECK_MISRA("-10.1")
#endif
	/* @warning Disable MISRA C 2004 rule 10.1 false positive */
	const uint8_t * pBuffer_ = &(buffer[ESP_StartOfFrame]);

#ifdef __TI_COMPILER_VERSION__
#pragma RESET_MISRA("10.1")
#endif

	const ESP_Frame_t *pFrame = (const ESP_Frame_t *) pBuffer_;
    return pFrame->header.len;
}

/**
 * @brief Fonction qui renvoie un pointeur sur la payload.
 * @param pBuffer les donn�es
 * @return la payload
 */
uint8_t* ESP_GetPayload(uint8_t buffer[])
{
#ifdef __TI_COMPILER_VERSION__
#pragma CHECK_MISRA("-10.1")
#endif
	/* @warning Disable MISRA C 2004 rule 10.1 false positive */
	uint8_t * pBuffer_ = &(buffer[ESP_StartOfFrame]);

#ifdef __TI_COMPILER_VERSION__
#pragma RESET_MISRA("10.1")
#endif

    ESP_Frame_t *pFrame = (ESP_Frame_t *) pBuffer_;
    return pFrame->data;
}

/**
 * @brief Fonction qui renvoie la longueur de la payload
 * @param pBuffer les donn�es
 * @return la longueur de la payload
 */
uint16_t ESP_GetPayloadLength(const uint8_t buffer[])
{
#ifdef __TI_COMPILER_VERSION__
#pragma CHECK_MISRA("-10.1")
#endif
	/* @warning Disable MISRA C 2004 rule 10.1 false positive */
	const uint8_t * pBuffer_ = &(buffer[ESP_StartOfFrame]);

#ifdef __TI_COMPILER_VERSION__
#pragma RESET_MISRA("10.1")
#endif

    const ESP_Frame_t *pFrame = (const ESP_Frame_t *) pBuffer_;
    return ( pFrame->header.len - (ESP_HEADER_SIZE + 1u) );
}

/**
 * @brief Fonction qui renvoie le destinataire d'une trame
 * @param pBuffer les donn�es
 * @return le destinataire
 */
uint32_t ESP_GetExpeditor(const uint8_t buffer[])
{
#ifdef __TI_COMPILER_VERSION__
#pragma CHECK_MISRA("-10.1")
#endif
	/* @warning Disable MISRA C 2004 rule 10.1 false positive */
	const uint8_t * pBuffer_ = &(buffer[ESP_StartOfFrame]);

#ifdef __TI_COMPILER_VERSION__
#pragma RESET_MISRA("10.1")
#endif

	const ESP_Frame_t *pFrame = (const ESP_Frame_t *) pBuffer_;
    return pFrame->header.expdt;
}

/**
 * @brief Fonction qui renvoie le destinataire d'une trame
 * @param pBuffer les donn�es
 * @return le destinataire
 */
uint32_t ESP_GetRecipient(const uint8_t buffer[])
{
#ifdef __TI_COMPILER_VERSION__
#pragma CHECK_MISRA("-10.1")
#endif
	/* @warning Disable MISRA C 2004 rule 10.1 false positive */
	const uint8_t * pBuffer_ = &(buffer[ESP_StartOfFrame]);

#ifdef __TI_COMPILER_VERSION__
#pragma RESET_MISRA("10.1")
#endif

	const ESP_Frame_t *pFrame = (const ESP_Frame_t *) pBuffer_;

    return pFrame->header.recpt;
}

#ifdef __cplusplus
}
#endif

#endif /* _ES_PROTOCOL_C_ */
