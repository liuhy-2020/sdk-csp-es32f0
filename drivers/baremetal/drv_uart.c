/*
 * Copyright (C) 2018 Shanghai Eastsoft Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-04-20     liuhy         the first version
 */

#include "string.h"
#include "board.h"
#include "drv_uart.h"
#include "es_conf_info_map.h"
#include <ald_uart.h>
#include <ald_usart.h>
#include <ald_gpio.h>
#include <ald_cmu.h>
/* es32 uart driver */
struct es32_uart
{
    uart_handle_t huart;
};                

/* es32 usart driver */
struct es32_usart
{
    usart_handle_t huart;
};

struct es32_uart *console_uart = 0;

#ifdef BSP_USING_UART0
/* UART0 device driver structure */
struct es32_uart uart0 =
{
    {UART0}
};
#endif /* BSP_USING_UART0 */

#ifdef BSP_USING_UART1
/* UART1 device driver structure */
struct es32_uart uart1 =
{
    {UART1}
};

#endif /* BSP_USING_UART1 */

#ifdef BSP_USING_UART2
/* UART2 device driver structure */
struct es32_uart uart2 =
{
    {UART2}
};
#endif /* BSP_USING_UART2 */

#ifdef BSP_USING_UART3
/* UART3 device driver structure */
struct es32_uart uart3 =
{
    {UART3}
};
#endif /* BSP_USING_UART3 */

#ifdef BSP_USING_USART0
/* USART0 device driver structure */
struct es32_usart usart0 =
{
    {USART0}
};

#endif /* BSP_USING_USART0 */

#ifdef BSP_USING_USART1
/* USART1 device driver structure */
struct es32_usart usart1 =
{
    {USART1}
};

#endif /* BSP_USING_USART1 */

static uint32_t es32f0x_configure(struct es32_uart *uart)
{
    gpio_init_t gpio_initstructure;

    /* Initialize tx pin */
    gpio_initstructure.mode = GPIO_MODE_OUTPUT;
    gpio_initstructure.odos = GPIO_PUSH_PULL;
    gpio_initstructure.pupd = GPIO_PUSH_UP;
    gpio_initstructure.odrv = GPIO_OUT_DRIVE_NORMAL;
    gpio_initstructure.flt  = GPIO_FILTER_DISABLE;
    gpio_initstructure.type = GPIO_TYPE_TTL;

    if((uint32_t)(uart->huart.perh) > (uint32_t)UART3)  /*根据外设物理地址区分UART和USART*/
    {
       /*USART*/
        struct es32_usart *usart= (struct es32_usart *)uart;
    
#ifdef BSP_USING_USART0
        if(usart == (&usart0))
        {
#if  defined(ES_USART0_TX_GPIO_FUNC)&&defined(ES_USART0_TX_GPIO_PORT)&&defined(ES_USART0_TX_GPIO_PIN)
            gpio_initstructure.func = ES_USART0_TX_GPIO_FUNC;
            ald_gpio_init(ES_USART0_TX_GPIO_PORT, ES_USART0_TX_GPIO_PIN, &gpio_initstructure);
#endif

#if  defined(ES_USART0_RX_GPIO_FUNC)&&defined(ES_USART0_RX_GPIO_PORT)&&defined(ES_USART0_RX_GPIO_PIN)        
            /* Initialize rx pin ,the same as txpin except mode */
            gpio_initstructure.mode = GPIO_MODE_INPUT;
            gpio_initstructure.func = ES_USART0_RX_GPIO_FUNC;
            ald_gpio_init(ES_USART0_RX_GPIO_PORT, ES_USART0_RX_GPIO_PIN, &gpio_initstructure);
#endif
            ald_cmu_perh_clock_config(CMU_PERH_USART0, ENABLE);
        }
    
#endif /* usart0 gpio init */
                                 
#ifdef BSP_USING_USART1
        if(usart == (&usart1))
        {
#if  defined(ES_USART1_TX_GPIO_FUNC)&&defined(ES_USART1_TX_GPIO_PORT)&&defined(ES_USART1_TX_GPIO_PIN)  
            gpio_initstructure.func = ES_USART1_TX_GPIO_FUNC;
            ald_gpio_init(ES_USART1_TX_GPIO_PORT, ES_USART1_TX_GPIO_PIN, &gpio_initstructure);
#endif

#if  defined(ES_USART1_RX_GPIO_FUNC)&&defined(ES_USART1_RX_GPIO_PORT)&&defined(ES_USART1_RX_GPIO_PIN) 
            /* Initialize rx pin ,the same as txpin except mode */
            gpio_initstructure.mode = GPIO_MODE_INPUT;
            gpio_initstructure.func = ES_USART1_RX_GPIO_FUNC;
            ald_gpio_init(ES_USART1_RX_GPIO_PORT, ES_USART1_RX_GPIO_PIN, &gpio_initstructure);
#endif

            ald_cmu_perh_clock_config(CMU_PERH_USART1, ENABLE);
        }
#endif /* usart1 gpio init */
        
        usart->huart.init.mode        = USART_MODE_TX_RX;
        usart->huart.init.baud        = 115200;  
        usart->huart.init.word_length = (usart_word_length_t)(0);   
        usart->huart.init.stop_bits   = USART_STOP_BITS_1;
        usart->huart.init.parity      = (usart_parity_t)(0);
        usart->huart.init.fctl        = USART_HW_FLOW_CTL_NONE; 

        ald_usart_init(&usart->huart);
                     
        
    }
    else
    {
       /*UART*/
       
#ifdef BSP_USING_UART0
        if(uart == (&uart0))
        {
#if  defined(ES_UART0_TX_GPIO_FUNC)&&defined(ES_UART0_TX_GPIO_PORT)&&defined(ES_UART0_TX_GPIO_PIN)
            gpio_initstructure.func = ES_UART0_TX_GPIO_FUNC;
            ald_gpio_init(ES_UART0_TX_GPIO_PORT, ES_UART0_TX_GPIO_PIN, &gpio_initstructure);
#endif

#if  defined(ES_UART0_RX_GPIO_FUNC)&&defined(ES_UART0_RX_GPIO_PORT)&&defined(ES_UART0_RX_GPIO_PIN)        
            /* Initialize rx pin ,the same as txpin except mode */
            gpio_initstructure.mode = GPIO_MODE_INPUT;
            gpio_initstructure.func = ES_UART0_RX_GPIO_FUNC;
            ald_gpio_init(ES_UART0_RX_GPIO_PORT, ES_UART0_RX_GPIO_PIN, &gpio_initstructure);
#endif
            ald_cmu_perh_clock_config(CMU_PERH_UART0, ENABLE);
        }
    
#endif /* uart0 gpio init */
                                 
#ifdef BSP_USING_UART1
        if(uart == (&uart1))
        {
#if  defined(ES_UART1_TX_GPIO_FUNC)&&defined(ES_UART1_TX_GPIO_PORT)&&defined(ES_UART1_TX_GPIO_PIN)  
            gpio_initstructure.func = ES_UART1_TX_GPIO_FUNC;
            ald_gpio_init(ES_UART1_TX_GPIO_PORT, ES_UART1_TX_GPIO_PIN, &gpio_initstructure);
#endif

#if  defined(ES_UART1_RX_GPIO_FUNC)&&defined(ES_UART1_RX_GPIO_PORT)&&defined(ES_UART1_RX_GPIO_PIN) 
            /* Initialize rx pin ,the same as txpin except mode */
            gpio_initstructure.mode = GPIO_MODE_INPUT;
            gpio_initstructure.func = ES_UART1_RX_GPIO_FUNC;
            ald_gpio_init(ES_UART1_RX_GPIO_PORT, ES_UART1_RX_GPIO_PIN, &gpio_initstructure);
#endif

            ald_cmu_perh_clock_config(CMU_PERH_UART1, ENABLE);
        }
#endif /* uart1 gpio init */
                               
#ifdef BSP_USING_UART2
        if(uart == (&uart2))
        {
#if  defined(ES_UART2_TX_GPIO_FUNC)&&defined(ES_UART2_TX_GPIO_PORT)&&defined(ES_UART2_TX_GPIO_PIN)
            gpio_initstructure.func = ES_UART2_TX_GPIO_FUNC;
            ald_gpio_init(ES_UART2_TX_GPIO_PORT, ES_UART2_TX_GPIO_PIN, &gpio_initstructure);
#endif

#if  defined(ES_UART2_RX_GPIO_FUNC)&&defined(ES_UART2_RX_GPIO_PORT)&&defined(ES_UART2_RX_GPIO_PIN)
            /* Initialize rx pin ,the same as txpin except mode */
            gpio_initstructure.mode = GPIO_MODE_INPUT;
            gpio_initstructure.func = ES_UART2_RX_GPIO_FUNC;
            ald_gpio_init(ES_UART2_RX_GPIO_PORT, ES_UART2_RX_GPIO_PIN, &gpio_initstructure);
#endif

            ald_cmu_perh_clock_config(CMU_PERH_UART2, ENABLE);
        }
#endif /* uart2 gpio init */
                                 
#ifdef BSP_USING_UART3
        if(uart == (&uart3))
        {
#if  defined(ES_UART3_TX_GPIO_FUNC)&&defined(ES_UART3_TX_GPIO_PORT)&&defined(ES_UART3_TX_GPIO_PIN)   
            gpio_initstructure.func = ES_UART3_TX_GPIO_FUNC;
            ald_gpio_init(ES_UART3_TX_GPIO_PORT, ES_UART3_TX_GPIO_PIN, &gpio_initstructure);
#endif

#if  defined(ES_UART3_RX_GPIO_FUNC)&&defined(ES_UART3_RX_GPIO_PORT)&&defined(ES_UART3_RX_GPIO_PIN)
            /* Initialize rx pin ,the same as txpin except mode */
            gpio_initstructure.mode = GPIO_MODE_INPUT;
            gpio_initstructure.func = ES_UART3_RX_GPIO_FUNC;
            ald_gpio_init(ES_UART3_RX_GPIO_PORT, ES_UART3_RX_GPIO_PIN, &gpio_initstructure);
#endif

            ald_cmu_perh_clock_config(CMU_PERH_UART3, ENABLE);   
        }
#endif /* uart3 gpio init */

        uart->huart.init.mode        = UART_MODE_UART;
        uart->huart.init.baud        = 115200;  
        uart->huart.init.word_length = UART_WORD_LENGTH_8B; 
        uart->huart.init.stop_bits   = UART_STOP_BITS_1;
        uart->huart.init.parity      = UART_PARITY_NONE;
        uart->huart.init.fctl        = UART_HW_FLOW_CTL_DISABLE; 

        ald_uart_init(&(uart->huart));

    }
    

    return 0;
}



int rt_hw_uart_init(void)
{
#ifdef BSP_USING_UART0
	es32f0x_configure(&uart0);
    if(!console_uart)console_uart = &uart0;
#endif /* BSP_USING_UART0 */

#ifdef BSP_USING_UART1
	es32f0x_configure(&uart1);
    if(!console_uart)console_uart = &uart2;
#endif /* BSP_USING_UART1 */

#ifdef BSP_USING_UART2
	es32f0x_configure(&uart2);
    if(!console_uart)console_uart = &uart2;
#endif /* BSP_USING_UART2 */

#ifdef BSP_USING_UART3
	es32f0x_configure(&uart3);
    if(!console_uart)console_uart = &uart3;
#endif /* BSP_USING_UART3 */
   
#ifdef BSP_USING_USART0
	es32f0x_configure(&usart0);
    if(!console_uart)console_uart = &usart0;
#endif /* BSP_USING_USART0 */

#ifdef BSP_USING_USART1
	es32f0x_configure(&usart1);
    if(!console_uart)console_uart = &usart1;
#endif /* BSP_USING_USART1 */
    return 0;
}


static int es32f0x_putc(struct es32_uart *uart, char c)
{
    
    if((uint32_t)(uart->huart.perh) > (uint32_t)UART3) /*根据外设物理地址区分UART和USART*/
    {  
       /*USART*/
        struct es32_usart *usart= (struct es32_usart *)uart;    
        while (!(usart->huart.perh->STAT & USART_STAT_TXEMPIF_MSK)) ;
        WRITE_REG(usart->huart.perh->DATA, c);
    }
    else 
    {    
       /*UART*/   
        while (!(uart->huart.perh->SR & 0x40)) ;
        WRITE_REG(uart->huart.perh->TBR, c);
    }

    return 1;
}

void print_char(char c)
{
    es32f0x_putc(console_uart,c);
}

#ifdef RT_USING_FINSH


static int es32f0x_getc(struct es32_uart *uart)
{
    int ch = -1;
                               
    if((uint32_t)(uart->huart.perh) > (uint32_t)UART3)  /*根据外设物理地址区分UART和USART*/
    {  
       /*USART*/
        struct es32_usart *usart= (struct es32_usart *)uart;   
        if (usart->huart.perh->STAT & USART_STAT_RXNEIF_MSK)
        {
            ch = (uint8_t)(usart->huart.perh->DATA & 0xFF);
        }
    }
    else 
    {    
       /*UART*/   
        if (uart->huart.perh->SR & 0x01)
        {
            ch = (uint8_t)(uart->huart.perh->RBR & 0xFF);
        }
    }

    return ch;
}


char rt_hw_console_getchar(void)
{
    if(!console_uart)return -1;
    return es32f0x_getc(console_uart);
}

#endif /* RT_USING_FINSH */

