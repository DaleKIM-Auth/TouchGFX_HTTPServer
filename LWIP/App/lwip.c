/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#if defined ( __CC_ARM )  /* MDK ARM Compiler */
#include "lwip/sio.h"
#endif /* MDK ARM Compiler */
#include "ethernetif.h"
#include <string.h>

/* USER CODE BEGIN 0 */
#include "lwip/sockets.h"
#include "lwip/apps/fs.h"
#include "FreeRTOS.h"
#include <stdio.h>

#define WEBSERVER_THREAD_PRIO    ( osPriorityAboveNormal )

u32_t nPageHits = 0;
/* Format of dynamic web page: the page header */
static const unsigned char PAGE_START[] = {
    0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x68,0x74,0x6d,0x6c,0x20,0x50,
    0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,0x2f,0x2f,0x44,
    0x54,0x44,0x20,0x48,0x54,0x4d,0x4c,0x20,0x34,0x2e,0x30,0x31,0x2f,0x2f,0x45,0x4e,
    0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
    0x2e,0x6f,0x72,0x67,0x2f,0x54,0x52,0x2f,0x68,0x74,0x6d,0x6c,0x34,0x2f,0x73,0x74,
    0x72,0x69,0x63,0x74,0x2e,0x64,0x74,0x64,0x22,0x3e,0x0a,0x3c,0x68,0x74,0x6d,0x6c,
    0x3e,0x3c,0x68,0x65,0x61,0x64,0x3e,0x3c,0x74,0x69,0x74,0x6c,0x65,0x3e,0x53,0x54,
    0x4d,0x33,0x32,0x48,0x37,0x78,0x78,0x54,0x41,0x53,0x4b,0x53,0x3c,0x2f,0x74,0x69,
    0x74,0x6c,0x65,0x3e,0x0d,0x0a,0x0d,0x0a,0x3c,0x6d,0x65,0x74,0x61,0x20,0x68,0x74,
    0x74,0x70,0x2d,0x65,0x71,0x75,0x69,0x76,0x3d,0x22,0x43,0x6f,0x6e,0x74,0x65,0x6e,
    0x74,0x2d,0x54,0x79,0x70,0x65,0x22,0x20,0x63,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x3d,
    0x22,0x74,0x65,0x78,0x74,0x2f,0x68,0x74,0x6d,0x6c,0x3b,0x20,0x63,0x68,0x61,0x72,
    0x73,0x65,0x74,0x3d,0x77,0x69,0x6e,0x64,0x6f,0x77,0x73,0x2d,0x31,0x32,0x35,0x32,
    0x22,0x3e,0x0d,0x0a,0x3c,0x6d,0x65,0x74,0x61,0x20,0x68,0x74,0x74,0x70,0x2d,0x65,
    0x71,0x75,0x69,0x76,0x3d,0x22,0x72,0x65,0x66,0x72,0x65,0x73,0x68,0x22,0x20,0x63,
    0x6f,0x6e,0x74,0x65,0x6e,0x74,0x3d,0x22,0x31,0x22,0x3e,0x0d,0x0a,0x3c,0x6d,0x65,
    0x74,0x61,0x20,0x63,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x3d,0x22,0x4d,0x53,0x48,0x54,
    0x4d,0x4c,0x20,0x36,0x2e,0x30,0x30,0x2e,0x32,0x38,0x30,0x30,0x2e,0x31,0x35,0x36,
    0x31,0x22,0x20,0x6e,0x61,0x6d,0x65,0x3d,0x22,0x47,0x45,0x4e,0x45,0x52,0x41,0x54,
    0x4f,0x52,0x22,0x3e,0x0d,0x0a,0x3c,0x73,0x74,0x79,0x6c,0x65,0x20,0x3d,0x22,0x66,
    0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x6e,0x6f,0x72,0x6d,
    0x61,0x6c,0x3b,0x20,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,0x79,0x3a,
    0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x22,0x3e,0x3c,0x2f,0x73,0x74,0x79,
    0x6c,0x65,0x3e,0x0d,0x0a,0x3c,0x2f,0x68,0x65,0x61,0x64,0x3e,0x0d,0x0a,0x0d,0x0a,
    0x3c,0x62,0x6f,0x64,0x79,0x3e,0x0d,0x0a,0x3c,0x68,0x34,0x20,0x73,0x74,0x79,0x6c,
    0x65,0x3d,0x22,0x63,0x6f,0x6c,0x6f,0x72,0x3a,0x20,0x72,0x67,0x62,0x28,0x35,0x31,
    0x2c,0x20,0x35,0x31,0x2c,0x20,0x32,0x35,0x35,0x29,0x3b,0x22,0x3e,0x3c,0x73,0x6d,
    0x61,0x6c,0x6c,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,
    0x66,0x61,0x6d,0x69,0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,
    0x22,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x62,0x69,0x67,0x3e,0x3c,0x62,
    0x69,0x67,0x3e,0x3c,0x62,0x69,0x67,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,
    0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,
    0x3b,0x22,0x3e,0x3c,0x62,0x69,0x67,0x3e,0x3c,0x73,0x74,0x72,0x6f,0x6e,0x67,0x3e,
    0x3c,0x65,0x6d,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,
    0x22,0x66,0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,0x74,0x61,
    0x6c,0x69,0x63,0x3b,0x22,0x3e,0x53,0x54,0x4d,0x33,0x32,0x48,0x37,0x78,0x78,0x20,
    0x4c,0x69,0x73,0x74,0x20,0x6f,0x66,0x20,0x74,0x61,0x73,0x6b,0x73,0x20,0x61,0x6e,
    0x64,0x0d,0x0a,0x74,0x68,0x65,0x69,0x72,0x20,0x73,0x74,0x61,0x74,0x75,0x73,0x3c,
    0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,0x2f,0x65,0x6d,0x3e,0x3c,0x2f,0x73,0x74,0x72,
    0x6f,0x6e,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,
    0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x73,0x6d,
    0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x68,0x34,
    0x3e,0x0d,0x0a,0x3c,0x68,0x72,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x77,0x69,
    0x64,0x74,0x68,0x3a,0x20,0x31,0x30,0x30,0x25,0x3b,0x20,0x68,0x65,0x69,0x67,0x68,
    0x74,0x3a,0x20,0x32,0x70,0x78,0x3b,0x22,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,
    0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,
    0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x0d,0x0a,0x3c,0x2f,0x73,0x70,
    0x61,0x6e,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,
    0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,
    0x64,0x3b,0x22,0x3e,0x0d,0x0a,0x3c,0x74,0x61,0x62,0x6c,0x65,0x20,0x73,0x74,0x79,
    0x6c,0x65,0x3d,0x22,0x77,0x69,0x64,0x74,0x68,0x3a,0x20,0x39,0x36,0x31,0x70,0x78,
    0x3b,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x33,0x30,0x70,0x78,0x3b,0x22,
    0x20,0x62,0x6f,0x72,0x64,0x65,0x72,0x3d,0x22,0x31,0x22,0x20,0x63,0x65,0x6c,0x6c,
    0x70,0x61,0x64,0x64,0x69,0x6e,0x67,0x3d,0x22,0x32,0x22,0x20,0x63,0x65,0x6c,0x6c,
    0x73,0x70,0x61,0x63,0x69,0x6e,0x67,0x3d,0x22,0x32,0x22,0x3e,0x0d,0x0a,0x3c,0x74,
    0x62,0x6f,0x64,0x79,0x3e,0x0d,0x0a,0x3c,0x74,0x72,0x3e,0x0d,0x0a,0x3c,0x74,0x64,
    0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,
    0x69,0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x20,0x66,0x6f,
    0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,
    0x20,0x66,0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,0x74,0x61,
    0x6c,0x69,0x63,0x3b,0x20,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,0x64,0x2d,
    0x63,0x6f,0x6c,0x6f,0x72,0x3a,0x20,0x72,0x67,0x62,0x28,0x35,0x31,0x2c,0x20,0x35,
    0x31,0x2c,0x20,0x32,0x35,0x35,0x29,0x3b,0x20,0x74,0x65,0x78,0x74,0x2d,0x61,0x6c,
    0x69,0x67,0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,0x22,0x3e,0x3c,0x73,
    0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x61,0x20,0x68,0x72,0x65,0x66,0x3d,0x22,0x2f,0x53,
    0x54,0x4d,0x33,0x32,0x48,0x37,0x78,0x78,0x2e,0x68,0x74,0x6d,0x6c,0x22,0x3e,0x3c,
    0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x63,0x6f,0x6c,0x6f,
    0x72,0x3a,0x20,0x77,0x68,0x69,0x74,0x65,0x3b,0x22,0x3e,0x48,0x6f,0x6d,0x65,0x0d,
    0x0a,0x70,0x61,0x67,0x65,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,0x2f,0x61,0x3e,
    0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x74,0x64,0x3e,0x0d,0x0a,0x3c,
    0x74,0x64,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,
    0x61,0x6d,0x69,0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x20,
    0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,
    0x64,0x3b,0x20,0x66,0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,
    0x74,0x61,0x6c,0x69,0x63,0x3b,0x20,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
    0x64,0x2d,0x63,0x6f,0x6c,0x6f,0x72,0x3a,0x20,0x72,0x67,0x62,0x28,0x35,0x31,0x2c,
    0x20,0x35,0x31,0x2c,0x20,0x32,0x35,0x35,0x29,0x3b,0x20,0x74,0x65,0x78,0x74,0x2d,
    0x61,0x6c,0x69,0x67,0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,0x22,0x3e,
    0x3c,0x61,0x20,0x68,0x72,0x65,0x66,0x3d,0x22,0x53,0x54,0x4d,0x33,0x32,0x48,0x37,
    0x78,0x78,0x41,0x44,0x43,0x2e,0x68,0x74,0x6d,0x6c,0x22,0x3e,0x3c,0x73,0x70,0x61,
    0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,
    0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x3c,0x2f,0x73,
    0x70,0x61,0x6e,0x3e,0x3c,0x2f,0x61,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,
    0x61,0x20,0x68,0x72,0x65,0x66,0x3d,0x22,0x2f,0x53,0x54,0x4d,0x33,0x32,0x48,0x37,
    0x78,0x78,0x54,0x41,0x53,0x4b,0x53,0x2e,0x68,0x74,0x6d,0x6c,0x22,0x3e,0x3c,0x73,
    0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x63,0x6f,0x6c,0x6f,0x72,
    0x3a,0x20,0x77,0x68,0x69,0x74,0x65,0x3b,0x22,0x3e,0x4c,0x69,0x73,0x74,0x0d,0x0a,
    0x6f,0x66,0x20,0x74,0x61,0x73,0x6b,0x73,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,
    0x2f,0x61,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x74,0x64,0x3e,
    0x0d,0x0a,0x3c,0x2f,0x74,0x72,0x3e,0x0d,0x0a,0x3c,0x2f,0x74,0x62,0x6f,0x64,0x79,
    0x3e,0x0d,0x0a,0x3c,0x2f,0x74,0x61,0x62,0x6c,0x65,0x3e,0x0d,0x0a,0x3c,0x62,0x72,
    0x3e,0x0d,0x0a,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,
    0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,
    0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x3c,0x2f,0x73,0x70,0x61,
    0x6e,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,
    0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,
    0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x22,0x3e,0x4e,0x75,0x6d,
    0x62,0x65,0x72,0x20,0x6f,0x66,0x20,0x70,0x61,0x67,0x65,0x20,0x68,0x69,0x74,0x73,
    0x3a,0x26,0x6e,0x62,0x73,0x70,0x3b,0x0d,0x0a,0x0d,0x0a,0x3c,0x2f,0x73,0x70,0x61,
    0x6e,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x0d,0x0a,0x3c,0x2f,0x62,0x6f,
    0x64,0x79,0x3e,0x3c,0x2f,0x68,0x74,0x6d,0x6c,0x3e,0
};

void http_server_serve(int conn);
/* USER CODE END 0 */
/* Private function prototypes -----------------------------------------------*/
static void ethernet_link_status_updated(struct netif *netif);
/* ETH Variables initialization ----------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Variables Initialization */
struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;
uint8_t IP_ADDRESS[4];
uint8_t NETMASK_ADDRESS[4];
uint8_t GATEWAY_ADDRESS[4];
/* USER CODE BEGIN OS_THREAD_ATTR_CMSIS_RTOS_V2 */
#define INTERFACE_THREAD_STACK_SIZE ( 1024 )
osThreadAttr_t attributes;
/* USER CODE END OS_THREAD_ATTR_CMSIS_RTOS_V2 */

/* USER CODE BEGIN 2 */
/**
  * @brief  Create and send a dynamic Web Page. This page contains the list of
  *         running tasks and the number of page hits.
  * @param  conn connection socket
  * @retval None
  */
void DynWebPage(int conn)
{
  portCHAR PAGE_BODY[512];
  portCHAR pagehits[10] = {0};

  memset(PAGE_BODY, 0,512);

    /* Update the hit count */
  nPageHits++;
  sprintf( pagehits, "%d", (int)nPageHits );
  strcat(PAGE_BODY, pagehits);
  /* Send the dynamically generated page */
  write(conn, PAGE_START, strlen((char*)PAGE_START));
  write(conn, PAGE_BODY, strlen(PAGE_BODY));
}

/**
  * @brief serve tcp connection
  * @param conn: connection socket
  * @retval None
  */
void http_server_serve(int conn)
{
  int buflen = 1500;
  int ret;
  //struct fs_file file;
  unsigned char recv_buffer[1500];

  /* Read in the request */
  ret = read(conn, recv_buffer, buflen);
  if(ret < 0) return;

  if(strncmp((char *)recv_buffer, "GET /Application.html", 19) == 0)
  {
    /* Load dynamic page */
    DynWebPage(conn);
  }

  /* Close connection socket */
  shutdown(conn, SHUT_RDWR);
  close(conn);
}

/**
  * @brief  http server thread
  * @param arg: pointer on argument(not used here)
  * @retval None
  */
static void http_server_socket_thread(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;

 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return;
  }

  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(80);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
    return;
  }

  /* listen for incoming connections (TCP listen backlog = 5) */
  listen(sock, 5);

  size = sizeof(remotehost);

  while (1)
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
    http_server_serve(newconn);
  }
}

void http_server_socket_init()
{
  sys_thread_new("HTTP", http_server_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE * 4, WEBSERVER_THREAD_PRIO);
}
/* USER CODE END 2 */

/**
  * LwIP initialization function
  */
void MX_LWIP_Init(void)
{
  /* IP addresses initialization */
  IP_ADDRESS[0] = 192;
  IP_ADDRESS[1] = 168;
  IP_ADDRESS[2] = 0;
  IP_ADDRESS[3] = 10;
  NETMASK_ADDRESS[0] = 255;
  NETMASK_ADDRESS[1] = 255;
  NETMASK_ADDRESS[2] = 255;
  NETMASK_ADDRESS[3] = 0;
  GATEWAY_ADDRESS[0] = 192;
  GATEWAY_ADDRESS[1] = 168;
  GATEWAY_ADDRESS[2] = 0;
  GATEWAY_ADDRESS[3] = 1;

/* USER CODE BEGIN IP_ADDRESSES */
/* USER CODE END IP_ADDRESSES */

  /* Initialize the LwIP stack with RTOS */
  tcpip_init( NULL, NULL );

  /* IP addresses initialization without DHCP (IPv4) */
  IP4_ADDR(&ipaddr, IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]);
  IP4_ADDR(&netmask, NETMASK_ADDRESS[0], NETMASK_ADDRESS[1] , NETMASK_ADDRESS[2], NETMASK_ADDRESS[3]);
  IP4_ADDR(&gw, GATEWAY_ADDRESS[0], GATEWAY_ADDRESS[1], GATEWAY_ADDRESS[2], GATEWAY_ADDRESS[3]);

  /* add the network interface (IPv4/IPv6) with RTOS */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /* Registers the default network interface */
  netif_set_default(&gnetif);

  /* We must always bring the network interface up connection or not... */
  netif_set_up(&gnetif);

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);

  /* Create the Ethernet link handler thread */
/* USER CODE BEGIN H7_OS_THREAD_NEW_CMSIS_RTOS_V2 */
  memset(&attributes, 0x0, sizeof(osThreadAttr_t));
  attributes.name = "EthLink";
  attributes.stack_size = INTERFACE_THREAD_STACK_SIZE;
  attributes.priority = osPriorityNormal;
  osThreadNew(ethernet_link_thread, &gnetif, &attributes);

/* USER CODE END H7_OS_THREAD_NEW_CMSIS_RTOS_V2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */
}

#ifdef USE_OBSOLETE_USER_CODE_SECTION_4
/* Kept to help code migration. (See new 4_1, 4_2... sections) */
/* Avoid to use this user section which will become obsolete. */
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
#endif

/**
  * @brief  Notify the User about the network interface config status
  * @param  netif: the network interface
  * @retval None
  */
static void ethernet_link_status_updated(struct netif *netif)
{
  if (netif_is_up(netif))
  {
/* USER CODE BEGIN 5 */
/* USER CODE END 5 */
  }
  else /* netif is down */
  {
/* USER CODE BEGIN 6 */
/* USER CODE END 6 */
  }
}

#if defined ( __CC_ARM )  /* MDK ARM Compiler */
/**
 * Opens a serial device for communication.
 *
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
  sio_fd_t sd;

/* USER CODE BEGIN 7 */
  sd = 0; // dummy code
/* USER CODE END 7 */

  return sd;
}

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
/* USER CODE BEGIN 8 */
/* USER CODE END 8 */
}

/**
 * Reads from the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 *
 * @note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
  u32_t recved_bytes;

/* USER CODE BEGIN 9 */
  recved_bytes = 0; // dummy code
/* USER CODE END 9 */
  return recved_bytes;
}

/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
  u32_t recved_bytes;

/* USER CODE BEGIN 10 */
  recved_bytes = 0; // dummy code
/* USER CODE END 10 */
  return recved_bytes;
}
#endif /* MDK ARM Compiler */

