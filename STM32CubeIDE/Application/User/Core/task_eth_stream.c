/**
 ******************************************************************************
 * @file    task_eth_stream.c
 * @brief   DVS event UDP streaming task over Ethernet.
 *
 *          Streams raw EVT2.0 words from the DCMI DMA circular buffer
 *          over UDP to a configurable host (default: 192.168.1.100:7777).
 *
 *          Wire format:
 *            - Raw little-endian uint32_t EVT2.0 words, back-to-back.
 *            - No framing header — same format as the USB FS stream.
 *            - Each UDP datagram carries ETH_UDP_PAYLOAD_BYTES of event data.
 *
 *          The UDP MTU for Ethernet is 1472 bytes (1500 - 20 IP - 8 UDP).
 *          Sending 1472-byte datagrams at 100 Mbps saturates the link at
 *          ~8 MB/s sustained, well above the sensor's peak event rate.
 *
 *          The task waits until LwIP has a valid link before opening the
 *          UDP PCB.  If the link drops mid-stream it yields and retries.
 *
 ******************************************************************************
 * @attention
 * Copyright (c) Prophesee S.A.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "task_eth_stream.h"

/* RTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* LwIP */
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip.h"
#include <string.h>

/* EVT2.0 buffer */
#include "task_decoder.h"

/* Private defines -----------------------------------------------------------*/
/* UDP destination: change to the IP of your receiving host */
#ifndef ETH_DEST_IP0
#define ETH_DEST_IP0   192U
#endif
#ifndef ETH_DEST_IP1
#define ETH_DEST_IP1   168U
#endif
#ifndef ETH_DEST_IP2
#define ETH_DEST_IP2   1U
#endif
#ifndef ETH_DEST_IP3
#define ETH_DEST_IP3   100U
#endif
#ifndef ETH_DEST_PORT
#define ETH_DEST_PORT  7777U
#endif

/* Max UDP payload for standard Ethernet MTU (1500 - 20 IP - 8 UDP) */
#define ETH_UDP_PAYLOAD_BYTES  1472U
#define ETH_UDP_PAYLOAD_WORDS  (ETH_UDP_PAYLOAD_BYTES / sizeof(uint32_t))

/* Task entry point ----------------------------------------------------------*/

/**
 * @brief  DVS event Ethernet UDP streaming task.
 *         Registered as "EthTx" in main.c.
 *
 *         Drains the DCMI DMA circular buffer and sends raw EVT2.0 words
 *         in 1472-byte UDP datagrams over Ethernet.
 *
 *         Maintains its own read pointer independent of the USB task so
 *         both transports stream simultaneously without interfering.
 */
void task_eth_stream(void *argument)
{
    (void)argument;

    uint32_t read_idx = 0;
    struct udp_pcb *pcb = NULL;
    ip4_addr_t dest_ip;
    IP4_ADDR(&dest_ip, ETH_DEST_IP0, ETH_DEST_IP1, ETH_DEST_IP2, ETH_DEST_IP3);

    /* Short delay to avoid competing with immediate post-boot init work. */
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Initialize LwIP here, isolated from all other tasks.
     * A failure in HAL_ETH_Init or PHY negotiation only affects this task. */
    MX_LWIP_Init();

    /* Wait for link to come up (cable may not be connected at boot) */
    for (;;)
    {
        if (netif_is_link_up(&gnetif))
            break;
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    /* Create UDP PCB */
    pcb = udp_new();
    if (pcb == NULL)
    {
        /* Out of memory — nothing we can do, park the task */
        for (;;) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    for (;;)
    {
        /* Re-check link — if it dropped, wait for it to recover */
        if (!netif_is_link_up(&gnetif))
        {
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        uint32_t write_idx = dcmi_dma_write_idx(NULL);

        /* Words available ahead of our read pointer */
        uint32_t available;
        if (write_idx >= read_idx) {
            available = write_idx - read_idx;
        } else {
            available = EVT_BUF_LEN - read_idx + write_idx;
        }

        /* Wait until a full datagram worth of data is ready */
        if (available < ETH_UDP_PAYLOAD_WORDS)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        /* Clamp to end of linear buffer (handle circular wraparound) */
        uint32_t words_to_send = ETH_UDP_PAYLOAD_WORDS;
        if (read_idx + words_to_send > EVT_BUF_LEN)
            words_to_send = EVT_BUF_LEN - read_idx;

        uint16_t byte_len = (uint16_t)(words_to_send * sizeof(uint32_t));

        /* Allocate a pbuf that references the event_buffer directly (zero-copy) */
        struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, byte_len, PBUF_RAM);
        if (p != NULL)
        {
            memcpy(p->payload, &event_buffer[read_idx], byte_len);
            udp_sendto(pcb, p, &dest_ip, ETH_DEST_PORT);
            pbuf_free(p);
        }

        read_idx += words_to_send;
        if (read_idx >= EVT_BUF_LEN)
            read_idx = 0;
    }
}
