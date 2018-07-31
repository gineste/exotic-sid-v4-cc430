/* =============================================================================
 ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
(  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
(____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/

  Copyright (c) 2017 EXOTIC SYSTEMS. All Rights Reserved.

  Licensees are granted free, non-transferable use of the information. NO
  WARRANTY of ANY KIND is provided. This heading must NOT be removed from
  the file.

  File name:    tpms_sid.h
  Date:         14 03 2018
  Author:       Emeric L.
  Description:  SID TPMS decode/encoder - Header file.
============================================================================= */
#ifndef TPMS_SID__H
  #define TPMS_SID__H

  /* ===========================================================================
                                   DEBUG Section
  =========================================================================== */

  /* ===========================================================================
                            Public defines and typedefs
  =========================================================================== */
  
  /* ===========================================================================
                          Public constants and variables
  =========================================================================== */

  /* ===========================================================================
                          Public function declarations
  =========================================================================== */
#ifdef TPMS_USE_ENCODER
  uint16_t 	TPMS_SID_EncodeFrameToSend (CaptureData_t *captureData, bytesData_t *encodedData);
#endif /* TPMS_USE_ENCODER */

#endif /* TPMS_SID__H */

