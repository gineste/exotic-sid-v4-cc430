/******************** (C) COPYRIGHT 2015 exoTIC Systems ************************
* File Name          : es_interfaces.c
* Description        : EXOTIC SYSTEMS Protocol Interfaces.
*******************************************************************************/

/* Multi-include protection --------------------------------------------------*/
#ifndef _ES_INTERFACES_C_
#define _ES_INTERFACES_C_

#ifdef __cplusplus
extern "C" {
#endif
  
  /* Includes ------------------------------------------------------------------*/
#include <ES_Protocol/ES_Interfaces.h>
#include <ES_Protocol/ES_Protocol.h>
  
  /* Private constants ---------------------------------------------------------*/
  
  /* Private macro -------------------------------------------------------------*/
  
  /* Private types -------------------------------------------------------------*/
  
  
  /* Private Variables ---------------------------------------------------------*/
  
  /* Storage for all the interface to use */
  static ESI_Interface_t *ESI_Interfaces;
  static uint32_t 		ESI_Interfaces_Count = 0u;;
  
  /* Private functions ---------------------------------------------------------*/
  
  /* Public functions ----------------------------------------------------------*/
  
  /**
  * @brief  Initialise the ES_INTERFACES module.
  *
  * @param  None.
  * @return None.
  */
  void ESI_Init(ESI_Interface_t * interfaces, uint32_t itf_count)
  {
    /* Initialize structure */
    uint8_t i;
    ESI_Interfaces_Count = itf_count;
    ESI_Interfaces = interfaces;
    
    for ( i=0u; i<ESI_Interfaces_Count; ++i )
    {
      ESI_Interfaces[i].timeOut       = 0u;
      ESI_Interfaces[i].physicalItf   = 0u;
      ESI_Interfaces[i].io            = (ESI_IO_t*) 0u;
    }
  }
  
  /**
  * @brief  Add a new Interface for protocol.
  *
  * @param  itf  Number of the interface.
  * @param  io   Pointer on the io interface.
  * @return ESI_Succeed or ESI_Failed.
  */
  ESI_ReturnCode_t ESI_AddInterface(uint8_t itf, ESI_IO_t * io)
  {
    ESI_ReturnCode_t AddInterface_retVal_ = ESI_Failed;
    if ( (itf < ESI_Interfaces_Count) && (io && io->readBuffer && io->recvFrame && io->releaseBuffer))
    {
      ESI_Interfaces[itf].io           = io;
      ESI_Interfaces[itf].io->Out.len   = 0u;
      AddInterface_retVal_ = ESI_Succeed;
    }else{
      /* Wrong settings, cant continue */
      while(1)
      {
      }
    }
    return AddInterface_retVal_;
  }
  
  /**
  * @brief  Send a frame through protocol.
  *
  * @param  itf     Number of the interface.
  * @param  opCode  OpCode for frame.
  * @param  expdt   expeditor.
  * @param  recpt   recipient.
  * @param  payload data to send.
  * @param  len     len of data.
  * @return ESI_Succeed or ESI_Failed.
  */
  ESI_ReturnCode_t ESI_SendFrame(uint8_t itf,
                                 uint8_t opCode,
                                 uint32_t expdt,
                                 uint32_t recpt,
                                 const uint8_t payload[], uint16_t len)
  {
    ESI_ReturnCode_t SendFrame_retVal_ = ESI_Failed;
    ESI_Interface_t * interface = &(ESI_Interfaces[itf]);
    
    if(expdt == recpt)
    {
      SendFrame_retVal_ = ESI_Succeed;
    }else{
      if((interface->io->lock == 0) || (interface->io->lock() == ESI_Succeed))
      {
        interface->io->Out.len = ESP_BuildCommand(*(interface->io->Out.data),
                                                  expdt,
                                                  recpt,
                                                  opCode,
                                                  payload, len );
        
        if ( (interface->io != 0u) && (interface->io->writeBuffer != 0u) )
        {
          SendFrame_retVal_ = (ESI_ReturnCode_t) interface->io->writeBuffer(*(interface->io->Out.data), interface->io->Out.len );
        }
        
        if(interface->io->unlock)
        {
          interface->io->unlock();
        }
      }
    }
    
    return SendFrame_retVal_;
  }
  
  /**
  * @brief  Read a received buffer through protocol.
  *
  * @param  itf     Number of the interface.
  * @param  buffer  buffer received.
  * @param  len     lenght of data
  * @return ESI_BadFrame,
  *         ESI_Overflow,
  *         ESI_Receiving or
  *         ESI_ValidFrame.
  */
  ESI_ReturnCode_t ESI_CheckForFrame(uint8_t itf)
  {
    /* DEBUG : Counter for received frame */
    static uint32_t ESI_cpt = 0u;
    uint8_t 		* reading_buffer_;
    ESI_Interface_t * interface = &(ESI_Interfaces[itf]);
    
    ESP_ReturnCode_t CheckForFrame_flag_ = ESP_Receiving;
    ESI_ReturnCode_t CheckForFrame_retVal_ = ESI_Failed;
    
    if(interface->io->readBuffer && interface->io->recvFrame && interface->io->releaseBuffer)
    {
      reading_buffer_ = interface->io->readBuffer(120u);
      
      CheckForFrame_flag_= ESP_CheckForValidFrame(reading_buffer_, 120u);
      
      switch(CheckForFrame_flag_)
      {
      case ESP_Receiving:
        break;
        
      case ESP_ValidFrame:
        ++ESI_cpt;
        
        /*
        * If we are sending multiframe we must ignore CC430 answers since only
        * the last one have to be forwarded to client.
        */
        interface->io->recvFrame(
                                 ESP_GetCommand(reading_buffer_),
                                 ESP_GetExpeditor(reading_buffer_),
                                 ESP_GetRecipient(reading_buffer_),
                                 ESP_GetPayload(reading_buffer_),
                                 ESP_GetPayloadLength(reading_buffer_)
                                   );
        
        interface->io->releaseBuffer();
        CheckForFrame_retVal_ = ESI_Succeed;
        break;
        
      case ESP_Failed:
        interface->io->releaseBuffer();
        CheckForFrame_retVal_ = ESI_Succeed;
        break;
        
      default:
        break;
      }
    }
    
    return CheckForFrame_retVal_;
  }
  
#ifdef __cplusplus
}
#endif

#endif /* _ES_INTERFACES_C_ */
