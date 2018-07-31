/* =============================================================================
 ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
(  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
(____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/

================================================================================
                                    Header file
================================================================================
Files       :   dma.h

Target      :   CC430F5137 (Code composer studio v6.1.0.00104).

Description :   DMA Driver. 
                If used external DMA ISR define DMA_USE_EXTERNAL_ISR in project
                settings.
================================================================================
   VERSION  |   DATE    |     AUTHOR      |    MODIFICATIONS
--------------------------------------------------------------------------------
  01.00.00  | 06/06/17  | E.LANDEL        | Module creation.
============================================================================= */
#ifndef DMA__H
  #define DMA__H

	/* ======================= EXPORTED DEFINITIONS  ======================= */

  /* === Defines === */

  /* === Constants === */

  /* === Macros === */

  /* === Typedef === */
  typedef enum _DMA_ERRORCODES_
  {
    DMA_ERROR_NO = 0u,
    DMA_ERROR_SET_TRIGGER,
    DMA_ERROR_SET_TRANSFER_MODE,
    DMA_ERROR_SET_DESTINATION_INCREMENT,
    DMA_ERROR_SET_SOURCE_INCREMENT,
    DMA_ERROR_SET_DESTINATION_BYTE,
    DMA_ERROR_SET_SOURCE_BYTE,
    DMA_ERROR_SET_LEVEL,
    DMA_ERROR_SET_DESTINATION_ADDRESS,
    DMA_ERROR_SET_SOURCE_ADDRESS,
    DMA_ERROR_SET_TRANSFER_SIZE,
    DMA_ERROR_INIT,
    DMA_ERROR_CONFIGURE,
    DMA_ERROR_SET_STATE,
    DMA_ERROR_SET_INTERRUPT_STATE,
    DMA_ERROR_SET_REQUEST_STATE,
    DMA_ERROR_ISR_INSTALL,
    DMA_ERROR_ISR_UNINSTALL,
    DMA_ERROR_NBR

  } eDMA_ErrorCodes_t;

  typedef enum _DMA_CHANNEL_
  {
    DMA_CHANNEL_0 = 0u,
    DMA_CHANNEL_1,
    DMA_CHANNEL_2,
    DMA_CHANNEL_NBR

  } eDMA_Channel_t;

  typedef enum _DMA_TRIGGER_
  {
    DMA_TRIGGER_DMAREQ = 0,
    DMA_TRIGGER_TA0CCR0,
    DMA_TRIGGER_TA0CCR2,
    DMA_TRIGGER_TA1CCR0,
    DMA_TRIGGER_TA1CCR2,
    DMA_TRIGGER_TB0CCR0,
    DMA_TRIGGER_TB0CCR2,
    DMA_TRIGGER_RES7,
    DMA_TRIGGER_RES8,
    DMA_TRIGGER_RES9,
    DMA_TRIGGER_RES10,
    DMA_TRIGGER_RES11,
    DMA_TRIGGER_RES12,
    DMA_TRIGGER_RES13,
    DMA_TRIGGER_UCA0RXIFG,
    DMA_TRIGGER_UCA0TXIFG,
    DMA_TRIGGER_UCB0RXIFG,
    DMA_TRIGGER_UCB0TXIFG,
    DMA_TRIGGER_RES20,
    DMA_TRIGGER_RES21,
    DMA_TRIGGER_RES22,
    DMA_TRIGGER_RES23,
    DMA_TRIGGER_ADC12IFGx,
    DMA_TRIGGER_RES24,
    DMA_TRIGGER_RES25,
    DMA_TRIGGER_RES26,
    DMA_TRIGGER_RES27,
    DMA_TRIGGER_RES28,
    DMA_TRIGGER_MPYRDY,
    DMA_TRIGGER_DMA2IFG,
    DMA_TRIGGER_DMAE0,
    DMA_TRIGGER_NBR

  } eDMA_Trigger_t;

  typedef enum _DMA_TRANSFER_MODE_
  {
    DMA_DT_SINGLE = 0u,
    DMA_DT_BLOCK,
    DMA_DT_BURST_BLOCK,
    DMA_DT_BURST_BLOCK2,
    DMA_DT_REPEATED_SINGLE,
    DMA_DT_REPEATED_BLOCK,
    DMA_DT_REPEATED_BURST_BLOCK,
    DMA_DT_REPEATED_BURST_BLOCK2,
    DMA_DT_NBR,

  } eDMA_TransferMode_t;

  typedef enum _DMA_DEST_INCREMENT_
  {
    DMA_DSTINCR_UNCHANGED = 0u,
    DMA_DSTINCR_UNCHANGED_,
    DMA_DSTINCR_DECREMENTED,
    DMA_DSTINCR_INCREMENTED,
    DMA_DSTINCR_NBR,

  } eDMA_DestIncrement_t;

  typedef enum _DMA_SRC_INCREMENT_
  {
    DMA_SRCINCR_UNCHANGED = 0u,
    DMA_SRCINCR_UNCHANGED_,
    DMA_SRCINCR_DECREMENTED,
    DMA_SRCINCR_INCREMENTED,
    DMA_SRCINCR_NBR,

  } eDMA_SrcIncrement_t;

  typedef enum _DMA_DEST_BYTE_
  {
    DMA_DSTBYTE_WORD = 0u,
    DMA_DSTBYTE_BYTE,
    DMA_DSTBYTE_NBR,

  } eDMA_Destbyte_t;

  typedef enum _DMA_SRC_BYTE_
  {
    DMA_SRCBYTE_WORD = 0u,
    DMA_SRCBYTE_BYTE,
    DMA_SRCBYTE_NBR,

  } eDMA_Srcbyte_t;

  typedef enum _DMA_LEVEL_
  {
    DMA_LEVEL_EDGE = 0u,
    DMA_LEVEL_LEVEL,
    DMA_LEVEL_NBR,

  } eDMA_Level_t;

  typedef enum _DMA_STATE_
  {
    DMA_DMAEN_DISABLED = 0u,
    DMA_DMAEN_ENABLED,
    DMA_DMAEN_NBR

  } eDMA_State_t;

  typedef enum _DMA_INTERRUPT_STATE_
  {
    DMA_DMAIE_DISABLED = 0u,
    DMA_DMAIE_ENABLED,
    DMA_DMAIE_NBR

  } eDMA_InterruptState_t;

  typedef enum DMA_REQUEST_STATE_
  {
    DMA_REQ_NOSTART = 0u,
    DMA_REQ_START,
    DMA_REQ_NBR

  } eDMARequestState_t;

  typedef struct _DMAx_CONFIGURATION_
  {
    eDMA_Channel_t eChannel;
    eDMA_Trigger_t eTrigger;
    eDMA_TransferMode_t eTransferMode;
    eDMA_DestIncrement_t eDstIncr;
    eDMA_SrcIncrement_t eSrcIncr;
    eDMA_Destbyte_t eDstByte;
    eDMA_Srcbyte_t eSrcByte;
    eDMA_Level_t eLevel;
    eDMA_State_t eState;

    uint32_t TransferSrc;
    uint32_t TransferDst;
    uint16_t u16TransferSize;

  } sDMAx_Configuration_t;

#ifndef DMA_USE_EXTERNAL_ISR
  typedef void (*pfDMA_isr_t) (void);
#endif /* DMA_USE_EXTERNAL_ISR */

  /* === Variables === */

  /* === DEBUG Section === */

  /* ======================== EXPORTED FUNCTIONS ========================= */
  extern eDMA_ErrorCodes_t eDMA_Init (eDMA_Channel_t p_eChannel);
  extern eDMA_ErrorCodes_t eDMA_Configure (sDMAx_Configuration_t *p_psConfiguration);
  extern eDMA_ErrorCodes_t eDMA_SetState (eDMA_Channel_t p_eChannel, eDMA_State_t p_eState);
  extern eDMA_ErrorCodes_t eDMA_SetInterruptState (eDMA_Channel_t p_eChannel, eDMA_InterruptState_t eState);
  extern eDMA_ErrorCodes_t eDMA_SetRequest (eDMA_Channel_t p_eChannel, eDMARequestState_t eState);

  #ifndef DMA_USE_EXTERNAL_ISR
  extern eDMA_ErrorCodes_t eDMA_IsrInstall (eDMA_Channel_t p_eChannel,  pfDMA_isr_t p_pfuncIsr);
  extern eDMA_ErrorCodes_t eDMA_IsrUninstall (eDMA_Channel_t p_eChannel);
  #endif /* DMA_USE_EXTERNAL_ISR */

#endif /* DMA__H */

