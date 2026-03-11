/**
 ******************************************************************************
 * @file    task_usb_enum.c
 * @brief   USB CDC DVS event streaming task.
 *
 *          Streams raw EVT2.0 words from the DCMI DMA circular buffer
 *          directly over the USB FS CDC (Virtual COM) port.
 *
 *          The USB FS bulk endpoint MPS is 64 bytes (16 × uint32_t).
 *          This task maintains its own read pointer into event_buffer[]
 *          and sends data in 64-byte chunks.  If the CDC TX is busy the
 *          task yields for 1 ms and retries, preventing data loss at the
 *          cost of backpressure.  If the USB host is not connected (or the
 *          DMA has not started yet) the task simply yields.
 *
 *          Wire format on the host:
 *            - Raw little-endian uint32_t EVT2.0 words, back-to-back.
 *            - No framing header — compatible with Prophesee SDK raw readers.
 *
 ******************************************************************************
 * @attention
 * Copyright (c) Prophesee S.A.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "task_usb_enum.h"

/* RTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USB CDC */
#include "usbd_cdc_if.h"

/* EVT2.0 buffer from the decoder task */
#include "task_decoder.h"

/* Private defines -----------------------------------------------------------*/
/* USB FS bulk MPS = 64 bytes = 16 uint32_t words */
#define USB_FS_MPS_BYTES   64U
#define USB_FS_MPS_WORDS   (USB_FS_MPS_BYTES / sizeof(uint32_t))

/* Task entry point ----------------------------------------------------------*/

/**
 * @brief  DVS event USB streaming task.
 *         Registered as "UsbTx" in main.c.
 */
void task_usb_tx(void *argument)
{
    (void)argument;

    uint32_t read_idx = 0;

    for (;;)
    {
        uint32_t write_idx = dcmi_dma_write_idx(NULL);

        /* Number of words available in the circular buffer */
        uint32_t available;
        if (write_idx >= read_idx) {
            available = write_idx - read_idx;
        } else {
            available = EVT_BUF_LEN - read_idx + write_idx;
        }

        /* Nothing ready — yield */
        if (available == 0) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        /* Send one MPS-sized chunk at a time to respect USB packet boundaries */
        uint32_t words_to_send = (available >= USB_FS_MPS_WORDS) ? USB_FS_MPS_WORDS : available;

        /* Check for wraparound: only send up to end of buffer in one shot */
        if (read_idx + words_to_send > EVT_BUF_LEN) {
            words_to_send = EVT_BUF_LEN - read_idx;
        }

        uint8_t *src = (uint8_t *)&event_buffer[read_idx];
        uint16_t byte_len = (uint16_t)(words_to_send * sizeof(uint32_t));

        /* Retry while CDC is busy — backpressure so we don't drop events */
        uint8_t result;
        do {
            result = CDC_Transmit_FS(src, byte_len);
            if (result == USBD_BUSY) {
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        } while (result == USBD_BUSY);

        /* Advance read pointer (only on success or FAIL — not on BUSY) */
        if (result == USBD_OK) {
            read_idx += words_to_send;
            if (read_idx >= EVT_BUF_LEN) {
                read_idx = 0;
            }
        }
    }
}
