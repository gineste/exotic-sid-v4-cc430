/* =============================================================================
 ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
(  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
(____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/

  Copyright (c) 2017 EXOTIC SYSTEMS. All Rights Reserved.

  Licensees are granted free, non-transferable use of the information. NO
  WARRANTY of ANY KIND is provided. This heading must NOT be removed from
  the file.

  File name:    lib_ExoFifo.h
  Date:         13 12 2017
  Author:       Emeric L.
  Description:  Fifo library - Header file.
============================================================================= */
#ifndef LIB_EXOEXOFIFO__H
  #define LIB_EXOEXOFIFO__H

  /* ===========================================================================
                                 DEBUG Section
  =========================================================================== */

  /* ===========================================================================
                        Public constants and variables
  =========================================================================== */
  #define EXOFIFO_SIZE (512u)                                    /* in bytes. */

  typedef struct _EXOFIFO_CONTEXT_ {
    uint8_t u8Buffer[EXOFIFO_SIZE];
    uint16_t u16PushIdx;
    uint16_t u16PopIdx;
    uint8_t u8PopBitPos;
    uint8_t u8PushBitPos;
    enum _EXOFIFO_STATUS_ {
      EXOFIFO_STATUS_EMPTY = 0u,
      EXOFIFO_STATUS_NOT_EMPTY,
      EXOFIFO_STATUS_FULL,
      EXOFIFO_STATUS_OVERFLOW,
      EXOFIFO_STATUS_MAX
    } eStatus;
  } s_ExoFifo_Context_t;

  /* ===========================================================================
                         Public functions declarations
  =========================================================================== */
  extern void     vEXOFIFO_Reset (s_ExoFifo_Context_t *p_psContext);
  extern void     vEXOFIFO_Init (s_ExoFifo_Context_t *p_psContext);
  extern void     vEXOFIFO_Push (  s_ExoFifo_Context_t *p_psContext
                                 , uint8_t p_u8ByteToPush);
  extern uint8_t  u8EXOFIFO_Pop (s_ExoFifo_Context_t *p_psContext);

#endif /* LIB_EXOEXOFIFO__H */

