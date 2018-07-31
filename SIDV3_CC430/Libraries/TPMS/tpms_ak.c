/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
* File Name          : tpms_ak.c
* Description        : Module pour le decodage/encodage des capteurs TPMS_AK.
*******************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef _TPMS_AK_C_
#define _TPMS_AK_C_

#ifdef __cplusplus
extern "C" {
#endif
  
  /* Includes ------------------------------------------------------------------*/
  //#include <cc430f5137.h>
#include <stdint.h>
#include "boardconfig.h"
#include "tpms.h"
#include "tpms_ak.h"
  
  /* Private constants ---------------------------------------------------------*/
#define TPMS_AK_BAUDRATE              19200
#define TPMS_REDI_BAUDRATE            11364
#define TPMS_SCHRADER_BAUDRATE        9600
#define TPMS_SCHRADER_LFTOOL_BAUDRATE 19200
#define TPMS_HUF_BAUDRATE             19200
#define TPMS_SID_BAUDRATE             1200
  
#define TPMS_AK_HALF_BIT              TPMS_HB_CALCULATOR(TPMS_AK_BAUDRATE) //140u
#define TPMS_AK_FULL_BIT              (2*TPMS_AK_HALF_BIT)
  
#define TPMS_REDI_HALF_BIT            TPMS_HB_CALCULATOR(TPMS_REDI_BAUDRATE) //1000u
#define TPMS_REDI_FULL_BIT            (2*TPMS_REDI_HALF_BIT)
  
#define TPMS_SCHRADER_HALF_BIT        TPMS_HB_CALCULATOR(TPMS_SCHRADER_BAUDRATE)
#define TPMS_SCHRADER_FULL_BIT        (2*TPMS_SCHRADER_HALF_BIT)
  
#define TPMS_SCHRADER_LF_HALF_BIT     TPMS_HB_CALCULATOR(TPMS_SCHRADER_LFTOOL_BAUDRATE)
#define TPMS_SCHRADER_LF_FULL_BIT     (2*TPMS_SCHRADER_LF_HALF_BIT)
  
#define TPMS_HUF_HALF_BIT             TPMS_HB_CALCULATOR(TPMS_HUF_BAUDRATE) //140u
#define TPMS_HUF_FULL_BIT             (2*TPMS_HUF_HALF_BIT)

#define TPMS_SID_HALF_BIT             TPMS_HB_CALCULATOR(TPMS_SID_BAUDRATE) //140u
#define TPMS_SID_FULL_BIT             (2*TPMS_SID_HALF_BIT)


  /* Threashold for detection */
//#define TPMS_NUMBER_OF_TAGS  3u
#define TPMS_AK_HB_THD          TPMS_HB_THD_CALCULATOR(TPMS_AK_HALF_BIT) //(245u)
#define TPMS_REDI_HB_THD        TPMS_HB_THD_CALCULATOR(TPMS_REDI_HALF_BIT) //(450u) 
#define TPMS_SCHRADER_HB_THD    TPMS_HB_THD_CALCULATOR(TPMS_SCHRADER_HALF_BIT) //(245u)
#define TPMS_SCHRADER_LF_HB_THD TPMS_HB_THD_CALCULATOR(TPMS_SCHRADER_LF_HALF_BIT) //(500u)
#define TPMS_HUF_HB_THD         TPMS_HB_THD_CALCULATOR(TPMS_HUF_HALF_BIT) //(245u)
#define TPMS_SID_HB_THD         TPMS_HB_THD_CALCULATOR(TPMS_SID_HALF_BIT) 

  
  /* Private macro -------------------------------------------------------------*/
  
  /* Private types -------------------------------------------------------------*/
  
  /* Private Variables ---------------------------------------------------------*/
  
  /* Private functions ---------------------------------------------------------*/
  
  /* Public functions ----------------------------------------------------------*/
#ifdef TPMS_USE_ENCODER
   uint8_t TPMS_AK_CalculateCRC(uint8_t *buffer, uint16_t len, TagTypes_t p_eTag)
  {
    uint8_t ak_checksum_[8];
    uint8_t ak_current_bit_;
    int16_t i,j;
    uint8_t DoInvert = 0u;
    
    if ((p_eTag == HUF_TAG) || (p_eTag == SID_TAG) )
    {
        ak_checksum_[7] = 0u;
        ak_checksum_[6] = 0u;
        ak_checksum_[5] = 0u;
        ak_checksum_[4] = 0u;
        ak_checksum_[3] = 0u;
        ak_checksum_[2] = 0u;
        ak_checksum_[1] = 0u;
        ak_checksum_[0] = 1u;
    }
    else
    {
    ak_checksum_[7] = 1u;
    ak_checksum_[6] = 0u;
    ak_checksum_[5] = 1u;
    ak_checksum_[4] = 0u;
    ak_checksum_[3] = 1u;
    ak_checksum_[2] = 0u;
    ak_checksum_[1] = 1u;
    ak_checksum_[0] = 0u;
    }

    for ( i=0; i < len; ++i )
    {
      for ( j=7; j >= 0; j-- )
      {
        ak_current_bit_ = (buffer[i] >> j) & 0x01u;
        
        DoInvert = (ak_current_bit_) ^ ak_checksum_[7];  // XOR required?
        
        ak_checksum_[7] = ak_checksum_[6];
        ak_checksum_[6] = ak_checksum_[5];
        ak_checksum_[5] = ak_checksum_[4] ^ DoInvert;
        ak_checksum_[4] = ak_checksum_[3];
        ak_checksum_[3] = ak_checksum_[2] ^ DoInvert;
        ak_checksum_[2] = ak_checksum_[1] ^ DoInvert;
        ak_checksum_[1] = ak_checksum_[0] ^ DoInvert;
        ak_checksum_[0] = DoInvert;
      }
    }
    ak_checksum_[0] += ((ak_checksum_[1] * 2u) + (ak_checksum_[2] * 4u) + (ak_checksum_[3] * 8u)
      + (ak_checksum_[4] * 16u) + (ak_checksum_[5] * 32u) + (ak_checksum_[6] * 64u)
                          + (ak_checksum_[7] * 128u));
    
    return ak_checksum_[0];
  }
#endif /* TPMS_USE_ENCODER */

#ifdef TPMS_USE_DECODER
  static uint16_t TPMS_AK_FindFrameLenWithCRC(bytesData_t *data)
  {
    uint8_t ak_crc_[8];
    uint8_t ak_current_bit_;
    int16_t i,j;
    uint8_t DoInvert = 0u;
    uint16_t retVal_   = 0u;
    uint8_t ak_checksum_, ak_checksum2_;
    
    ak_crc_[7] = 1u;
    ak_crc_[6] = 0u;
    ak_crc_[5] = 1u;
    ak_crc_[4] = 0u;
    ak_crc_[3] = 1u;
    ak_crc_[2] = 0u;
    ak_crc_[1] = 1u;
    ak_crc_[0] = 0u;
    
    if ( data->sz > 2u )
    {
      ak_checksum_ = 0u;
      /* Do not use first byte : end of sync word */
      for ( i = 1u; i < data->sz-1; ++i )
      {
        // Read CRC on next byte
        ak_checksum2_ = data->data[i+1];
        
        // Calculate CRC on frame
        for ( j=7; j >= 0; j-- )
        {
          ak_current_bit_ = (data->data[i] >> j) & 0x01u;
          
          DoInvert = (ak_current_bit_) ^ ak_crc_[7];  // XOR required?
          
          ak_crc_[7] = ak_crc_[6];
          ak_crc_[6] = ak_crc_[5];
          ak_crc_[5] = ak_crc_[4] ^ DoInvert;
          ak_crc_[4] = ak_crc_[3];
          ak_crc_[3] = ak_crc_[2] ^ DoInvert;
          ak_crc_[2] = ak_crc_[1] ^ DoInvert;
          ak_crc_[1] = ak_crc_[0] ^ DoInvert;
          ak_crc_[0] = DoInvert;
        }
        
        ak_checksum_ = (ak_crc_[0] + (ak_crc_[1] * 2u) + (ak_crc_[2] * 4u) + (ak_crc_[3] * 8u)
                        + (ak_crc_[4] * 16u) + (ak_crc_[5] * 32u) + (ak_crc_[6] * 64u)
                          + (ak_crc_[7] * 128u));
        
        // Check CRC
        if ( ak_checksum_ == ak_checksum2_ )
        { // CRC found
          retVal_ = i + 1u; // To include the CRC byte
          break;
        }
      }
      
      data->crcC = ak_checksum_;
      data->crcR = ak_checksum2_;
      
      if ( ak_checksum_ == ak_checksum2_ )
      {
        data->sz = retVal_;
      }else{
        retVal_ = 0u;
      }
      
    }
    
    return retVal_;
  }
  
  static uint8_t TPMS_AK_CheckCRC(bytesData_t *data)
  {
    uint8_t ak_checksum_, ak_checksum2_;
    ak_checksum2_ = data->data[data->sz-1];
    
    if((data->sz > 2) && (ak_checksum2_ != 0x00))
    {
      ak_checksum_ = TPMS_AK_CalculateCRC(data->data+1, data->sz-2u);
    }
    
    data->crcC = ak_checksum_;
    data->crcR = ak_checksum2_;
    
    return (ak_checksum2_ == ak_checksum_);
  }
  
  /**
  * @brief  Decode frame from capture structure.
  *
  * @param  captureData struct describing captured data.
  * @param  decodedData pointer on buffer to store decoded data.
  * @param  pLen       pointer on decoded data len (in input is the max size for decodedData tab.
  *                 in output the decoded data len).
  * @return ak_TAG if decoded, else NO_TAG.
  */
  TagTypes_t TPMS_AK_DecodeFrameReceived(CaptureData_t *captureData, bytesData_t *encodedData, AK_Info_t * info)
  {
    TagTypes_t ak_retVal_     = NO_TAG;
    TagDecodeStates_e state = TAG_START;
    
    uint16_t ak_decode_ptr_ = 0u;
    uint8_t ak_endOfFrame = 0u;
    uint8_t current_byte = 0u;
    uint8_t bit_state = 1u;
    
    uint8_t bit_idx = 0u;
    
    uint8_t first_short = 0u;
    
    /* start + preamble : 0xFFF2 ou 0xFFFE ou 0x0001 */
    /* cmd = AK (0x00 0x80) REDI(0xCC 0x10 0xE0) SCHRADER(Others) */
    /* len = AK 0x00 = 7
    * len = AK 0x80 = 10 -> REDI
    * len = RE 0x10 = 11
    * len = RE 0xCC = 10
    * len = RE 0xE0 = 9
    * len = SCHR = 9
    * Decode de tout le buffer puis check
    */
    encodedData->sz = 0u;
    bit_state = 1u;
    
    uint16_t bit_hb = TPMS_HB_CALCULATOR(info->baudrate);
    uint16_t bit_hb_thd = TPMS_HB_THD_CALCULATOR(bit_hb);
    uint16_t bit_limit_high = 3u * bit_hb;
    uint16_t bit_limit_low = bit_hb / 2u;
    
    bit_state = ((info->preamble >> 15u) & 0x01u);
    
    
    while(ak_endOfFrame == 0u)
    {
      switch(state)
      {
      case TAG_START:
        /* Search for two consecutive short time : start of preamble 0xFFF2 */
        /* */
        if(
           (captureData->data[ak_decode_ptr_] > (bit_limit_low)) &&
             (captureData->data[ak_decode_ptr_] < (bit_hb_thd))
               )
        {
          if(first_short == 1u)
          {
            state = TAG_PREAMBLE;
          }else{
            first_short  = 1u;
          }
        }else{
          /* error
          * bit too short
          * bit too long
          */
          first_short  = 0u;
        }
        
        ak_decode_ptr_++;
        if((ak_decode_ptr_) >= (captureData->sz))
        {
          ak_endOfFrame = 1u;
        }
        break;
        
      case TAG_PREAMBLE:
        /* Search for long bit 0xF2
        * cc cc cc cc l cc l l
        * 1  1  1  1  0 0  1 0
        * 0xFE or 0x01
        * cc cc cc cc cc cc cc l
        * 1  1  1  1  1  1  1  0
        * 0  0  0  0  0  0  0  1
        */
        if(captureData->data[ak_decode_ptr_] > bit_hb_thd) //l
        {
          /* Long byte found */
          if(info->preamble == 0xFFF2)
          {
            /* Next time need to be cc l l */
            if((captureData->data[ak_decode_ptr_ + 1u] < TPMS_AK_HB_THD) && // c
               (captureData->data[ak_decode_ptr_ + 2u] < TPMS_AK_HB_THD) && // c
                 (captureData->data[ak_decode_ptr_ + 3u] > TPMS_AK_HB_THD) && // l
                   (captureData->data[ak_decode_ptr_ + 4u] > TPMS_AK_HB_THD))   // l
            {
              /* 0010 */
              state = TAG_DATA;
              current_byte = 0u;
              bit_state = 0u;
              
              encodedData->data[encodedData->sz] = 0xF2;
              encodedData->sz++;
              ak_decode_ptr_ += 4u;
            }else{
              /* error */
            }
          }else{
            /* 0001 or 1110*/
            state = TAG_DATA;
            current_byte = 0u;
            
            bit_state = (info->preamble & 0x01u);
            encodedData->data[encodedData->sz] = (uint8_t) (info->preamble & 0xFFu);
            encodedData->sz++;
          }
        }else{
          // short time
          //ak_decode_ptr_++;
        }
        
        ak_decode_ptr_++;
        if((ak_decode_ptr_) >= (captureData->sz))
        {
          ak_endOfFrame = 1u;
        }
        
        break;
        
      case TAG_DATA:
        /* 1 bit shift left : prepare for current bit */
        current_byte = current_byte << 1u;
        
        if((captureData->data[ak_decode_ptr_] < (bit_limit_low)) ||
           (captureData->data[ak_decode_ptr_] > (bit_limit_high)) ||
             ((ak_decode_ptr_) >= (captureData->sz)))
        {
          /* Timing error : maybe end of frame or error, so check CRC */
          state = TAG_CRC;
        }else if(captureData->data[ak_decode_ptr_] < bit_hb_thd) //c
        {
          // skip next value
          ak_decode_ptr_ += 2u;
        }
        else //l
        {
          bit_state = (bit_state == 0);
          ak_decode_ptr_++;
        }
        
        current_byte += bit_state;
        
        bit_idx++;
        
        if(bit_idx == 8u)
        {
          /* Complete byte */
          encodedData->data[encodedData->sz] = current_byte;
          encodedData->sz++;
          current_byte = 0u;
          bit_idx = 0;
        }
        break;
        
      case TAG_CRC:
        ak_endOfFrame = 1u;
        if(info->type == REDI_TAG)
        {
          if(encodedData->sz >= 10)
          {
            switch(encodedData->data[1])
            {
            case 0xCC:
              ak_retVal_ = REDI_TAG;
              break;
            case 0x10:
            case 0xE0:
              ak_retVal_ = REDI_TAG;
              break;
              
            default:
              break;
            }
          }else{
            ak_retVal_ = NO_TAG;
          }
        }else if((encodedData->sz >= 5) && (TPMS_AK_CheckCRC(encodedData) == 1u))
        {
          switch(info->type)
          {
          case AK_TAG:
            if(encodedData->data[1] == 0x00)
            {
              ak_retVal_ = AK_TAG;
            }else{
              ak_retVal_ = NO_TAG;
            }
            break;
            
          case SID_TAG:
            if(encodedData->data[1] == 0xCF)
            {
              ak_retVal_ = SID_TAG;
            }else{
              ak_retVal_ = NO_TAG;
            }
            break;
            
          case REDI_TAG:
            if(encodedData->data[1] == 0x80)
            {
              ak_retVal_ = REDI_TAG;
            }else{
              ak_retVal_ = NO_TAG;
            }
            break;
            
          case SCHRADER_TAG:
            if((encodedData->data[0] == 0x01) || (encodedData->data[1] == 0xC1))
            {
              ak_retVal_ = SCHRADER_TAG;
            }else{
              ak_retVal_ = NO_TAG;
            }
            break;
            
          default: 
              ak_retVal_ = NO_TAG;
            break;
          }
        }else{
          if(encodedData->sz >= 5)
          {
            //__no_operation();
          }
        }
        break;
        
      default:
        break;
      }
    }
    
    encodedData->type = ak_retVal_;
    
    return ak_retVal_;
  }
#endif /* TPMS_USE_DECODER */


#ifdef TPMS_USE_ENCODER
  /**
  * @brief  Encode frame to output structure.
  *
  * @param  outputTimer struct describing data to output.
  * @param  ak_dataToEncode pointer on buffer for encoded data.
  * @param  len       encoded data len.
  * @return 0.
  */
uint16_t   TPMS_AK_EncodeFrameToSend(CaptureData_t *captureData, bytesData_t *encodedData)
{
  uint16_t ak_encode_ptr = 0u;
  int16_t i = 0u;
  uint8_t current_bit = 1u;
  uint8_t tmp_bit = 1u;
  //uint8_t last_bit = 1u;
  
  uint16_t half_bit_timing = 0u;
  uint16_t full_bit_timing = 0u;
  
  switch(encodedData->type)
  {
    case AK_TAG:
      half_bit_timing = TPMS_AK_HALF_BIT;
      full_bit_timing = TPMS_AK_FULL_BIT;
      break;
    case REDI_TAG:
      half_bit_timing = TPMS_REDI_HALF_BIT;
      full_bit_timing = TPMS_REDI_FULL_BIT;
      break;
    case SCHRADER_TAG:
      half_bit_timing = TPMS_SCHRADER_HALF_BIT;
      full_bit_timing = TPMS_SCHRADER_FULL_BIT;
      break;
    case HUF_TAG:
      half_bit_timing = TPMS_HUF_HALF_BIT;
      full_bit_timing = TPMS_HUF_FULL_BIT;
      break;
    case SID_TAG:
      half_bit_timing = TPMS_SID_HALF_BIT;
      full_bit_timing = TPMS_SID_FULL_BIT;
      break;

    default:
      break;
  }
    
  captureData->sz = 0u;
  
  encodedData->data[encodedData->sz-1] = TPMS_AK_CalculateCRC(encodedData->data, encodedData->sz-1, encodedData->type);
  
  captureData->data[captureData->sz++] = 5 * full_bit_timing;
  
  if (encodedData->type != SID_TAG)
  {
    /* Add the frame sync word */
    for(i = 0u; i < 12u; i++)
    {
      captureData->data[captureData->sz++] = half_bit_timing;
      captureData->data[captureData->sz++] = half_bit_timing;
    }
    
  if ((encodedData->type == AK_TAG) || (encodedData->type == HUF_TAG))
  {
    /* 0xFFF2 */
    /* 0b1111111111110010 */
    captureData->data[captureData->sz++] = full_bit_timing; // 0
    
    captureData->data[captureData->sz++] = half_bit_timing; // 0
    captureData->data[captureData->sz++] = half_bit_timing;
    
    captureData->data[captureData->sz++] = full_bit_timing; // 1
    captureData->data[captureData->sz++] = full_bit_timing; // 0
    
    current_bit = 0u;
    }else{
      /* 0x0001 */
      /* 0b0000000000001 */
      captureData->data[captureData->sz++] = half_bit_timing; // 0
      captureData->data[captureData->sz++] = half_bit_timing;
      captureData->data[captureData->sz++] = half_bit_timing; // 0
      captureData->data[captureData->sz++] = half_bit_timing;
      captureData->data[captureData->sz++] = half_bit_timing; // 0
      captureData->data[captureData->sz++] = half_bit_timing;
      captureData->data[captureData->sz++] = full_bit_timing; // 1
      
      current_bit = 1u;
    }
  }
  else
  {
    /* Add the frame sync word */
    for(i = 0u; i < 8u; i++)
    {
      captureData->data[captureData->sz++] = half_bit_timing;
      captureData->data[captureData->sz++] = half_bit_timing;
    }
    
    /* 0xFFA5 */
    captureData->data[captureData->sz++] = half_bit_timing; // 1
    captureData->data[captureData->sz++] = half_bit_timing;
    captureData->data[captureData->sz++] = full_bit_timing; // 0
    captureData->data[captureData->sz++] = full_bit_timing; // 1
    captureData->data[captureData->sz++] = full_bit_timing; // 0
    captureData->data[captureData->sz++] = half_bit_timing; // 0
    captureData->data[captureData->sz++] = half_bit_timing;
    captureData->data[captureData->sz++] = full_bit_timing; // 1
    captureData->data[captureData->sz++] = full_bit_timing; // 0
    captureData->data[captureData->sz++] = full_bit_timing; // 1      
  }

  /* data manchester encode */
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
  
#ifdef __cplusplus
}
#endif

#endif /* _TPMS_AK_C_ */
