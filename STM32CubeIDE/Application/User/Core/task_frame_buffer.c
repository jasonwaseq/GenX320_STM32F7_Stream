/**
 ******************************************************************************
 * @file    task_frame_buffer.c
 * @author  PSEE Applications Team
 * @brief	Frame buffer specific Functions
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
#include "task_frame_buffer.h"

/* Private Defines */
#define DISPLAY_RESOLUTION (320*272)

/* 3 Display Frame Buffers */
static uint8_t frame_buffer_l1_one[DISPLAY_RESOLUTION] __attribute__((section("L1_Frame_Buffers"))) __attribute__((aligned(4)));
static uint8_t frame_buffer_l1_two[DISPLAY_RESOLUTION] __attribute__((section("L1_Frame_Buffers"))) __attribute__((aligned(4)));
static uint8_t frame_buffer_l1_three[DISPLAY_RESOLUTION] __attribute__((section("L1_Frame_Buffers"))) __attribute__((aligned(4)));

uint8_t* volatile fb_cpi = frame_buffer_l1_three;

/* This semaphore is used to notify the application that the new
 * frame buffer is displayed, a new one can be prepared...
 */
static SemaphoreHandle_t xSemphFbUpdtHdl = NULL;

/**
 * @brief Color look-up table for Events streaming mode
 */
static const uint32_t clut_event[] = {
		((30UL << 16) | (37UL << 8) | (52UL)), 		// Background
		((216UL << 16) | (223UL << 8) | (236UL)), 	// ON event
		((64UL << 16) | (126UL << 8) | (201UL)) 	// OFF event
};

/**
 * @brief Function to enable the LCD layer
 * @param hltdc Pointer to a LTDC_HandleTypeDef structure
 * @param layer LTDC Layer index
 * @param address Pointer to the address of the frame buffer
 */
static void enable_layer(LTDC_HandleTypeDef *hltdc, unsigned int layer, void *address) {

	LTDC_LayerCfgTypeDef pLayerCfg = {0};

	pLayerCfg.WindowX0 = 160;
	pLayerCfg.WindowX1 = 480;
	pLayerCfg.WindowY0 = 0;
	pLayerCfg.WindowY1 = 272;
	pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_L8; //LTDC_PIXEL_FORMAT_RGB565;
	pLayerCfg.Alpha = 255;
	pLayerCfg.Alpha0 = 0;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	pLayerCfg.FBStartAdress = (uint32_t)address;
	pLayerCfg.ImageWidth = 320;
	pLayerCfg.ImageHeight = 272;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0;
	pLayerCfg.Backcolor.Red = 0;
	HAL_LTDC_ConfigLayer(hltdc, &pLayerCfg, layer);
	HAL_LTDC_ConfigCLUT(hltdc, (uint32_t *)clut_event, sizeof(clut_event)/sizeof(uint32_t), layer);
	HAL_LTDC_EnableCLUT(hltdc, layer);
}

/**
 * @brief Function to Reset the frame buffer. It must be done before scheduling starts.
 * It can not be done by DMA.
 */
void reset_frame_buffers() {

	memset(frame_buffer_l1_one, 0 , sizeof(frame_buffer_l1_one));
	memset(frame_buffer_l1_two, 0 , sizeof(frame_buffer_l1_two));
	memset(frame_buffer_l1_three, 0 , sizeof(frame_buffer_l1_three));

}

/**
 * @brief Function implementing the Update frame buffer task.
 *
 * Triple-buffer scheme:
 *   fb_cpi     – decoder writes events into this buffer
 *   fb_display – currently scanned out by LTDC
 *   fb_reset   – being cleared for the next write cycle
 *
 * The fb_cpi pointer swap is guarded by a critical section so the
 * decoder always sees a consistent target.  The recycled buffer is
 * cleared with CPU memset (synchronous) instead of async DMA to
 * eliminate the race between clearing and the next decode write.
 * The LTDC reload semaphore uses a timeout instead of assert so a
 * missed vsync never crashes the display pipeline.
 */
extern SemaphoreHandle_t tskCtlUpdateFbHandle;
void task_update_fb(const args_update_fb_t *args) {

	uint8_t* fb_display = frame_buffer_l1_one;
	uint8_t* fb_reset   = frame_buffer_l1_two;
	uint8_t* old_fb_cpi;
	BaseType_t status;
	StaticSemaphore_t xSemaphoreBuffer;

	xSemphFbUpdtHdl = xSemaphoreCreateBinaryStatic( &xSemaphoreBuffer );
	assert(xSemphFbUpdtHdl);

	enable_layer(args->hltdc, 1, fb_display);

	/* Infinite loop */
	for(;;)
	{
		/* Suspend until app_state signals this task should run */
		status = xSemaphoreTake(tskCtlUpdateFbHandle, portMAX_DELAY);
		assert(status == pdTRUE);

		/* Atomically swap decode-target pointer — critical section is
		 * pointer-width so it completes in a handful of cycles. */
		taskENTER_CRITICAL();
		old_fb_cpi = fb_cpi;
		fb_cpi     = fb_reset;
		taskEXIT_CRITICAL();

		fb_reset   = fb_display;
		fb_display = old_fb_cpi;

		/* Request LTDC shadow reload on next vertical blank */
		HAL_LTDC_Reload(args->hltdc, LTDC_RELOAD_VERTICAL_BLANKING);

		/* Update LTDC layer 2 frame buffer address */
		LTDC_LAYER(args->hltdc, 1)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
		LTDC_LAYER(args->hltdc, 1)->CFBAR  =  (uint32_t)(fb_display);

		/* Wait for reload ISR — timeout instead of assert so a late
		 * vsync (e.g. during heavy TouchGFX activity) is skipped
		 * gracefully rather than crashing the whole pipeline. */
		xSemaphoreTake(xSemphFbUpdtHdl, pdMS_TO_TICKS(100));

		status = xSemaphoreGive(tskCtlUpdateFbHandle);
		assert(status == pdTRUE);

		/* Synchronous CPU clear — guarantees the buffer is fully zeroed
		 * before the decoder starts writing into it next cycle. */
		memset(fb_reset, 0, DISPLAY_RESOLUTION);

		vTaskDelay(pdMS_TO_TICKS(12));
	}

}

/**
 * @brief Call back function for register reload
 */
void update_fb_callback()
{
	BaseType_t xHigherPriorityTaskWoken_RR = pdFALSE;
	xSemaphoreGiveFromISR( xSemphFbUpdtHdl, &xHigherPriorityTaskWoken_RR);

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken_RR );
}


