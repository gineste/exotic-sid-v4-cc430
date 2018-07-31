/* =============================================================================
 ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
(  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
(____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/

  Copyright (c) 2017 EXOTIC SYSTEMS. All Rights Reserved.

  Licensees are granted free, non-transferable use of the information. NO
  WARRANTY of ANY KIND is provided. This heading must NOT be removed from
  the file.

  File name:    boardconfig.h
  Date:         02 01 2018
  Author:       Emeric L.
  Description:  SID V3 project (Board configuration) - Header file.
============================================================================= */
#ifndef BOARDCONFIG__H
  #define BOARDCONFIG__H

  /* ===========================================================================
                                   DEBUG Section
  =========================================================================== */

  /* ===========================================================================
                            Public defines and typedefs
  =========================================================================== */
  #define SOFTWARE_VERSION_MAJOR    ((uint8_t)1)
  #define SOFTWARE_VERSION_MINOR    ((uint8_t)0)
  #define SOFTWARE_VERSION_REVISION ((uint8_t)4)
 
  #define BOARD_XT1_FREQ            ((uint32_t)32768)
  #define BOARD_DCO_MULT            ((uint32_t)375)
  #define BOARD_SMCCLK_FREQ         ((uint32_t)(BOARD_XT1_FREQ*BOARD_DCO_MULT))

  /* ===========================================================================
                          Public constants and variables
  =========================================================================== */

  /* ===========================================================================
                          Public function declarations
  =========================================================================== */

#endif /* BOARDCONFIG__H */
