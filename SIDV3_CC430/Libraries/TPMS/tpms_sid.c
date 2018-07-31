/* =============================================================================
 ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
(  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
(____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/

  Copyright (c) 2017 EXOTIC SYSTEMS. All Rights Reserved.

  Licensees are granted free, non-transferable use of the information. NO
  WARRANTY of ANY KIND is provided. This heading must NOT be removed from
  the file.

  File name:    tpms_sid.c
  Date:         14 03 2018
  Author:       Emeric L.
  Description:  SID TPMS decode/encoder - body file.
============================================================================= */

/* =============================================================================
                                 DEBUG Section
============================================================================= */

/* =============================================================================
                                 Include Files
============================================================================= */
#include <stdint.h>
#include "boardconfig.h"
#include "tpms.h"
#include "tpms_sid.h"


/* =============================================================================
                          Private defines and typedefs
============================================================================= */
#define TPMS_SID_BAUDRATE ((uint16_t)1200)
#define TPMS_SID_HALF_BIT TPMS_HB_CALCULATOR(TPMS_SID_BAUDRATE)
#define TPMS_SID_FULL_BIT (2*TPMS_SID_HALF_BIT)
#define TPMS_SID_HB_THD   TPMS_HB_THD_CALCULATOR(TPMS_SID_HALF_BIT)
#define SID_SYNCWORD      ((uint16_t)0xFFA5)


/* =============================================================================
                        Private constants and variables
============================================================================= */


/* =============================================================================
                        Public constants and variables
============================================================================= */


/* =============================================================================
                        Private function declarations
============================================================================= */
static uint8_t TPMS_SID_CalculateCRC (uint8_t p_au8buffer[], uint8_t p_u8BufferLength);

/* =============================================================================
                               Public functions
============================================================================= */

#ifdef TPMS_USE_ENCODER
/* =============================================================================
Function    :   Encode frame to output structure.

Description :   .

Parameters  :   .

Return      :   .
============================================================================= */
uint16_t TPMS_SID_EncodeFrameToSend (CaptureData_t *captureData, bytesData_t *encodedData)
{
  uint16_t ak_encode_ptr = 0u;
  int16_t i = 0u;
  uint8_t current_bit = 1u;
  uint8_t tmp_bit = 1u;
  uint16_t l_u16SyncWord = SID_SYNCWORD;
   //uint8_t last_bit = 1u;

  uint16_t half_bit_timing = TPMS_SID_HALF_BIT;
  uint16_t full_bit_timing = TPMS_SID_FULL_BIT;

  captureData->sz = 0u;

  encodedData->data[encodedData->sz-1] = TPMS_SID_CalculateCRC(encodedData->data, encodedData->sz-1);
  captureData->data[captureData->sz++] = 5 * full_bit_timing;
 
  /* Assuming last bit is 1. */
  if (1 != ((l_u16SyncWord>>15u) & 0x01) )
  {
    captureData->data[captureData->sz++] = full_bit_timing;
  }
  else
  {
    captureData->data[captureData->sz++] = half_bit_timing;
    captureData->data[captureData->sz++] = half_bit_timing;
  }  

  /* Add sync word. */
  for (i = 14; i >= 0; i--)
  {
    if ( ( (l_u16SyncWord>>i) & 0x01u) != (l_u16SyncWord>>(i + 1u) & 0x01u) )
    {
      captureData->data[captureData->sz++] = full_bit_timing;
    }
    else
    {
      captureData->data[captureData->sz++] = half_bit_timing;
      captureData->data[captureData->sz++] = half_bit_timing;
    }
  }
  
  /* data Manchester encode */
  while(ak_encode_ptr < encodedData->sz)
  {
    for(i = 0; i < 8; i++)
    {
      tmp_bit = (encodedData->data[ak_encode_ptr] >> (7u - i) ) & 0x01u;

      if(tmp_bit != current_bit) // l
      {
        captureData->data[captureData->sz++] = full_bit_timing;
        current_bit ^= 1;
      }
      else // 0
      {
        captureData->data[captureData->sz++] = half_bit_timing;
        captureData->data[captureData->sz++] = half_bit_timing;
      }
    }
    ak_encode_ptr++;
  }

  captureData->data[captureData->sz++] = 5 * full_bit_timing;

  return 0u;
}
#endif /* TPMS_USE_ENCODER */


/* =============================================================================
                              Private functions
============================================================================= */

#ifdef TPMS_USE_ENCODER
/* =============================================================================
Function    :   TPMS_SID_CalculateCRC

Description :   Compute frame checksum.

Parameters  :   p_au8buffer = which contain data,
                p_u8BufferLength = length of buffer.

Return      :   Checksum.
============================================================================= */
uint8_t TPMS_SID_CalculateCRC (uint8_t p_au8buffer[], uint8_t p_u8BufferLength)
{
  /* Locals variables declaration. */
  uint8_t l_u8Checksum = 0u;
  uint8_t l_u8Index;

  /* Compute checksum. */
  for (l_u8Index = 0; l_u8Index < p_u8BufferLength; l_u8Index++)
  {
    l_u8Checksum ^= p_au8buffer[l_u8Index];
  }

  return (l_u8Checksum);
}
#endif /* TPMS_USE_ENCODER */

