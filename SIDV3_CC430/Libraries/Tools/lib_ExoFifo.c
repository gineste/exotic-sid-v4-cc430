/* =============================================================================
 ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
(  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
(____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/

  Copyright (c) 2017 EXOTIC SYSTEMS. All Rights Reserved.

  Licensees are granted free, non-transferable use of the information. NO
  WARRANTY of ANY KIND is provided. This heading must NOT be removed from
  the file.

  File name:    lib_ExoFifo.c
  Date:         13 12 2017
  Author:       Emeric L.
  Description:  Fifo library - Body file.
============================================================================= */

/* =============================================================================
                                 DEBUG Section
============================================================================= */

/* =============================================================================
                                 Include Files
============================================================================= */
#include <stdint.h>
#include "lib_ExoTypes.h"
#include "lib_ExoDefs.h"
#include "lib_ExoErrors.h"
#include "lib_ExoFifo.h"

/* =============================================================================
                          Private defines and typedefs
============================================================================= */


/* =============================================================================
                        Private constants and variables
============================================================================= */


/* =============================================================================
                        Public constants and variables
============================================================================= */


/* =============================================================================
                               Public functions
============================================================================= */

/* =============================================================================
Function    :   vEXOFIFO_Init

Description :   Initialize RF FIFO.

Parameters  :   None.

Return      :   None.
============================================================================= */
void vEXOFIFO_Init (s_ExoFifo_Context_t *p_psContext)
{
  /* Locals variables declaration */
  uint16_t l_u16InitIndex = UINT16_MIN_VALUE;

  /* Clear lib_ExoFifo contents */
  for (l_u16InitIndex = UINT16_MIN_VALUE; l_u16InitIndex < EXOFIFO_SIZE; l_u16InitIndex++)
  {
    p_psContext->u8Buffer[l_u16InitIndex] = UINT16_MIN_VALUE;
  }

  /* Initialize Push and Pop index */
  p_psContext->u16PushIdx = UINT16_MIN_VALUE;
  p_psContext->u16PopIdx = UINT16_MIN_VALUE;

  /* Set lib_ExoFifo empty */
  p_psContext->eStatus = EXOFIFO_STATUS_EMPTY;
}


/* =============================================================================
Function    :   vEXOFIFO_Push

Description :   Push Data on FIFO.

Parameters  :   p_u8ByteToPush = Byte to push.

Return      :   None.
============================================================================= */
void vEXOFIFO_Push (s_ExoFifo_Context_t *p_psContext, uint8_t p_u8ByteToPush)
{
  /* Save byte inside FIFO (PUSH). */
  p_psContext->u8Buffer[p_psContext->u16PushIdx] = p_u8ByteToPush;

  /* Next byte */
  p_psContext->u16PushIdx = (p_psContext->u16PushIdx + 1u) % EXOFIFO_SIZE;

  /* If push and pop index are same */
  if (   (p_psContext->u16PushIdx   == p_psContext->u16PopIdx  )
      && (p_psContext->u8PushBitPos == p_psContext->u8PopBitPos) )
  {
    /* FIFO is full */
    p_psContext->eStatus = EXOFIFO_STATUS_FULL;
  }
  else
  {
    /* FIFO was full, but a new byte is received => OVERFLOW */
    if (   (EXOFIFO_STATUS_FULL == p_psContext->eStatus)
        || (EXOFIFO_STATUS_OVERFLOW == p_psContext->eStatus) )
    {
      /* FIFO was full or overflow, but a new byte is received => OVERFLOW */
      p_psContext->eStatus = EXOFIFO_STATUS_OVERFLOW;

      /* Set Pop index to Push index (last byte received) */
      p_psContext->u8PopBitPos = p_psContext->u8PushBitPos;
      p_psContext->u16PopIdx = p_psContext->u16PushIdx;
    }
    else
    {
      /* FIFO is not empty */
      p_psContext->eStatus = EXOFIFO_STATUS_NOT_EMPTY;
    }
  }
}


/* =============================================================================
Function    :   u8EXOFIFO_Pop

Description :   Pop Data from FIFO.

Parameters  :   None.

Return      :   Popped data.
============================================================================= */
uint8_t u8EXOFIFO_Pop (s_ExoFifo_Context_t *p_psContext)
{
  /* Locals variables declaration */
  uint8_t l_u8PoppedTxByte = UINT8_MIN_VALUE;

  /* If FIFO not empty */
  if (EXOFIFO_STATUS_EMPTY == p_psContext->eStatus)
  {
    /* Nothing to do */
  }
  else
  {
    /* Is not in overflow */
    if (EXOFIFO_STATUS_OVERFLOW != p_psContext->eStatus)
    {
      /* Pop byte. */
      l_u8PoppedTxByte = p_psContext->u8Buffer[p_psContext->u16PopIdx];

      /* Next byte */
      p_psContext->u16PopIdx = (p_psContext->u16PopIdx + 1u) % EXOFIFO_SIZE;
    }
    else
    {
      /* OVERFLOW case */
      p_psContext->u16PushIdx = p_psContext->u16PopIdx;
    }

    /* If Pop and Push index are same */
    if (   (p_psContext->u16PushIdx   == p_psContext->u16PopIdx  )
        && (p_psContext->u8PushBitPos == p_psContext->u8PopBitPos) )
    {
      p_psContext->eStatus = EXOFIFO_STATUS_EMPTY;
    }
    else
    {
      p_psContext->eStatus = EXOFIFO_STATUS_NOT_EMPTY;
    }
  }

  return (l_u8PoppedTxByte);
}


