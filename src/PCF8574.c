/**
 **********************************************************************************
 * @file   PCF8574.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  PCF8574 8-bit I/O expander driver
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
 * @retval PCF8574_Result_t
 *         - PCF8574_OK: Operation was successful.
 *         - PCF8574_FAIL: Failed to send or receive data.
 *         - PCF8574_INVALID_PARAM: Invalid parameter.
 */
PCF8574_Result_t
PCF8574_Init(PCF8574_Handler_t *Handler)
{
  if (!Handler)
    return PCF8574_INVALID_PARAM;

  if (!Handler->Platform.Send || !Handler->Platform.Receive)
    return PCF8574_INVALID_PARAM;

  if (Handler->Platform.Init)
  {
    if (Handler->Platform.Init() < 0)
      return PCF8574_FAIL;
  }

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

