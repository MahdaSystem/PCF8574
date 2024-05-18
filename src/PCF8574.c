/**
 **********************************************************************************
 * @file   PCF8574.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  PCF8574 and PCF8574A 8-bit I/O expander driver
 **********************************************************************************
 *
 * Copyright (c) 2024 Mahda Embedded System (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */

/* Includes ---------------------------------------------------------------------*/
#include "PCF8574.h"
#include <stdio.h>


/* Private Constants ------------------------------------------------------------*/
/**
 * @brief  I2C device addresses
 */
#define PCF8574_I2C_ADDRESS_BASE    0x20
#define PCF8574A_I2C_ADDRESS_BASE   0x38



/**
 ==================================================================================
                       ##### Private Functions #####
 ==================================================================================
 */
static PCF8574_Result_t
PCF8574_WriteReg(PCF8574_Handler_t *Handler, uint8_t Data)
{
  if (Handler->Platform.Send(Handler->AddressI2C, &Data, 1) < 0)
    return PCF8574_FAIL;

  return PCF8574_OK;
}

static PCF8574_Result_t
PCF8574_ReadReg(PCF8574_Handler_t *Handler, uint8_t *Data)
{
  if (Handler->Platform.Receive(Handler->AddressI2C, Data, 1) < 0)
    return PCF8574_FAIL;

  return PCF8574_OK;
}



/**
 ==================================================================================
                            ##### Public Functions #####
 ==================================================================================
 */

/**
 * @brief  Initializer function
 * @note   This function must be called after initializing platform dependent
 *         layer and before using other functions.
 * @param  Handler: Pointer to handler
 * @param  Device: Device type
 * @param  Address: Address pins state (0 <= Address <= 7)
 * @retval PCF8574_Result_t
 *         - PCF8574_OK: Operation was successful.
 *         - PCF8574_FAIL: Failed to send or receive data.
 *         - PCF8574_INVALID_PARAM: Invalid parameter.
 */
PCF8574_Result_t
PCF8574_Init(PCF8574_Handler_t *Handler, PCF8574_Device_t Device,
             uint8_t Address)
{
  if (!Handler)
    return PCF8574_INVALID_PARAM;

  if (Device != PCF8574_DEVICE_PCF8574 &&
      Device != PCF8574_DEVICE_PCF8574A)
    return PCF8574_INVALID_PARAM;
  Handler->Device = Device;

  if (PCF8574_SetAddressI2C(Handler, Address) != PCF8574_OK)
    return PCF8574_INVALID_PARAM;

  if (!Handler->Platform.Send || !Handler->Platform.Receive)
    return PCF8574_INVALID_PARAM;

  if (Handler->Platform.Init)
  {
    if (Handler->Platform.Init() < 0)
      return PCF8574_FAIL;
  }

  Handler->Direction = 0x00;
  if (PCF8574_WriteReg(Handler, ~(Handler->Direction)) != PCF8574_OK)
    return PCF8574_FAIL;

  return PCF8574_OK;
}


/**
 * @brief  Deinitialize function
 * @param  Handler: Pointer to handler
 * @retval PCF8574_Result_t
 *         - PCF8574_OK: Operation was successful.
 *         - PCF8574_FAIL: Failed to send or receive data.
 */
PCF8574_Result_t
PCF8574_DeInit(PCF8574_Handler_t *Handler)
{
  if (!Handler)
    return PCF8574_INVALID_PARAM;

  if (Handler->Platform.DeInit)
    return ((Handler->Platform.DeInit() >= 0) ? PCF8574_OK : PCF8574_FAIL);

  return PCF8574_OK;
}


/**
 * @brief  Set I2C Address
 * @param  Handler: Pointer to handler
 * @param  Address: Address pins state (0 <= Address <= 7)
 * @retval PCF8574_Result_t
 *         - PCF8574_OK: Operation was successful.
 *         - PCF8574_INVALID_PARAM: One of parameters is invalid.
 */
PCF8574_Result_t
PCF8574_SetAddressI2C(PCF8574_Handler_t *Handler, uint8_t Address)
{
  if (Address > 7)
    return PCF8574_INVALID_PARAM;

  switch (Handler->Device)
  {
  case PCF8574_DEVICE_PCF8574:
    Handler->AddressI2C = PCF8574_I2C_ADDRESS_BASE | Address;
    break;

  case PCF8574_DEVICE_PCF8574A:
    Handler->AddressI2C = PCF8574A_I2C_ADDRESS_BASE | Address;
    break;
  
  default:
    return PCF8574_INVALID_PARAM;
  }

  return PCF8574_OK;
}

/**
 * @brief  Set direction of pins
 * @param  Handler: Pointer to handler
 * @param  Dir: Direction of pins (1: Output, 0: Input)
 * @retval PCF8574_Result_t
 *         - PCF8574_OK: Operation was successful.
 *         - PCF8574_FAIL: Failed to send or receive data.
 */
PCF8574_Result_t
PCF8574_SetDir(PCF8574_Handler_t *Handler, uint8_t Dir)
{
  Handler->Direction = Dir;
  return PCF8574_OK;
}


/**
 * @brief  Read data from the device
 * @param  Handler: Pointer to handler
 * @param  Data: Pointer to data
 * @retval PCF8574_Result_t
 *         - PCF8574_OK: Operation was successful.
 *         - PCF8574_FAIL: Failed to send or receive data.
 */
PCF8574_Result_t
PCF8574_Read(PCF8574_Handler_t *Handler, uint8_t *Data)
{
  Handler->Output |= ~(Handler->Direction);
  if (PCF8574_WriteReg(Handler, Handler->Output) != PCF8574_OK)
    return PCF8574_FAIL;

  if (PCF8574_ReadReg(Handler, Data) != PCF8574_OK)
    return PCF8574_FAIL;

  return PCF8574_OK;
}


/**
 * @brief  Write data to the device
 * @param  Handler: Pointer to handler
 * @param  Data: Data to write
 * @retval PCF8574_Result_t
 *         - PCF8574_OK: Operation was successful.
 *         - PCF8574_FAIL: Failed to send or receive data.
 */
PCF8574_Result_t
PCF8574_Write(PCF8574_Handler_t *Handler, uint8_t Data)
{
  Handler->Output = Data | (~(Handler->Direction));
  if (PCF8574_WriteReg(Handler, Handler->Output) != PCF8574_OK)
    return PCF8574_FAIL;
  return PCF8574_OK;
}


/**
 * @brief  Write data to the one bit
 * @param  Handler: Pointer to handler
 * @param  Pos: Position of bit (0 <= Pos <= 7)
 * @param  Value: Value to write (1: High, 0: Low)
 * @retval PCF8574_Result_t
 *         - PCF8574_OK: Operation was successful.
 *         - PCF8574_FAIL: Failed to send or receive data.
 *         - PCF8574_INVALID_PARAM: One of parameters is invalid.
 */
PCF8574_Result_t
PCF8574_WriteOne(PCF8574_Handler_t *Handler, uint8_t Pos, uint8_t Value)
{
  if (Pos > 7)
    return PCF8574_INVALID_PARAM;

  if (Value)
    Handler->Output |= (1 << Pos);
  else
    Handler->Output &= ~(1 << Pos);

  if (PCF8574_WriteReg(Handler, Handler->Output) != PCF8574_OK)
    return PCF8574_FAIL;

  return PCF8574_OK;
}
