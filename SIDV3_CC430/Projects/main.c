/* =============================================================================
 ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
(  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
(____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/

  Copyright (c) 2017 EXOTIC SYSTEMS. All Rights Reserved.

  Licensees are granted free, non-transferable use of the information. NO
  WARRANTY of ANY KIND is provided. This heading must NOT be removed from
  the file.

  File name:    main.c
  Date:         02 01 2018
  Author:       Emeric L.
  Description:  SID V3 project - Main file.
============================================================================= */

/* =============================================================================
                                 DEBUG Section
============================================================================= */
#define DEBUG_MEMS

/* =============================================================================
                                 Include Files
============================================================================= */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "Tools/lib_ExoTypes.h"
#include "Tools/lib_ExoDefs.h"
#include "Tools/lib_ExoErrors.h"
#include <msp430.h>
#include "boardconfig.h"
#include "CC430/dma.h"
#include "RF/RF1A.h"
#include "TPMS/tpms.h"
#include "CC430/hal_pmm.h"
#include "ES_Protocol/ES_OpCodes.h"
#include "ES_Protocol/ES_Protocol.h"


/* =============================================================================
                          Private defines and typedefs
============================================================================= */
/* Clocks. */
#define DELAY_AFTER_FLL_SET           ((uint32_t)(32*32*BOARD_DCO_MULT))

/* GPIO IO state. */
#define PORTx_LOW_LEVEL               ((uint8_t)0)
#define PORTx_HIGH_LEVEL              ((uint8_t)0xff)
#define PORTx_PULL_DISABLE            ((uint8_t)0)
#define PORTx_DIR_INPUT               ((uint8_t)0)
#define PORTx_DIR_OUTPUT              ((uint8_t)0xff)
#define PORTJ_DIR_OUTPUT              ((uint8_t)0x0F)
#define P5DIR_DIR_OUTPUT              ((uint8_t)0x03)

/* GPIO Port 1 definition. */
#define GPO_NOT_READY                 ((uint8_t)BIT5)
#define GPIO_UCB0SDA                  ((uint8_t)BIT3)
#define GPI_UCB0SCL                   ((uint8_t)BIT2)

/* RF. */
#define PATABLE_VAL_MIN               ((uint8_t)0x26)
#define PATABLE_VAL_DEF               ((uint8_t)0x50)
#define PATABLE_VAL_MAX               ((uint8_t)0xC0)

/* Timer A1.*/
#define TA1CCR0_DEFAULT_VALUE         UINT8_MIN_VALUE
#define TA1CCR1_DEFAULT_VALUE         UINT8_MIN_VALUE
#define TA1CTL_DEFAULT_VALUE          UINT8_MIN_VALUE
#define TA1CCTL0_DEFAULT_VALUE        UINT8_MIN_VALUE
#define ES_SENSOR_TYPE_POS            UINT8_MIN_VALUE

/* I2C slave address. */
#define I2C_MY_ADDRESS                ((uint8_t)0x4D)
#define I2C_BUFFER_LENGTH             ((uint16_t)256)

/* ES frame. */
#define ES_PAYLOAD_LEN_POS            ((uint8_t)ES_SENSOR_TYPE_POS + 1)
#define ES_PAYLOAD_POS                ((uint8_t)ES_SENSOR_TYPE_POS + 3)
#define ES_MEMSFRAME_LEN              ((uint8_t)11)
#define ES_LDLFRAME_LEN               ((uint8_t)11)
#define ES_AKFRAME_LEN                ((uint8_t)10)

/* SID frame. */
#define SIDFRAME_SID_ID               ((uint8_t)0)
#define SIDFRAME_AK_ID                ((uint8_t)0xCF)
#define SIDFRAME_ENCAPS_LEN           ((uint8_t)5)
#define SIDFRAME_ID_POS               ((uint8_t)0)
#define SIDFRAME_LEN_POS              ((uint8_t)SIDFRAME_ID_POS + 1)
#define SIDFRAME_PAYLOAD_POS          ((uint8_t)SIDFRAME_LEN_POS + 1)
#define SIDFRAME_HEADER_LEN           ((uint8_t)SIDFRAME_LEN_POS + 1)

/* I2C command frame. */
#define I2CCMD_TAG_POS                ((uint8_t)0)

/* Send RF command specific. */
#define I2CCMD_RF_POWER_POS           ((uint8_t)I2CCMD_TAG_POS + 1)
#define I2CCMD_PAYLOAD_POS            ((uint8_t)I2CCMD_RF_POWER_POS + 1)
#define I2CCMD_INFO_LEN               ((uint8_t)I2CCMD_RF_POWER_POS + 1)

/* Set Rf retry command specific. */
#define I2CCMD_RETRY_POS              ((uint8_t)I2CCMD_TAG_POS + 1)
#define I2CCMD_DELAYMS_POS            ((uint8_t)I2CCMD_RETRY_POS + 1)

/* I2C reply frame. */
#define I2CREPLY_ACK_POS              ((uint8_t)0)
#define I2CREPLY_CMD_POS              ((uint8_t)I2CREPLY_ACK_POS + 1)
#define I2CREPLY_PAYLOAD_POS          ((uint8_t)I2CREPLY_CMD_POS + 1)
#define I2CREPLY_INFO_LEN             ((uint8_t)I2CREPLY_CMD_POS + 1)
#define I2CREPLY_MAX_FRAME_LEN        ((uint8_t)64)
#define I2CREPLY_FW_VERS_LEN          ((uint8_t)3)

/* Miscellaneous. */
#define SEARCHPOWER_TRY               ((uint8_t)6)
#define SEARCHPOWER_INDEX_MAX         ((uint8_t)43)
#define SEARCHPOWER_INDEX_IN          ((uint8_t)0)
#define DELAY_BEFORE_RESET            ((uint32_t)1000000)
#define RESET()                       ((void (*)())0x170)()
#define POWER_LUT_LENGTH              ((uint8_t)44)
#define GDOx_TRISTATE                 ((uint8_t)0x2E)

#define DEFAULT_TX_RETRY		          ((uint8_t)0)
#define DEFAULT_DELAYMS_BETWEEN_FRAME ((uint8_t)50)
#define MAX_TX_ALLOWED                ((uint16_t)500)

#define ONE_MS_IN_CYCLE               ((uint32_t)12288)


#ifdef __REGISTER_MODEL__
/* for IAR */
#   if __REGISTER_MODEL__ == __REGISTER_MODEL_REG20__
#       define __DMA_ACCESS_REG__ (void __data20 *)
#   else
#       define __DMA_ACCESS_REG__ (unsigned short)
#   endif
#else
/* for CCS */
#   define __DMA_ACCESS_REG__ (__SFR_FARPTR)(unsigned long)
#endif

/* FSM states. */
typedef enum _FSM_STATES_ {
    FSM_SLEEP                 = UINT8_MIN_VALUE
  , FSM_PROCESS_COMMAND
  , FSM_RF_TRANSMIT
  , FSM_SEND_RESPONSE_ACK
  , FSM_SEND_RESPONSE_NACK
  , FSM_RESET
  , FSM_INTERNAL_ERROR
  , FSM_MAX
} e_FsmStates_t;

/* ES frame target. */
enum {
  ESP_MASTER_NUM      = UINT8_MIN_VALUE,
  ESP_HOST_STM32_NUM  = 0x01,
  ESP_HOST_CC430_NUM  = 0x02,
  ESP_HOST_BLE_NUM    = 0x03,
  ESP_TRIG_STM32_NUM  = 0x04,
  ESP_MAX_DEVICE_NUM  = 0x05, /* Always last in enumeration. */
};

/* ES frame. */
typedef struct _ES_FRAME_ {
  uint8_t au8Frame[255];
  uint8_t u8FrameLen;
} s_ESFrame_t;

typedef enum RF1A_PacketFormat {
  RF1A_PKT_FORMAT_NORMAL              = UINT8_MIN_VALUE,
  RF1A_PKT_FORMAT_SYNCHRONOUS_SERIAL  = (0x01 << 4),
  RF1A_PKT_FORMAT_RANDOM_TX           = (0x02 << 4),
  RF1A_PKT_FORMAT_ASYNCHRONOUS_SERIAL = (0x03 << 4)
} RF1A_PacketFormat;

typedef enum RF1A_Modulation {
  RF1A_MODULATION_2FSK    =  UINT8_MIN_VALUE,
  RF1A_MODULATION_2GFSK   = (0x01 << 4),
  RF1A_MODULATION_ASK_OOK = (0x03 << 4),
  RF1A_MODULATION_MSK     = (0x07 << 4)
} RF1A_Modulation;

/**
 * \brief List all the register fields.
 */
typedef enum RF1A_RegisterFields {
  RF1A_IOCFG2_GDO2_INV                = (0x01 << 6),
  RF1A_IOCFG2_GDO2_CFG                = (0x3F),
  RF1A_IOCFG1_GDO_DS                  = (0x01 << 7),
  RF1A_IOCFG1_GDO1_INV                = (0x01 << 6),
  RF1A_IOCFG1_GDO1_CFG                = (0x3F),
  RF1A_IOCFG0_TEMP_SENSOR_ENABLE      = (0x01 << 7),
  RF1A_IOCFG0_GDO0_INV                = (0x01 << 6),
  RF1A_IOCFG0_GDO0_CFG                = (0x3F),
  RF1A_FIFOTHR_ADC_RETENTION          = (0x01 << 6),
  RF1A_FIFOTHR_CLOSE_IN_RX            = (0x03 << 4),
  RF1A_FIFOTHR_FIFO_THR               = (0x0F),
  RF1A_SYNC1_SYNC_15_8                = (0xFF),
  RF1A_SYNC0_SYNC_7_0                 = (0xFF),
  RF1A_PKTLEN_PACKET_LENGTH           = (0xFF),
  RF1A_PKTCTRL1_PQT                   = (0x07 << 5),
  RF1A_PKTCTRL1_CRC_AUTOFLUSH         = (0x01 << 3),
  RF1A_PKTCTRL1_APPEND_STATUS         = (0x01 << 2),
  RF1A_PKTCTRL1_ADR_CHK               = (0x03),
  RF1A_PKTCTRL0_WHITE_DATA            = (0x01 << 6),
  RF1A_PKTCTRL0_PKT_FORMAT            = (0x03 << 4),
  RF1A_PKTCTRL0_CRC_EN                = (0x01 << 2),
  RF1A_PKTCTRL0_LENGTH_CONFIG         = (0x03),
  RF1A_ADDR_DEVICE_ADDR               = (0xFF),
  RF1A_CHANNR_CHAN                    = (0xFF),
  RF1A_FSCTRL1_FREQ_IF                = (0x1F),
  RF1A_FSCTRL0_FREQOFF                = (0xFF),
  RF1A_FREQ2_FREQ_23_22               = (0x03 << 6),
  RF1A_FREQ2_FREQ_21_16               = (0x3F),
  RF1A_FREQ1_FREQ_15_8                = (0xFF),
  RF1A_FREQ0_FREQ_7_0                 = (0xFF),
  RF1A_MDMCFG4_CHANBW_E               = (0x03 << 6),
  RF1A_MDMCFG4_CHANBW_M               = (0x03 << 4),
  RF1A_MDMCFG4_DRATE_E                = (0x0F),
  RF1A_MDMCFG3_DRATE_M                = (0xFF),
  RF1A_MDMCFG2_DEM_DCFILT_OFF         = (0x01 << 7),
  RF1A_MDMCFG2_MOD_FORMAT             = (0x07 << 4),
  RF1A_MDMCFG2_MANCHESTER_EN          = (0x01 << 3),
  RF1A_MDMCFG2_SYNC_MODE              = (0x07),
  RF1A_MDMCFG1_FEC_EN                 = (0x01 << 7),
  RF1A_MDMCFG1_NUM_PREAMBLE           = (0x07 << 4),
  RF1A_MDMCFG1_CHANSPC_E              = (0x03),
  RF1A_MDMCFG0_CHANSPC_M              = (0xFF),
  RF1A_DEVIATN_DEVIATION_E            = (0x07 << 4),
  RF1A_DEVIATN_DEVIATION_M            = (0x07),
  RF1A_MCSM2_RX_TIME_RSSI             = (0x01 << 4),
  RF1A_MCSM2_RX_TIME_QUAL             = (0x01 << 3),
  RF1A_MCSM2_RX_TIME                  = (0x07),
  RF1A_MCSM1_CCA_MODE                 = (0x03 << 4),
  RF1A_MCSM1_RXOFF_MODE               = (0x03 << 2),
  RF1A_MCSM1_TXOFF_MODE               = (0x03),
  RF1A_MCSM0_FS_AUTOCAL               = (0x03 << 4),
  RF1A_MCSM0_PO_TIMEOUT               = (0x03 << 2),
  RF1A_MCSM0_PIN_CTRL_EN              = (0x01 << 1),
  RF1A_MCSM0_XOSC_FORCE_ON            = (0x01),
  RF1A_FOCCFG_FOC_BS_CS_GATE          = (0x01 << 5),
  RF1A_FOCCFG_FOC_PRE_K               = (0x03 << 3),
  RF1A_FOCCFG_FOC_POST_K              = (0x01 << 2),
  RF1A_FOCCFG_FOC_LIMIT               = (0x03),
  RF1A_BSCFG_BS_PRE_K                 = (0x03 << 6),
  RF1A_BSCFG_BS_PRE_KP                = (0x03 << 4),
  RF1A_BSCFG_BS_POST_K                = (0x01 << 3),
  RF1A_BSCFG_BS_POST_KP               = (0x01 << 2),
  RF1A_BSCFG_BS_LIMIT                 = (0x03),
  RF1A_AGCCTRL2_MAX_DVGA_GAIN         = (0x03 << 6),
  RF1A_AGCCTRL2_MAX_LNA_GAIN          = (0x07 << 3),
  RF1A_AGCCTRL2_MAGN_TARGET           = (0x07),
  RF1A_AGCCTRL1_AGC_LNA_PRIORITY      = (0x01 << 6),
  RF1A_AGCCTRL1_CARRIER_SENSE_REL_THR = (0x03 << 4),
  RF1A_AGCCTRL1_CARRIER_SENSE_ABS_THR = (0x0F),
  RF1A_AGCCTRL0_HYST_LEVEL            = (0x03 << 6),
  RF1A_AGCCTRL0_WAIT_TIME             = (0x03 << 4),
  RF1A_AGCCTRL0_AGC_FREEZE            = (0x03 << 2),
  RF1A_AGCCTRL0_FILTER_LENGTH         = (0x03),
  RF1A_WOREVT1_EVENT_0_15_8           = (0xFF),
  RF1A_WOREVT0_EVENT_0_7_0            = (0xFF),
  RF1A_WORCTRL_ACLK_PD                = (0x01 << 7),
  RF1A_WORCTRL_RC_PD                  = RF1A_WORCTRL_ACLK_PD,
  RF1A_WORCTRL_EVENT_1                = (0x07 << 4),
  RF1A_WORCTRL_RC_CAL                 = (0x01 << 3),
  RF1A_WORCTRL_WOR_RES                = (0x03),
  RF1A_FREND1_LNA_CURRENT             = (0x03 << 6),
  RF1A_FREND1_LNA2MIX_CURRENT         = (0x03 << 4),
  RF1A_FREND1_LODIV_BUF_CURRENT_RX    = (0x03 << 2),
  RF1A_FREND1_MIX_CURRENT             = (0x03),
  RF1A_FREND0_LODIV_BUF_CURRENT_TX    = (0x03 << 4),
  RF1A_FREND0_PA_POWER                = (0x07),
  RF1A_FSCAL3_FSCAL3_7_6              = (0x03 << 6),
  RF1A_FSCAL3_CHP_CURR_CAL_EN         = (0x03 << 4),
  RF1A_FSCAL3_FSCAL3_3_0              = (0x0F),
  RF1A_FSCAL2_VCO_CORE_H_EN           = (0x01 << 5),
  RF1A_FSCAL2_FSCAL2_4_0              = (0x0F),
  RF1A_FSCAL1_FSCAL1                  = (0x3F),
  RF1A_FSCAL0_FSCAL0                  = (0x7F),
  RF1A_FSTEST_FSTEST                  = (0xFF),
  RF1A_PTEST                          = (0xFF),
  RF1A_AGCTEST                        = (0xFF),
  RF1A_TEST2                          = (0xFF),
  RF1A_TEST1                          = (0xFF),
  RFA1_TEST0_TEST0_7_2                = (0xFC),
  RFA1_TEST0_VCO_SEL_CAL_EN           = (0x01 << 1),
  RFA1_TEST0_TEST0_0                  = (0x01),
  RF1A_PARTNUM_PARTNUM                = (0xFF),
  RF1A_VERSION_VERSION                = (0xFF),
  RF1A_FREQEST_FREQOFF_EST            = (0xFF),
  RF1A_LQI_CRC_OK                     = (0x01 << 7),
  RF1A_LQI_LQI_EST                    = (0x7F),
  RF1A_RSSI_RSSI                      = (0xFF),
  RF1A_MARCSTATE_MARC_STATE           = (0x1F),
  RF1A_WORTIME1_TIME_15_8             = (0xFF),
  RF1A_WORTIME0_TIME_7_0              = (0xFF),
  RF1A_PKTSTATUS_CRC_OK               = (0x01 << 7),
  RF1A_PKTSTATUS_CS                   = (0x01 << 6),
  RF1A_PKTSTATUS_PQT_REACHED          = (0x01 << 5),
  RF1A_PKTSTATUS_CCA                  = (0x01 << 4),
  RF1A_PKTSTATUS_SFD                  = (0x01 << 3),
  RF1A_PKTSTATUS_GDO2                 = (0x01 << 2),
  RF1A_PKTSTATUS_GDO0                 = (0x01),
  RF1A_VCO_VC_DAC_VCO_VC_DAC          = (0xFF),
  RF1A_TXBYTES_TX_UNDERFLOW           = (0x01 << 7),
  RF1A_TXBYTES_NUM_TXBYTES            = (0x7F),
  RF1A_RXBYTES_RX_OVERFLOW            = (0x01 << 7),
  RF1A_RXBYTES_NUM_RXBYTES            = (0x7F)
} RF1A_RegisterFields;

/**
 * \enum RF1A_MARCSTATE_Id
 * \brief Identifiers (the unique ones) for the MARCSTATE status register
 */

typedef enum RF1A_MARCSTATE_Id {
  RF1A_MARCSTATE_SLEEP         = 0,
  RF1A_MARCSTATE_IDLE          = 1,
  RF1A_MARCSTATE_XOFF          = 2,
  RF1A_MARCSTATE_TXRX_SETTLING = 16,
  RF1A_MARCSTATE_RX_OVERFLOW   = 17,
  RF1A_MARCSTATE_FSTXON        = 18,
  RF1A_MARCSTATE_TXON          = 19,
  RF1A_MARCSTATE_RXTX_SETTLING = 21,
  RF1A_MARCSTATE_TX_UNDERFLOW  = 22
} RF1A_MARCSTATE_Id;


typedef enum _I2CSTATE_ {
  I2CSTATE_WAIT_CMD = ((uint8_t)0),
  I2CSTATE_WAIT_FOR_REPLY
} e_I2cState_t;


/* =============================================================================
                        Private constants and variables
============================================================================= */
static const char g_cachCompileTime[] = {__TIME__};
static const char g_cachCompileDate[] = {__DATE__};
static const int8_t g_cas8RfPowerdBm[POWER_LUT_LENGTH] = {
      10,    9,    8,    7,    6,    5,    4,    3,    2,    1
  ,    0,   -1,   -2,   -3,   -4,   -5,   -6,   -7,   -8,   -9
  ,  -10,  -12,  -13,  -14,  -15,  -16,  -17,  -18,  -19
  ,  -20,  -21,  -22,  -23,  -24,  -25,  -26,  -27,  -28
  ,  -30,  -32,  -34,  -37,  -39
  ,  -63
};
static const uint8_t g_cau8PatableValue[POWER_LUT_LENGTH] = {
    0xc0, 0xc3, 0xc6, 0xc9, 0xcc, 0x85, 0x88, 0x8b, 0x8d, 0x8e
  , 0x50, 0x40, 0x63, 0x55, 0x39, 0x68, 0x6a, 0x36, 0x6d, 0x27
  , 0x26, 0x25, 0x33, 0x1f, 0x1d, 0x32, 0x1a, 0x19, 0x18
  , 0x0e, 0x16, 0x0b, 0x15, 0x09, 0x21, 0x07, 0x13, 0x05
  , 0x12, 0x03, 0x11, 0x01, 0x10
  , 0x00
};
static uint8_t g_u8RfPowerValue = PATABLE_VAL_DEF;
//static uint8_t g_u8RemainingTx  = UINT8_MIN_VALUE;
//static uint8_t g_u8TxRemaingRetry = DEFAULT_TX_RETRY;
//static uint8_t g_u8DelaymsbeforeRetry = DEFAULT_DELAYMS_BETWEEN_FRAME;
static volatile e_I2cState_t g_eI2CState  = I2CSTATE_WAIT_CMD;
static volatile uint8_t g_u8RxCounter     = UINT8_MIN_VALUE;
static volatile s_ESFrame_t g_sEsFrame    = { /* ES FRAME. */
  .au8Frame = {0},
  .u8FrameLen = UINT8_MIN_VALUE
};
static uint8_t g_au8TransmitBuffer[256] = {0};

/* =============================================================================
                        Public constants and variables
============================================================================= */
extern RF_SETTINGS rfSettings;


/* =============================================================================
                        Private function declarations
============================================================================= */
static void           vBoard_Init (void);
static void           vSystemClock (void);
static uint8_t        u8SearchRfPower (int8_t p_s8RfPower);
static e_FsmStates_t  eFsm_Sleep (void);
static e_FsmStates_t  eFsm_ProcessCmd (void);
static e_FsmStates_t  eFsm_RfTransmit (void);
static e_FsmStates_t  eFsm_SendResponse (e_ES_AckCode_t p_eAckCode);
static e_FsmStates_t  eCmdSetRfSettings (void);
//static e_FsmStates_t  eCmdRfTransmission (void);
static e_FsmStates_t  eCmdSetRfPower (void);
static e_FsmStates_t  eCmdSetOokCarrier (e_ExoBool_t p_eModulation);
static e_FsmStates_t  eCmdSetRfRetry (void);
static void           vSID_RFWaitState (uint8_t p_u8RequiredState);
static void           vSID_TimerA1_Stop (void);
static void           vSID_TimerA1_SetOutputMode (void);


/* =============================================================================
                               Public functions
============================================================================= */
                                     

/* =============================================================================
Function    :   main

Description :   Main program entry.

Parameters  :   None.

Return      :   Always 0.
============================================================================= */
int32_t main (void)
{
  /* Locals variables declaration. */
  e_ExoBool_t l_eLoop = EXOBOOL_TRUE;
  e_FsmStates_t l_eState = FSM_SLEEP;

  /* IO and clock initialization. */
  vBoard_Init ();

  /* Main loop. */
  do
  {
    /* Finite State Machine. */
    switch (l_eState)
    {
      case FSM_SLEEP:
        l_eState = eFsm_Sleep ();
        break;

      case FSM_PROCESS_COMMAND:
        l_eState = eFsm_ProcessCmd ();
        break;

      case FSM_RF_TRANSMIT:
        l_eState = eFsm_RfTransmit ();
        break;

      case FSM_SEND_RESPONSE_ACK:
        l_eState = eFsm_SendResponse (ES_ACK_OK);
        break;

      case FSM_SEND_RESPONSE_NACK:
        l_eState = eFsm_SendResponse (ES_ACK_NOK);
        break;

      case FSM_RESET:
      case FSM_INTERNAL_ERROR:
      default:
        l_eLoop = EXOBOOL_FALSE;
        break;
    }
  }
  while (EXOBOOL_TRUE == l_eLoop);

  /* Reset processor (with invalid fetch method ("call #0170h") ) */
  RESET ();

  return (0);
}


/* =============================================================================
                              Private functions
============================================================================= */

/* =============================================================================
Function    :   vSID_RF_Init

Description :   Radio core initialization.

Parameters  :   None.

Return      :   None.
============================================================================= */
static void vSID_RF_Init (void)
{
  /* Increase PMMCOREV level to 2 for proper radio operation. */
  SetVCore (2);

  /* Set the High-Power Mode Request Enable bit so LPM3 can be entered
     with active radio enabled */
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L; 
  PMMCTL0_H = 0x00; 

  ResetRadioCore ();    
  
  /* Write RF settings. */
  WriteRfSettings (&rfSettings);
  
  /* Clear pending interrupts and Enable TX end-of-packet interrupt. */
  RF1AIES |= BIT9;                          
  RF1AIFG &= ~BIT9;                         
  RF1AIE |= BIT9;

  /* Return to RF FSM IDLE state */
  Strobe (RF_SIDLE);

  /* Wait the end of RF initialization (Rf to Idle state) */
  vSID_RFWaitState (RF1A_MARCSTATE_IDLE);
}


/**
 * \public
 * \brief Return the state number from MARCSTATE status register.
 *
 * \return the state value. You cancheck it against the values in the enumeration RF1A_MARCSTATE_Id.
 *         even though not all the possible values are defined in this enumeration.
 */
static uint8_t RF1A_get_state(void)
{
  uint8_t v1, v2;
  // The state may change while being read. There is an errata note about this.
  // So read the value two consecutive times until thses values are the same.
  // This should fix the issue.
  do
  {
    v1 = ReadSingleReg(MARCSTATE);
    v2 = ReadSingleReg(MARCSTATE);
  }  while(v1 != v2);

  return v1;
}


/* =============================================================================
Function    :   vSID_RF_Stop

Description :   Stop Radio core.

Parameters  :   None.

Return      :   None.
============================================================================= */
static void	vSID_RF_Stop(void)
{
	// ---------------------------------------------------------------------------------
	//							 RESET THE CC1101 RF CONFIG
	// ---------------------------------------------------------------------------------

	 //RF1AIFCTL1	= 0x0014;
	 //RF1AINSTRW	= 0x0000;
	 //RF1ADINW	= 0x0000;

	 RF1AIN	= 0; ///RF1AIN_L	= 0x04;
	 RF1AIFG	= 0x0000;
	// ---------------------------------------------------------------------------------
	//							 RESET THE POWER MANAGMENT
	// ---------------------------------------------------------------------------------
	//PMMCTL0 = 0x9600; //SVSMHCTL = 0x4400; //SVSMHCTL = 0x4400; //PMMIFG = 0x8200;

	/* RAZ registre PMMCTL0 */
	PMMCTL0_H = 0xA5;
	PMMCTL0_L &= ~PMMHPMRE_L;
	PMMCTL0_H = 0x00;
														// Switch off RF :
	PMMCTL0_H = 0xA5;                         			// Open PMM module registers for write access
	SVSMLCTL = SVSLE + SVMLE;     						// Set SVS/M Low side to new level
	while ((PMMIFG & SVSMLDLYIFG) == 0);      			// Wait till SVM is settled (Delay)
	PMMCTL0_L = 0;          							// Set VCore to 1.85 V for Max Speed.
	PMMCTL0_H = 0x00;                         			// Lock PMM module registers for write access

	/* RAZ registre SVSMHCTL */
	PMMCTL0_H = 0xA5;
	SVSMHCTL &= ~SVMHFP;
	PMMCTL0_H = 0x00;

	PMMCTL0_H = 0xA5;
	SVSMHCTL = SVSHRVL_0 + SVSMHRRL_0;
	PMMCTL0_H = 0x00;

	PMMCTL0_H = 0xA5;
	SVSMHCTL |= (SVMHE + SVSHE + SVSHMD);				// Activate with LPM3
	PMMCTL0_H = 0x00;

	/* RAZ registre SVSMLCTL */
	PMMCTL0_H = 0xA5;
	SVSMLCTL &= ~SVMLFP;
	PMMCTL0_H = 0x00;

	PMMCTL0_H = 0xA5;
	SVSMLCTL = SVSLRVL_0 + SVSMLRRL_0;
	PMMCTL0_H = 0x00;

	PMMCTL0_H = 0xA5;
	SVSMLCTL |= (SVMLE + SVSLE + SVSLMD);				// Activate with LPM3
	PMMCTL0_H = 0x00;

	/* RAZ registre PMMIFG */
	Strobe(RF_SRES);                       // resets the radio and sets the state machine to SLEEP
	Strobe(RF_SNOP);                       // Reset Radio Pointer
 }


/* =============================================================================
Function    :   vBoard_Init

Description :   Board initialization.

Parameters  :   None.

Return      :   None.
============================================================================= */
static void vBoard_Init (void)
{
  /* Stop watch dog timer. */
  WDTCTL = (uint16_t)(WDTPW | WDTHOLD);
  
  /* Initialize outputs level. */
  P1OUT = PORTx_LOW_LEVEL;
  P2OUT = PORTx_LOW_LEVEL;
  P3OUT = PORTx_LOW_LEVEL;
  PJOUT = PORTx_LOW_LEVEL;

  /* Disable all Pull up/down. */
  P1REN = PORTx_PULL_DISABLE;
  P2REN = PORTx_PULL_DISABLE;
  P3REN = PORTx_PULL_DISABLE;

  /* Initialize pin function. */
  P1SEL = GPIO_UCB0SDA | GPI_UCB0SCL;
  P2SEL = UINT8_MIN_VALUE;
  P3SEL = UINT8_MIN_VALUE;

  /* Initialize direction. */
  P1DIR = GPO_NOT_READY | BIT0 | BIT1 | BIT4 | BIT5| BIT7;
  P2DIR = PORTx_DIR_OUTPUT;
  P3DIR = PORTx_DIR_OUTPUT;
  PJDIR = PORTJ_DIR_OUTPUT;

  /* Initialize system clock. */
  vSystemClock ();

  /* Enable software reset before change UCB0 registers. */
  UCB0CTL1 |= UCSWRST;

  /* I2C slave, synchronous mode. */
  UCB0CTL0 = UCMODE_3 + UCSYNC;

  /* Set owner address. */
  UCB0I2COA = I2C_MY_ADDRESS;

  /* Disable software reset agter change UCB0 registers. */
  UCB0CTL1 &= ~UCSWRST;

  /* Clear interrupt flag. */
  UCB0IFG = UINT8_MIN_VALUE;

  /* Enable Start, Stop, Rx and Tx I2C interrupts. */
  UCB0IE = UCSTPIE + UCSTTIE + UCRXIE + UCTXIE;
}


/* =============================================================================
Function    :   vSystemClock

Description :   Configure CC430 system clock (low power).

Parameters  :   None.

Return      :   None.
============================================================================= */
static void vSystemClock (void)
{
  /* Disable XT1 using. */
  P5SEL = UINT8_MIN_VALUE;
  P5OUT = UINT8_MIN_VALUE;
  P5DIR = P5DIR_DIR_OUTPUT;

  /* Disable unused clock. */
  UCSCTL8 &= (uint8_t)(~(ACLKREQEN | SMCLKREQEN) );

  /* Set REFOCLK as FLL reference. */
  UCSCTL3 |= SELREF_2;

  /* ACLK = VLO ; VLO = 9.4 kHz ; SELM__DCOCLKDIV. */
  UCSCTL4 |= SELA__VLOCLK + SELS__DCOCLKDIV;

  /* ACLK = VLO/32. */
  UCSCTL5 |= DIVA__32;

  /* SMCLK/2. */
  UCSCTL5 |= DIVS__2;

  /* Disable the FLL control loop. */
  __bis_SR_register (SCG0);

  /* Set lowest possible DCOx, MODx. */
  UCSCTL0 = UINT16_MIN_VALUE;

  /* Select DCO range 24MHz operation. */
  UCSCTL1 = DCORSEL_5;

  /* Set DCO Multiplier and FLL Div = fDCOCLK/2. */
  UCSCTL2 = FLLD_1 + (BOARD_DCO_MULT - 1u);

  /* Enable the FLL control loop. */
  __bic_SR_register (SCG0);

  /* Worst-case settling time for the DCO when the DCO range bits have been
   changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
   UG for optimization. */
  __delay_cycles (DELAY_AFTER_FLL_SET);

  /* Loop until XT1,XT2 & DCO fault flag is cleared. */
  do
  {
    /* Clear XT2,XT1,DCO fault flags. */
    UCSCTL7 &= (uint16_t) ~(uint16_t)(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);

    /* Clear fault flags. */
    SFRIFG1 &= (uint16_t) ~(uint16_t)OFIFG;

    /* Test oscillator fault flag. */
  } while (SFRIFG1 & OFIFG);
}


/* =============================================================================
Function    :   eFsm_Sleep

Description :   Enter in LPM4 and wake up with I2C received command.

Parameters  :   None.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eFsm_Sleep (void)
{
  /* Locals variables declaration. */
  e_FsmStates_t l_eNextState = FSM_INTERNAL_ERROR;

  /* I2C will received command. */
  g_eI2CState = I2CSTATE_WAIT_CMD;

  /* Low power mode 4 with interrupts enabled. */
  __bis_SR_register (LPM3_bits + GIE);

  /* I2C will send reply. */
  g_eI2CState = I2CSTATE_WAIT_FOR_REPLY;

  /* CC430 not ready, processing command... */
  P1OUT |= GPO_NOT_READY;

  /* Save size of command. */
  g_sEsFrame.u8FrameLen = g_u8RxCounter;

  /* Update FSM. */
  l_eNextState = FSM_PROCESS_COMMAND;

  return (l_eNextState);
}


/* =============================================================================
Function    :   eFsm_ProcessCmd

Description :   Processed MASTER I2C received command.

Parameters  :   None.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eFsm_ProcessCmd (void)
{
  /* Locals variables declaration. */
  e_FsmStates_t l_eNextState = FSM_INTERNAL_ERROR;

  /* Is valid ES protocol frame ? */
  if (ESP_ValidFrame == ESP_CheckForValidFrame (  (uint8_t *)g_sEsFrame.au8Frame
                                                , (uint8_t)g_sEsFrame.u8FrameLen) )
  {
    /* Is it for us ? */
    if (ESP_HOST_CC430_NUM == ESP_GetRecipient ( (uint8_t *)g_sEsFrame.au8Frame) )
    {
      /* Which command ? */
      switch (ESP_GetCommand ( (uint8_t *)g_sEsFrame.au8Frame) )
      {
        case CMD_RESET:
        case CMD_GET_FIRMWARE_VERSION:
        case CMD_GET_COMPILATION_DATA:
        case CMD_RF_SETTINGS_GET:
          l_eNextState = FSM_SEND_RESPONSE_ACK;
          break;

        case CMD_RF_SETTINGS_SET:
          l_eNextState = eCmdSetRfSettings ();
           break;

        case CMD_RF_TRANSMISSION:
           l_eNextState = eFsm_RfTransmit(); //eCmdRfTransmission ();
          break;

        case CMD_RF_POWER_SET:
          l_eNextState = eCmdSetRfPower ();
          break;

        case CMD_RF_OOK_CARRIER_SET:
          l_eNextState = eCmdSetOokCarrier (EXOBOOL_FALSE);
          break;

        case CMD_RF_MOD_CARRIER_SET:
          l_eNextState = eCmdSetOokCarrier (EXOBOOL_TRUE);
          break;

        case CMD_RF_RETRY_SET:
          l_eNextState = eCmdSetRfRetry ();
          break;

        default:
          l_eNextState = FSM_SEND_RESPONSE_NACK;
          break;
      }
    }
    else
    {
      l_eNextState = FSM_SLEEP;
    }
  }
  else
  {
    l_eNextState = FSM_SLEEP;
  }

  return (l_eNextState);
}


/* =============================================================================
Function    :   eFsm_RfTransmit

Description :   Transmit data over RF.

Parameters  :   None.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eFsm_RfTransmit (void)
{
  /* Locals variables declaration. */
  e_FsmStates_t l_eNextState = FSM_INTERNAL_ERROR;
  uint8_t *l_pau8Payload      = ESP_GetPayload ( (uint8_t *)g_sEsFrame.au8Frame);
  uint8_t l_u8PayloadLenght   = ESP_GetPayloadLength ( (uint8_t *)g_sEsFrame.au8Frame) - I2CCMD_INFO_LEN;

  /* Check for payload presence. */
  if (   (EXO_PNULL != l_pau8Payload)
      && (UINT8_MIN_VALUE < l_u8PayloadLenght) )
  {
    /* Initialize radio core. */
    vSID_RF_Init ();

    /* Get RF power. */
    g_u8RfPowerValue = u8SearchRfPower ( (int8_t)l_pau8Payload[I2CCMD_RF_POWER_POS]);
    WriteSinglePATable (g_u8RfPowerValue);

    /* Make packet. */
    g_au8TransmitBuffer[0] = l_u8PayloadLenght + SIDFRAME_HEADER_LEN + 1;
    g_au8TransmitBuffer[1] = 0xD4;
    g_au8TransmitBuffer[2] = SIDFRAME_SID_ID;
    g_au8TransmitBuffer[3] = l_u8PayloadLenght + 1;
    memcpy (&g_au8TransmitBuffer[4], l_pau8Payload, l_u8PayloadLenght);

    /* Write it into TX fifo. */
    WriteBurstReg (RF_TXFIFOWR, g_au8TransmitBuffer, g_au8TransmitBuffer[0] + 1u);

    /* enable TX if in IDLE state and perform calibration first if MCSM0.FS_AUTOCAL = 1 */
    Strobe (RF_STX);    

    /* Set low power mode 4 with interrupts enable. */
    __bis_SR_register (LPM3_bits + GIE);
    __no_operation(); 

    /* Return to RF FSM IDLE state */
    Strobe (RF_SIDLE);

    /* Wait the end of RF initialization (Rf to Idle state) */
    vSID_RFWaitState (RF1A_MARCSTATE_IDLE);

    /* Return to RF FSM SLEEP state */
    Strobe (RF_SPWD);  

    /* Stop RF core. */
    vSID_RF_Stop ();
    
    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_ACK;
  }
  else
  {
    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_NACK;
  }

  return (l_eNextState);
}


/* =============================================================================
Function    :   eFsm_SendResponse

Description :   Prepare I2C response to MASTER.

Parameters  :   None.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eFsm_SendResponse (e_ES_AckCode_t p_eAckCode)
{
  /* Locals variables declaration. */
  uint8_t l_au8Temp[I2CREPLY_MAX_FRAME_LEN] = {UINT8_MIN_VALUE};
  uint8_t l_u8Datalength                    = I2CREPLY_INFO_LEN;
  e_FsmStates_t l_eNextState                = FSM_INTERNAL_ERROR;

  /* Set last command. */
  l_au8Temp[I2CREPLY_ACK_POS] = p_eAckCode;
  l_au8Temp[I2CREPLY_CMD_POS] = ESP_GetCommand ((uint8_t *)g_sEsFrame.au8Frame);

  /* Response manager. */
  switch (l_au8Temp[I2CREPLY_CMD_POS])
  {
    case CMD_GET_FIRMWARE_VERSION:
      l_au8Temp[I2CREPLY_PAYLOAD_POS]      = SOFTWARE_VERSION_MAJOR;
      l_au8Temp[I2CREPLY_PAYLOAD_POS + 1u] = SOFTWARE_VERSION_MINOR;
      l_au8Temp[I2CREPLY_PAYLOAD_POS + 2u] = SOFTWARE_VERSION_REVISION;
      l_u8Datalength += I2CREPLY_FW_VERS_LEN;
      break;

    case CMD_GET_COMPILATION_DATA:
      memcpy (  &l_au8Temp[I2CREPLY_PAYLOAD_POS]
              , g_cachCompileDate
              , sizeof (g_cachCompileDate) );
      memcpy (  &l_au8Temp[I2CREPLY_PAYLOAD_POS + sizeof (g_cachCompileDate)]
              , g_cachCompileTime
              , sizeof (g_cachCompileTime) );
      l_u8Datalength += (sizeof (g_cachCompileDate) + sizeof (g_cachCompileTime) );

      break;

    case CMD_RF_SETTINGS_GET:
      ReadRfSettings ((RF_SETTINGS *)&l_au8Temp[I2CREPLY_PAYLOAD_POS]);
     // memcpy (&l_au8Temp[I2CREPLY_PAYLOAD_POS], &rfSettings, sizeof (rfSettings) );
      l_u8Datalength += sizeof (rfSettings);
      break;

    default:
      /* Nothing to do. */
      break;
  }

  /* Build ES frame. */
  g_sEsFrame.u8FrameLen = ESP_BuildCommand (  (uint8_t *)g_sEsFrame.au8Frame
                                            , ESP_HOST_CC430_NUM
                                            , ESP_GetExpeditor ( (uint8_t *)g_sEsFrame.au8Frame)
                                            , CMD_ACKNOWLEDGE
                                            , l_au8Temp
                                            , l_u8Datalength);
  /* Command processed... */
  P1OUT &= (~GPO_NOT_READY);

  /* Reset command ? */
  if (CMD_RESET == l_au8Temp[I2CREPLY_CMD_POS])
  {
  /* Make delay for answer before reset. */
    __delay_cycles (DELAY_BEFORE_RESET);
  }
  else
  {
    /* Nothing to do. */
  }

  /* Update state machine value. */
  l_eNextState = FSM_SLEEP;

  return (l_eNextState);
}


/* =============================================================================
Function    :   eCmdSetRfSettings

Description :   Set new RF settings.

Parameters  :   None.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eCmdSetRfSettings (void)
{
  /* Locals variables declaration. */
  RF_SETTINGS *l_pRfSettings = (RF_SETTINGS *)(ESP_GetPayload ( (uint8_t *)g_sEsFrame.au8Frame) );
  e_FsmStates_t l_eNextState = FSM_INTERNAL_ERROR;

  /* Check for payload presence. */
  if (EXO_PNULL != l_pRfSettings)
  {
    /* Set new settings. */
    WriteRfSettings (l_pRfSettings);

    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_ACK;
  }
  else
  {
    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_NACK;
  }

  return (l_eNextState);
}

#if 0
/* =============================================================================
Function    :   eCmdRfTransmission

Description :   Search correct RF power, and encode TAG timing before transmit.

Parameters  :   None.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eCmdRfTransmission (void)
{

  /* Locals variables declaration. */
  uint8_t *l_pau8Payload      = ESP_GetPayload ( (uint8_t *)g_sEsFrame.au8Frame);
  uint8_t l_u8PayloadLenght   = ESP_GetPayloadLength ( (uint8_t *)g_sEsFrame.au8Frame) - I2CCMD_INFO_LEN;
  e_FsmStates_t l_eNextState  = FSM_INTERNAL_ERROR;

  /* Check for payload presence. */
  if (   (EXO_PNULL != l_pau8Payload)
      && (UINT8_MIN_VALUE < l_u8PayloadLenght) )
  {
    /* Get RF power. */
    g_u8RfPowerValue = u8SearchRfPower ( (int8_t)l_pau8Payload[I2CCMD_RF_POWER_POS]);

    /* Encode bit stream. */
    if (EXOERRORS_NO == eSID_EncodeBuffer (  &l_pau8Payload[I2CCMD_PAYLOAD_POS]
                                           , l_u8PayloadLenght
                                           , (TagTypes_t)l_pau8Payload[I2CCMD_TAG_POS]) )
    {
      /* Set TX remaining number. */
      g_u8RemainingTx = g_u8TxRemaingRetry;

      /* Begin RF transmit. */
      l_eNextState = FSM_RF_TRANSMIT;
    }
    else
    {
      /* Send command response. */
      l_eNextState = FSM_SEND_RESPONSE_NACK;
    }
  }
  else
  {
    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_NACK;
  }

  return (l_eNextState);
}
#endif

/* =============================================================================
Function    :   eCmdSetRfPower

Description :   Set RF power.

Parameters  :   None.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eCmdSetRfPower (void)
{
  /* Locals variables declaration. */
  uint8_t *l_pau8Payload      = ESP_GetPayload ( (uint8_t *)g_sEsFrame.au8Frame);
  e_FsmStates_t l_eNextState  = FSM_INTERNAL_ERROR;

  /* Input parameters checking. */
  if (EXO_PNULL != l_pau8Payload)
  {
    /* Update PA value. */
    g_u8RfPowerValue = u8SearchRfPower ( (int8_t)l_pau8Payload[BUFFER_IDX0]);

    /* Set PA value */
    WriteSinglePATable (g_u8RfPowerValue);

    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_ACK;
  }
  else
  {
    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_NACK;
  }

  return (l_eNextState);
}


/* =============================================================================
Function    :   eCmdSetOokCarrier

Description :   Enable/Disable OOk carrier with/without modulation.

Parameters  :   p_eModulation = EXOBOOL_TRUE to activate modulation.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eCmdSetOokCarrier (e_ExoBool_t p_eModulation)
{
  /* Locals variables declaration. */
  uint8_t l_u8MDMCFG2         = UINT8_MIN_VALUE;
  uint8_t *l_pau8Payload      = ESP_GetPayload ( (uint8_t *)g_sEsFrame.au8Frame);
  e_FsmStates_t l_eNextState  = FSM_INTERNAL_ERROR;

  /* Input parameters checking. */
  if (EXO_PNULL != l_pau8Payload)
  {
    /* Security key. */
    PMMCTL0_H = PMMPW_H;
    PMMCTL0_L |= PMMHPMRE_L;
    PMMCTL0_H = UINT8_MIN_VALUE;

    /* Reset radio core. */
    ResetRadioCore ();

    /* Activate OOK ?*/
    if (EXOBOOL_TRUE == l_pau8Payload[BUFFER_IDX0])
    {
      /* Save current RF value. */
      l_u8MDMCFG2 = rfSettings.mdmcfg2;

      /* Set 2-FSK modulation. */
      rfSettings.mdmcfg2 &= ~RF1A_MDMCFG2_MOD_FORMAT;

      /* Deactivate modulation ? */
      if (EXOBOOL_FALSE == p_eModulation)
      {
        /* Set ASK/OOK modulation. */
        rfSettings.mdmcfg2 |=  RF1A_MODULATION_ASK_OOK;
      }
      else
      {
        /* Nothing to do. */
      }

      /* Set RF settings. */
      WriteRfSettings (&rfSettings);

      /* Set in IDLE mode. */
      Strobe (RF_SIDLE);

      /* Write PA value. */
      WriteSinglePATable (g_u8RfPowerValue);

      /* Switch RF to TX mode */
      WriteSingleReg (IOCFG0, GDOx_TRISTATE);

      /* Move RF FSM to RF_SFSTXON state */
      Strobe (RF_SFSTXON);
      vSID_RFWaitState (RF1A_MARCSTATE_FSTXON);

      /* Move RF FSM to RF_STX (transmit) state */
      Strobe (RF_STX);
      vSID_RFWaitState (RF1A_MARCSTATE_TXON);

      /* Configure and start timer A1. */
      TA1CCR0  = UINT16_MAX_VALUE;

      /* Force output to logic 1. */
      TA1CCTL1 = OUTMOD_0 + (EXOBIT_ONE << 2u);
      vSID_TimerA1_SetOutputMode ();

      /* Restore  RF value. */
      rfSettings.mdmcfg2 = l_u8MDMCFG2;
    }
    else
    {
      /* Stop and reset timer A1. */
      vSID_TimerA1_Stop ();

      /* Return to RF FSM IDLE state */
      Strobe (RF_SIDLE);

      /* Wait the end of RF initialization (Rf to Idle state) */
      vSID_RFWaitState (RF1A_MARCSTATE_IDLE);

      /* Return to RF FSM SLEEP state */
      Strobe (RF_SPWD);

      /* Stop RF. */
      vSID_RF_Stop ();
    }

    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_ACK;
  }
  else
  {
    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_NACK;
  }

  return (l_eNextState);
}


/* =============================================================================
Function    :   eCmdSetRfRetry

Description :   Set RF retry and delay for selected tag.

Parameters  :   None.

Return      :   Next FSM step.
============================================================================= */
static e_FsmStates_t eCmdSetRfRetry (void)
{
  /* Locals variables declaration. */
  uint8_t *l_pau8Payload      = ESP_GetPayload ( (uint8_t *)g_sEsFrame.au8Frame);
  uint8_t l_u8PayloadLenght   = ESP_GetPayloadLength ( (uint8_t *)g_sEsFrame.au8Frame);
  uint8_t l_u8SelectedTag     = NO_TAG;
  uint16_t l_u16Retry         = UINT16_MIN_VALUE;
  uint16_t l_u16Delay         = UINT16_MIN_VALUE;
  e_FsmStates_t l_eNextState  = FSM_INTERNAL_ERROR;

  /* Check for payload presence. */
  if ( (EXO_PNULL != l_pau8Payload) && (UINT8_MIN_VALUE < l_u8PayloadLenght) )
  {
    /* Get current tag. */
    l_u8SelectedTag = l_pau8Payload[I2CCMD_TAG_POS];
    if (MAX_TAG > l_u8SelectedTag)
    {
      /* Get retry and delay. */
      l_u16Retry = l_pau8Payload[I2CCMD_RETRY_POS];
      l_u16Delay = l_pau8Payload[I2CCMD_DELAYMS_POS];

      /* Valid totals times ? */
      if (MAX_TX_ALLOWED >= ( (l_u16Retry+1u) * l_u16Delay) )
      {
        /* Save retry and delay. */
         //g_u8TxRemaingRetry = (uint8_t)l_u16Retry;
        // g_u8DelaymsbeforeRetry = (uint8_t)l_u16Delay;

        /* Send command response. */
        l_eNextState = FSM_SEND_RESPONSE_ACK;
      }
      else
      {
        /* Send command response. */
        l_eNextState = FSM_SEND_RESPONSE_NACK;
      }
    }
    else
    {
      /* Send command response. */
      l_eNextState = FSM_SEND_RESPONSE_NACK;
    }
  }
  else
  {
    /* Send command response. */
    l_eNextState = FSM_SEND_RESPONSE_NACK;
  }

  return (l_eNextState);
}


/* =============================================================================
Function    :   u8SearchRfPower

Description :   Search RF power inside LUT (dichotomy method).

Parameters  :   None.

Return      :   RF power value.
============================================================================= */
static uint8_t u8SearchRfPower (int8_t p_s8RfPower)
{
  /* Locals variables declaration. */
  uint8_t l_u8MaxTry        = SEARCHPOWER_TRY;
  uint8_t l_u8IndexMax      = SEARCHPOWER_INDEX_MAX;
  uint8_t l_u8IndexMin      = SEARCHPOWER_INDEX_IN;
  uint8_t l_u8IndexCurrent  = UINT8_MIN_VALUE;

  /* Search loop. */
  do
  {
    /* Compute middle range. */
    l_u8IndexCurrent = (l_u8IndexMax + l_u8IndexMin)>>1u;

    /* Is lower ? */
    if (p_s8RfPower < g_cas8RfPowerdBm[l_u8IndexCurrent])
    {
      /* Update range min. */
      l_u8IndexMin = l_u8IndexCurrent;

      l_u8MaxTry--;
    }
    else if (p_s8RfPower > g_cas8RfPowerdBm[l_u8IndexCurrent]) /* Upper ? */
    {
      /* Update range max. */
      l_u8IndexMax = l_u8IndexCurrent;

      l_u8MaxTry--;
    }
    else
    {
      /* Value found. */
    }
  }
  while ( (p_s8RfPower != g_cas8RfPowerdBm[l_u8IndexCurrent]) && (UINT8_MIN_VALUE < l_u8MaxTry) );

  return (g_cau8PatableValue[l_u8IndexCurrent]);
}


/* =============================================================================
Function    :   vSID_RFWaitState

Description :   Wait the require RF state (blocking !!!).

Parameters  :   p_u8RequiredState = require RF state.

Return      :   None.
============================================================================= */
static void vSID_RFWaitState (uint8_t p_u8RequiredState)
{
  /* Locals variables declaration. */
  uint8_t l_u8CurrentState = UINT8_MIN_VALUE;

  /* Wait the require RF state. */
  do
  {
    l_u8CurrentState = RF1A_get_state ();
  }
  while (l_u8CurrentState != p_u8RequiredState);
}


/* =============================================================================
Function    :   vSID_TimerA0_Stop

Description :   Stop and reset timer A1 values.

Parameters  :   None.

Return      :   None.
============================================================================= */
static void vSID_TimerA1_Stop (void)
{
  /* Initialize timer to default value. */
  TA1CCR0  = TA1CCR0_DEFAULT_VALUE;
  TA1CCR1  = TA1CCR1_DEFAULT_VALUE;
  TA1CTL   = TA1CTL_DEFAULT_VALUE;
  TA1CCTL0 = TA1CCTL0_DEFAULT_VALUE;
}


/* =============================================================================
Function    :   vSID_TimerA1_SetOutputMode

Description :   Set timer A1 (RF timer) in output mode.

Parameters  :   None.

Return      :   None.
====================/========================================================= */
static void vSID_TimerA1_SetOutputMode (void)
{
  /* INCLK (RF 26MHz) as clock source */
  /* Timer counts up to TAxCCR0 */
  /* Clear timer clock divider logic, and the count direction */
  /* Input divider/2 */
  TA1CTL = TASSEL__INCLK + MC_1 + TACLR  + ID_0;

  /* The output is toggled when the timer counts to the TAxCCRn value. */
  /* The output period is double the timer period. */
  /* Clear Capture/compare interrupt flag */
  TA1CCTL0 = OUTMOD_4 + CCIFG;
}


/* =============================================================================
Function    :   vUSCI_B0_UserIsr

Description :   User I2C Interrupt Service Routine.

Parameters  :   None.

Return      :   None.
============================================================================= */
#pragma vector = USCI_B0_VECTOR
__interrupt void vUSCI_B0_UserIsr (void)
{
  /* Locals variables declaration. */
  static uint8_t g_u8TxCounter = UINT8_MIN_VALUE;

  switch (__even_in_range (UCB0IV, USCI_I2C_UCTXIFG) )
  {
    case USCI_NONE:                                   /* No Interrupt pending */
      break;

    case USCI_I2C_UCALIFG:                               /* Arbitration lost. */
      break;

    case USCI_I2C_UCNACKIFG:                           /* Not acknowledgment. */
      break;

    case  USCI_I2C_UCSTTIFG:                     /* Start condition received. */
      UCB0IFG &= ~UCSTTIFG;
      g_u8TxCounter = UINT8_MIN_VALUE;
      g_u8RxCounter = UINT8_MIN_VALUE;
      break;

    case  USCI_I2C_UCSTPIFG:                      /* Stop condition received. */
      UCB0IFG &= ~UCSTPIFG;

      /* Wait command state ? */
      if (I2CSTATE_WAIT_CMD == g_eI2CState)
      {
        /* Wake up. */
        __bic_SR_register_on_exit (LPM3_bits);
      }
      break;

    case USCI_I2C_UCRXIFG:                                  /* Data received. */
      g_sEsFrame.au8Frame[g_u8RxCounter++] = UCB0RXBUF;
      break;

    case USCI_I2C_UCTXIFG:                          /* Transmit buffer empty. */
      UCB0TXBUF = g_sEsFrame.au8Frame[g_u8TxCounter++];
      break;

    default:
      break;
  }
}


#pragma vector=CC1101_VECTOR
__interrupt void CC1101_ISR (void)
{
  switch (__even_in_range(RF1AIV, 32) )        // Prioritizing Radio Core Interrupt 
  {
    case  0: break;                         // No RF core interrupt pending                                            
    case  2: break;                         // RFIFG0 
    case  4: break;                         // RFIFG1
    case  6: break;                         // RFIFG2
    case  8: break;                         // RFIFG3
    case 10: break;                         // RFIFG4
    case 12: break;                         // RFIFG5
    case 14: break;                         // RFIFG6          
    case 16: break;                         // RFIFG7
    case 18: break;                         // RFIFG8
    case 20:                                // RFIFG9
      RF1AIE &= ~BIT9;                    // Disable TX end-of-packet interrupt
      break;
    case 22: break;                         // RFIFG10
    case 24: break;                         // RFIFG11
    case 26: break;                         // RFIFG12
    case 28: break;                         // RFIFG13
    case 30: break;                         // RFIFG14
    case 32: break;                         // RFIFG15
  }  
  __bic_SR_register_on_exit (LPM3_bits);
}

