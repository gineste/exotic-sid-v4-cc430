/* =============================================================================
 ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
(  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
(____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/

================================================================================
                                    Source file
================================================================================
Files       :   dma.c

Target      :   CC430F5137 (Code composer studio v6.1.0.00104).

Description :   DMA Driver. 
                If used external DMA ISR define DMA_USE_EXTERNAL_ISR in project
                settings.
================================================================================
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 06/06/17  | E.LANDEL        | Module creation.
================================================================================
Functions :
static eDMA_ErrorCodes_t eDMA_SetTrigger (eDMA_Channel_t p_eChannel, 
                                          eDMA_Trigger_t p_eTrigger);
static eDMA_ErrorCodes_t eDMA_SetTransferMode (eDMA_Channel_t p_eChannel, 
                                    eDMA_TransferMode_t p_eTransferMode);
static eDMA_ErrorCodes_t eDMA_SetDestInc (eDMA_Channel_t p_eChannel, 
                             eDMA_DestIncrement_t p_eDestIncrement);
static eDMA_ErrorCodes_t eDMA_SetSrcInc (eDMA_Channel_t p_eChannel, 
                              eDMA_SrcIncrement_t p_eSrcIncrement);
static eDMA_ErrorCodes_t eDMA_SetDestByte (eDMA_Channel_t p_eChannel, 
                                        eDMA_Destbyte_t p_eDestbyte);
static eDMA_ErrorCodes_t eDMA_SetSrcByte (eDMA_Channel_t p_eChannel, 
                                         eDMA_Srcbyte_t p_eSrcbyte);
static eDMA_ErrorCodes_t eDMA_SetLevel (eDMA_Channel_t p_eChannel, 
                                           eDMA_Level_t p_eLevel);
static eDMA_ErrorCodes_t eDMA_SetDstAddr (eDMA_Channel_t p_eChannel,
                                              uint32_t p_u32AdrDst);
static eDMA_ErrorCodes_t eDMA_SetSourceAddr (eDMA_Channel_t p_eChannel, 
                                                 uint32_t p_u32AdrSrc);
static eDMA_ErrorCodes_t eDMA_SetTransferSize (eDMA_Channel_t p_eChannel, 
                                             uint16_t p_u16TransferSize);

extern eDMA_ErrorCodes_t eDMA_Init (eDMA_Channel_t p_eChannel);
extern eDMA_ErrorCodes_t eDMA_Configure (sDMAx_Configuration_t *p_psConfiguration);
extern eDMA_ErrorCodes_t eDMA_SetState (eDMA_Channel_t p_eChannel, 
                                           eDMA_State_t p_eState);
extern eDMA_ErrorCodes_t eDMA_SetInterruptState (eDMA_Channel_t p_eChannel, 
                                             eDMA_InterruptState_t eState);
extern eDMA_ErrorCodes_t eDMA_SetRequest (eDMA_Channel_t p_eChannel, 
                                         eDMARequestState_t eState);
extern eDMA_ErrorCodes_t eDMA_IsrInstall (eDMA_Channel_t p_eChannel, 
                                            pfDMA_isr_t p_pfuncIsr);
extern eDMA_ErrorCodes_t eDMA_IsrUninstall (eDMA_Channel_t p_eChannel); 
============================================================================= */

/* === General statements === */
#include <msp430.h>
//#include <cc430f5137.h>
#include <stddef.h>
#include <stdint.h>

/* === Used Interfaces ===*/

/* === Attached interface  === */
#include "dma.h"

/* =============================== DEFINITIONS ============================== */
/* === Defines === */
#define DMA_DMA0CTL_DEFAULT_VALUE (0x0000) /* SLAU259E - Table 7-4. DMA Registers */
#define DMA_DMA1CTL_DEFAULT_VALUE (0x0000) /* SLAU259E - Table 7-4. DMA Registers */
#define DMA_DMA2CTL_DEFAULT_VALUE (0x0000) /* SLAU259E - Table 7-4. DMA Registers */

/* === Typedef === */

/* === Constants === */
/* Text */
/* 8 bits */
/* 16 bits */
/* 32 bits */
/* === Macros === */

/* === LOCALS Variables === */
/* Text */
/* 8 bits */
/* 16 bits */
/* 32 bits */
#ifndef DMA_USE_EXTERNAL_ISR
static pfDMA_isr_t apfDMA_Isr[DMA_CHANNEL_NBR] =
{
  NULL,
  NULL,
  NULL
};
#endif /* DMA_USE_EXTERNAL_ISR */

/* === EXPORTED Variables === */
/* Text */
/* 8 bits */
/* 16 bits */
/* 32 bits */

/* === DEBUG Section === */
#pragma diag_suppress=770

/* ============================ LOCALS FUNCTIONS ============================ */
static eDMA_ErrorCodes_t eDMA_SetTrigger (eDMA_Channel_t p_eChannel, 
                                         eDMA_Trigger_t p_eTrigger);
static eDMA_ErrorCodes_t eDMA_SetTransferMode (eDMA_Channel_t p_eChannel, 
                                    eDMA_TransferMode_t p_eTransferMode);
static eDMA_ErrorCodes_t eDMA_SetDestInc (eDMA_Channel_t p_eChannel, 
                             eDMA_DestIncrement_t p_eDestIncrement);
static eDMA_ErrorCodes_t eDMA_SetSrcInc (eDMA_Channel_t p_eChannel, 
                              eDMA_SrcIncrement_t p_eSrcIncrement);
static eDMA_ErrorCodes_t eDMA_SetDestByte (eDMA_Channel_t p_eChannel, 
                                        eDMA_Destbyte_t p_eDestbyte);
static eDMA_ErrorCodes_t eDMA_SetSrcByte (eDMA_Channel_t p_eChannel, 
                                         eDMA_Srcbyte_t p_eSrcbyte);
static eDMA_ErrorCodes_t eDMA_SetLevel (eDMA_Channel_t p_eChannel, 
                                           eDMA_Level_t p_eLevel);
static eDMA_ErrorCodes_t eDMA_SetDstAddr (eDMA_Channel_t p_eChannel, 
                                              uint32_t p_u32AdrDst);
static eDMA_ErrorCodes_t eDMA_SetSourceAddr (eDMA_Channel_t p_eChannel, 
                                                 uint32_t p_u32AdrSrc);
static eDMA_ErrorCodes_t eDMA_SetTransferSize (eDMA_Channel_t p_eChannel, 
                                             uint16_t p_u16TransferSize);

/* =============================================================================
Function    :   eDMA_SetTrigger

Description :   Set trigger source.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eTrigger = see eDMA_Trigger_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_TRIGGER.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetTrigger (eDMA_Channel_t p_eChannel,
                                          eDMA_Trigger_t p_eTrigger)
{
  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_TRIGGER;

  /* Valid trigger */
  if (DMA_TRIGGER_NBR > p_eTrigger)
  {
    switch (p_eChannel)
    {
      case DMA_CHANNEL_0:

        /* Set new value */
        DMACTL0 &= (uint16_t)(~(DMA_TRIGGER_NBR - 1u));
        DMACTL0 |= (uint16_t)p_eTrigger;

        l_eErrorCode = DMA_ERROR_NO;

      break;

      case DMA_CHANNEL_1:

        /* Set new value */
        DMACTL0 &= (uint16_t)(~((DMA_TRIGGER_NBR - 1u) * 0x0100u));
        DMACTL0 |= (uint16_t)(p_eTrigger * 0x0100u);

        l_eErrorCode = DMA_ERROR_NO;

      break;

      case DMA_CHANNEL_2:

        /* Set new value */
        DMACTL1 &= (uint16_t)(~(DMA_TRIGGER_NBR - 1));
        DMACTL1 |= (uint16_t)p_eTrigger;

        l_eErrorCode = DMA_ERROR_NO;

      break;

      default:

        /* Nothing to do */

      break;      
    }
  }
  else
  {
    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetTransferMode

Description :   Set transfer mode.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eTransferMode = See eDMA_TransferMode_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_TRANSFER_MODE.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetTransferMode (eDMA_Channel_t p_eChannel,
                                     eDMA_TransferMode_t p_eTransferMode)
{
  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_TRANSFER_MODE;

  /* If valid transfer mode */
  if (DMA_DT_NBR > p_eTransferMode)
  {
    switch (p_eChannel)
    {
      case DMA_CHANNEL_0:

        /* Set new value */
        DMA0CTL &= (uint16_t)(~((DMA_DT_NBR-1)*DMADT0));
        DMA0CTL |= (uint16_t)(p_eTransferMode*DMADT0);

        l_eErrorCode = DMA_ERROR_NO;
      
      break;

      case DMA_CHANNEL_1:

        /* Set new value */
        DMA1CTL &= (uint16_t)(~((DMA_DT_NBR-1)*DMADT0));
        DMA1CTL |= (uint16_t)(p_eTransferMode*DMADT0);

        l_eErrorCode = DMA_ERROR_NO;
        
      break;

      case DMA_CHANNEL_2:

        /* Set new value */
        DMA2CTL &= (uint16_t)(~((DMA_DT_NBR-1)*DMADT0));
        DMA2CTL |= (uint16_t)(p_eTransferMode*DMADT0);

        l_eErrorCode = DMA_ERROR_NO;
        
      break;

      default:
      
        /* Nothing to do */

      break;      
    }
  }
  else
  {
    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetDestInc

Description :   Set destination increment.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eDestIncrement = see eDMA_DestIncrement_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_DESTINATION_INCREMENT.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetDestInc (eDMA_Channel_t p_eChannel, eDMA_DestIncrement_t p_eDestIncrement) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_DESTINATION_INCREMENT;

  /* If valid transfer mode */
  if (DMA_DSTINCR_NBR > p_eDestIncrement) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        DMA0CTL &= (uint16_t)(~((DMA_DSTINCR_NBR-1)*DMADSTINCR0));
        DMA0CTL |= (uint16_t)(p_eDestIncrement*DMADSTINCR0);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        DMA1CTL &= (uint16_t)(~((DMA_DSTINCR_NBR-1)*DMADSTINCR0));
        DMA1CTL |= (uint16_t)(p_eDestIncrement*DMADSTINCR0);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        DMA2CTL &= (uint16_t)(~((DMA_DSTINCR_NBR-1)*DMADSTINCR0));
        DMA2CTL |= (uint16_t)(p_eDestIncrement*DMADSTINCR0);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetSrcInc

Description :   Set source increment.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eSrcIncrement = eDMA_SrcIncrement_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_SOURCE_INCREMENT.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetSrcInc (eDMA_Channel_t p_eChannel, eDMA_SrcIncrement_t p_eSrcIncrement) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_SOURCE_INCREMENT;

  /* If valid transfer mode */
  if (DMA_SRCINCR_NBR > p_eSrcIncrement) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        DMA0CTL &= (uint16_t)(~((DMA_SRCINCR_NBR-1)*DMASRCINCR0));
        DMA0CTL |= (uint16_t)(p_eSrcIncrement*DMASRCINCR0);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        DMA1CTL &= (uint16_t)(~((DMA_SRCINCR_NBR-1)*DMASRCINCR0));
        DMA1CTL |= (uint16_t)(p_eSrcIncrement*DMASRCINCR0);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        DMA2CTL &= (uint16_t)(~((DMA_SRCINCR_NBR-1)*DMASRCINCR0));
        DMA2CTL |= (uint16_t)(p_eSrcIncrement*DMASRCINCR0);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetDestByte

Description :   Set destination byte.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eDestbyte = see eDMA_Destbyte_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_DESTINATION_BYTE.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetDestByte (eDMA_Channel_t p_eChannel, eDMA_Destbyte_t p_eDestbyte) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_DESTINATION_BYTE;

  /* If valid transfer mode */
  if (DMA_DSTBYTE_NBR > p_eDestbyte) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        DMA0CTL &= (uint16_t)(~((DMA_DSTBYTE_NBR-1)*DMADSTBYTE));
        DMA0CTL |= (uint16_t)(p_eDestbyte*DMADSTBYTE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        DMA1CTL &= (uint16_t)(~((DMA_DSTBYTE_NBR-1)*DMADSTBYTE));
        DMA1CTL |= (uint16_t)(p_eDestbyte*DMADSTBYTE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        DMA2CTL &= (uint16_t)(~((DMA_DSTBYTE_NBR-1)*DMADSTBYTE));
        DMA2CTL |= (uint16_t)(p_eDestbyte*DMADSTBYTE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetSrcByte

Description :   Set source byte.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eSrcbyte = see eDMA_Srcbyte_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_SOURCE_BYTE.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetSrcByte (eDMA_Channel_t p_eChannel, eDMA_Srcbyte_t p_eSrcbyte) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_SOURCE_BYTE;

  /* If valid transfer mode */
  if (DMA_SRCINCR_NBR > p_eSrcbyte) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        DMA0CTL &= (uint16_t)(~((DMA_SRCINCR_NBR-1)*DMASRCBYTE));
        DMA0CTL |= (uint16_t)(p_eSrcbyte*DMASRCBYTE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        DMA1CTL &= (uint16_t)(~((DMA_SRCINCR_NBR-1)*DMASRCBYTE));
        DMA1CTL |= (uint16_t)(p_eSrcbyte*DMASRCBYTE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        DMA2CTL &= (uint16_t)(~((DMA_SRCINCR_NBR-1)*DMASRCBYTE));
        DMA2CTL |= (uint16_t)(p_eSrcbyte*DMASRCBYTE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetLevel

Description :   Set level.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eLevel = see eDMA_Level_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_LEVEL.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetLevel (eDMA_Channel_t p_eChannel, eDMA_Level_t p_eLevel) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_LEVEL;

  /* If valid transfer mode */
  if (DMA_LEVEL_NBR > p_eLevel) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        DMA0CTL &= (uint16_t)(~((DMA_LEVEL_NBR-1)*DMALEVEL));
        DMA0CTL |= (uint16_t)(p_eLevel*DMALEVEL);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        DMA1CTL &= (uint16_t)(~((DMA_LEVEL_NBR-1)*DMALEVEL));
        DMA1CTL |= (uint16_t)(p_eLevel*DMALEVEL);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        DMA2CTL &= (uint16_t)(~((DMA_LEVEL_NBR-1)*DMALEVEL));
        DMA2CTL |= (uint16_t)(p_eLevel*DMALEVEL);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetDstAddr

Description :   Set destination address.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_u32AdrDst = destination address.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_DESTINATION_ADDRESS.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetDstAddr (eDMA_Channel_t p_eChannel, uint32_t p_u32AdrDst) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_DESTINATION_ADDRESS;

  /* Check input parameters */
  if (0 < p_u32AdrDst) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

         __data16_write_addr ((uint16_t)&DMA0DA, p_u32AdrDst);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        __data16_write_addr ((uint16_t)&DMA1DA, p_u32AdrDst);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        __data16_write_addr ((uint16_t)&DMA2DA, p_u32AdrDst);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        /* Nothing to do */

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetSourceAddr

Description :   Set source address.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_u32AdrSrc = source address.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_SOURCE_ADDRESS.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetSourceAddr (eDMA_Channel_t p_eChannel, uint32_t p_u32AdrSrc) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_SOURCE_ADDRESS;

  /* Check input parameters */
  if (0 < p_u32AdrSrc) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        __data16_write_addr ((uint16_t)&DMA0SA, p_u32AdrSrc);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        __data16_write_addr ((uint16_t)&DMA1SA, p_u32AdrSrc);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        __data16_write_addr ((uint16_t)&DMA2SA, p_u32AdrSrc);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        /* Nothing to do */

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetTransferSize

Description :   Set transfer size.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_u16TransferSize = transfer size.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_TRANSFER_SIZE.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
static eDMA_ErrorCodes_t eDMA_SetTransferSize (eDMA_Channel_t p_eChannel, uint16_t p_u16TransferSize) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_TRANSFER_SIZE;

  /* Check input parameters */
  if (0 < p_u16TransferSize) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        DMA0SZ = p_u16TransferSize;

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        DMA1SZ = p_u16TransferSize;

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        DMA2SZ = p_u16TransferSize;

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        /* Nothing to do */

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   vDMA_ISR

Description :   DMA Interrupt Service Routine.

Parameters  :   None.

Return      :   None.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
#ifndef DMA_USE_EXTERNAL_ISR
#pragma vector = DMA_VECTOR
__interrupt void vDMA_ISR (void) {

  switch (__even_in_range (DMAIV, 16)) {

    case DMAIV_NONE: {

      /* Nothing to do */

      break;
    }
    case DMAIV_DMA0IFG: {

      if (NULL != apfDMA_Isr[DMA_CHANNEL_0]) {

        apfDMA_Isr[DMA_CHANNEL_0]();
      }
      else {

        /* Disable DMA0 */
        DMA0CTL &= ~((uint16_t) (DMAEN + DMAIE));
      }

      break;
    }
    case DMAIV_DMA1IFG: {

      if (NULL != apfDMA_Isr[DMA_CHANNEL_1]) {

        apfDMA_Isr[DMA_CHANNEL_1]();
      }
      else {

        /* Disable DMA1 */
        DMA1CTL &= ~((uint16_t) (DMAEN + DMAIE));
      }

      break;
    }
    case DMAIV_DMA2IFG: {

      if (NULL != apfDMA_Isr[DMA_CHANNEL_2]) {

        apfDMA_Isr[DMA_CHANNEL_2]();
      }
      else {

        /* Disable DMA2 */
        DMA2CTL &= ~((uint16_t) (DMAEN + DMAIE));
      }
    }
    default: {

      /* Nothing to do */

      break;
    }
  }
}
#endif /* UCSI_Ax_USE_EXTERNAL_ISR */


/* ============================================ EXPORTED FUNCTIONS ============================================ */
/* =============================================================================
Function    :   eDMA_Init

Description :   Initialize DMAx registers to default value and clear ISRx function pointer.

Parameters  :   p_eChannel = see eDMA_Channel_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_INIT.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
eDMA_ErrorCodes_t eDMA_Init (eDMA_Channel_t p_eChannel) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_INIT;

  switch (p_eChannel) {

    case DMA_CHANNEL_0: {

      /* Reset DMA0 registers */
      DMA0CTL = DMA_DMA0CTL_DEFAULT_VALUE;
      DMACTL0 &= (~DMA0TSEL_31);

#ifndef DMA_USE_EXTERNAL_ISR
      /* Delete ISR */
      apfDMA_Isr[p_eChannel] = NULL;
#endif /* DMA_USE_EXTERNAL_ISR */

      l_eErrorCode = DMA_ERROR_NO;
      break;
    }
    case DMA_CHANNEL_1: {

      /* Reset DMA1 registers */
      DMA1CTL = DMA_DMA1CTL_DEFAULT_VALUE;
      DMACTL0 &= (~DMA1TSEL_31);

#ifndef DMA_USE_EXTERNAL_ISR
      /* Delete ISR */
      apfDMA_Isr[p_eChannel] = NULL;
#endif /* DMA_USE_EXTERNAL_ISR */

      l_eErrorCode = DMA_ERROR_NO;
      break;
    }
    case DMA_CHANNEL_2: {

      /* Reset DMA2 registers */
      DMA2CTL = DMA_DMA2CTL_DEFAULT_VALUE;
      DMACTL1 = (~DMA2TSEL_31);

#ifndef DMA_USE_EXTERNAL_ISR
      /* Delete ISR */
      apfDMA_Isr[p_eChannel] = NULL;
#endif /* DMA_USE_EXTERNAL_ISR */

      l_eErrorCode = DMA_ERROR_NO;
      break;
    }
    default: {

      /* Nothing to do */

      break;
    }
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_Configure

Description :   Configure and enable DMA. After configuration, don't forget to configure interrupt and start DMA.

Parameters  :   p_psConfiguration = Pointer to structure configuration.

Return      :   DMA_ERROR_NO or DMA_ERROR_CONFIGURE.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
eDMA_ErrorCodes_t eDMA_Configure (sDMAx_Configuration_t *p_psConfiguration) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_CONFIGURE;

  /* Check input parameter */
  if (NULL != p_psConfiguration) {

    /* Check input parameters values */
    if ((0 != p_psConfiguration->TransferSrc) &&
        (0 != p_psConfiguration->TransferDst) &&
        (0 < p_psConfiguration->u16TransferSize)) {

      /* Init interface to default value */
      l_eErrorCode = eDMA_Init (p_psConfiguration->eChannel);
      if (DMA_ERROR_NO == l_eErrorCode) {

        /* Set trigger */
        l_eErrorCode = eDMA_SetTrigger (p_psConfiguration->eChannel, p_psConfiguration->eTrigger);
        if (DMA_ERROR_NO == l_eErrorCode) {

          /* Set transfer mode */
          l_eErrorCode = eDMA_SetTransferMode (p_psConfiguration->eChannel, p_psConfiguration->eTransferMode);
          if (DMA_ERROR_NO == l_eErrorCode) {

            /* Set destination increment */
            l_eErrorCode = eDMA_SetDestInc (p_psConfiguration->eChannel, p_psConfiguration->eDstIncr);
            if (DMA_ERROR_NO == l_eErrorCode) {

              /* Set source increment */
              l_eErrorCode = eDMA_SetSrcInc (p_psConfiguration->eChannel, p_psConfiguration->eSrcIncr);
              if (DMA_ERROR_NO == l_eErrorCode) {

                /* Set destination byte */
                l_eErrorCode = eDMA_SetDestByte (p_psConfiguration->eChannel, p_psConfiguration->eDstByte);
                if (DMA_ERROR_NO == l_eErrorCode) {

                  /* Set source byte */
                  l_eErrorCode = eDMA_SetSrcByte (p_psConfiguration->eChannel, p_psConfiguration->eSrcByte);
                  if (DMA_ERROR_NO == l_eErrorCode) {

                    /* Set level */
                    l_eErrorCode = eDMA_SetLevel (p_psConfiguration->eChannel, p_psConfiguration->eLevel);
                    if (DMA_ERROR_NO == l_eErrorCode) {

                      /* Set destination address */
                      l_eErrorCode = eDMA_SetDstAddr (p_psConfiguration->eChannel, p_psConfiguration->TransferDst);
                      if (DMA_ERROR_NO == l_eErrorCode) {

                        /* Set source address */
                        l_eErrorCode = eDMA_SetSourceAddr (p_psConfiguration->eChannel, p_psConfiguration->TransferSrc);
                        if (DMA_ERROR_NO == l_eErrorCode) {

                          /* Set transfer size */
                          l_eErrorCode = eDMA_SetTransferSize (p_psConfiguration->eChannel, p_psConfiguration->u16TransferSize);
                          if (DMA_ERROR_NO == l_eErrorCode) {

                            /* Set DMA state*/
                            l_eErrorCode = eDMA_SetState (p_psConfiguration->eChannel, p_psConfiguration->eState);
                          }
                          else {

                             /* Nothing to do */
                          }
                        }
                        else {

                          /* Nothing to do */
                        }
                      }
                      else {

                        /* Nothing to do */
                      }
                    }
                    else {

                      /* Nothing to do */
                    }
                  }
                  else {

                    /* Nothing to do */
                  }
                }
                else {

                  /* Nothing to do */
                }
              }
              else {

                /* Nothing to do */
              }
            }
            else {

              /* Nothing to do */
            }
          }
          else {

            /* Nothing to do */
          }
        }
        else {

          /* Nothing to do */
        }
      }
      else {

        /* Nothing to do */
      }
    }
    else {

      /* Nothing to do */
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetState

Description :   Enable or disable DMA.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eState = see eDMA_State_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_STATE.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
eDMA_ErrorCodes_t eDMA_SetState (eDMA_Channel_t p_eChannel, eDMA_State_t p_eState) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_STATE;

  /* Check input parameter */
  if (DMA_DMAEN_NBR > p_eState) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        DMA0CTL &= (uint16_t)(~DMAEN);
        DMA0CTL |= (uint16_t)(p_eState*DMAEN);

        l_eErrorCode = DMA_ERROR_NO;

        break;
      }
      case DMA_CHANNEL_1: {

        DMA1CTL &= (uint16_t)(~DMAEN);
        DMA1CTL |= (uint16_t)(p_eState*DMAEN);

        l_eErrorCode = DMA_ERROR_NO;

        break;
      }
      case DMA_CHANNEL_2: {

        DMA2CTL &= (uint16_t)(~DMAEN);
        DMA2CTL |= (uint16_t)(p_eState*DMAEN);

        l_eErrorCode = DMA_ERROR_NO;

        break;
      }
      default: {

        /* Nothing to do */

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetInterruptState

Description :   Enable or disable DMA interrupt.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eState = see eDMA_InterruptState_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_INTERRUPT_STATE.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
eDMA_ErrorCodes_t eDMA_SetInterruptState (eDMA_Channel_t p_eChannel, eDMA_InterruptState_t p_eState) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_INTERRUPT_STATE;

  /* Check input parameters */
  if (DMA_DMAIE_NBR > p_eState) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        DMA0CTL &= (uint16_t)(~DMAIE);
        DMA0CTL |= (uint16_t)(p_eState*DMAIE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        DMA1CTL &= (uint16_t)(~DMAIE);
        DMA1CTL |= (uint16_t)(p_eState*DMAIE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        DMA2CTL &= (uint16_t)(~DMAIE);
        DMA2CTL |= (uint16_t)(p_eState*DMAIE);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        /* Nothing to do */

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_SetRequest

Description :   Request/Start DMA transfer.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_eState = see eDMARequestState_t.

Return      :   DMA_ERROR_NO or DMA_ERROR_SET_REQUEST_STATE.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
eDMA_ErrorCodes_t eDMA_SetRequest (eDMA_Channel_t p_eChannel, eDMARequestState_t p_eState) {

  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_SET_REQUEST_STATE;

  /* Check input parameters */
  if (DMA_REQ_NBR > p_eState) {

    switch (p_eChannel) {

      case DMA_CHANNEL_0: {

        /* Set new value */
        DMA0CTL &= (uint16_t)(~DMAREQ);
        DMA0CTL |= (uint16_t)(p_eState*DMAREQ);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_1: {

        /* Set new value */
        DMA1CTL &= (uint16_t)(~DMAREQ);
        DMA1CTL |= (uint16_t)(p_eState*DMAREQ);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      case DMA_CHANNEL_2: {

        /* Set new value */
        DMA2CTL &= (uint16_t)(~DMAREQ);
        DMA2CTL |= (uint16_t)(p_eState*DMAREQ);

        l_eErrorCode = DMA_ERROR_NO;
        break;
      }
      default: {

        /* Nothing to do */

        break;
      }
    }
  }
  else {

    /* Nothing to do */
  }

  return (l_eErrorCode);
}


#ifndef DMA_USE_EXTERNAL_ISR
/* =============================================================================
Function    :   eDMA_IsrInstall

Description :   Install DMAx Interrupt Service Routine.

Parameters  :   p_eChannel = see eDMA_Channel_t,
                p_pfuncIsr = function pointer to user isr.

Return      :   DMA_ERROR_NO or DMA_ERROR_INSTALL.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
eDMA_ErrorCodes_t eDMA_IsrInstall (eDMA_Channel_t p_eChannel,
                                      pfDMA_isr_t p_pfuncIsr)
{
  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_ISR_INSTALL;

  /* Check input parameters */
  if ((DMA_CHANNEL_NBR > p_eChannel) && (NULL != p_pfuncIsr))
  {
    /* Disable interrupt */
    l_eErrorCode = eDMA_SetInterruptState (p_eChannel, DMA_DMAIE_DISABLED);
    if (DMA_ERROR_NO == l_eErrorCode)
    {
      /* Save ISR */
    	apfDMA_Isr[p_eChannel] = p_pfuncIsr;
    }
    else
    {
      /* Nothing to do */
    }
  }
  else
  {
    /* Nothing to do */
  }

  return (l_eErrorCode);
}


/* =============================================================================
Function    :   eDMA_IsrUninstall

Description :   Uninstall DMAx Interrupt Service Routine.

Parameters  :   p_eChannel = see eDMA_Channel_t,

Return      :   DMA_ERROR_NO or DMA_ERROR_UNINSTALL.
--------------------------------------------------------------------------------
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 29/05/17  | E.LANDEL        | Function creation.
============================================================================= */
eDMA_ErrorCodes_t eDMA_IsrUninstall (eDMA_Channel_t p_eChannel)
{
  /* Locals variables declaration */
  eDMA_ErrorCodes_t l_eErrorCode;

  /* Default return error code */
  l_eErrorCode = DMA_ERROR_ISR_UNINSTALL;

  /* Check input parameters */
  if (DMA_CHANNEL_NBR > p_eChannel)
  {
    /* Disable interrupt */
    l_eErrorCode = eDMA_SetInterruptState (p_eChannel, DMA_DMAIE_DISABLED);
    if (DMA_ERROR_NO == l_eErrorCode)
    {
      /* Delete ISR */
    	apfDMA_Isr[p_eChannel] = NULL;
    }
    else
    {
      /* Nothing to do */
    }
  }
  else
  {
    /* Nothing to do */
  }

  return (l_eErrorCode);
}
#endif /* DMA_USE_EXTERNAL_ISR */


