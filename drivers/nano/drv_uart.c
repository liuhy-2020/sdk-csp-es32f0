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

#include <rthw.h>
#include <rtthread.h>
#include "board.h"
#include "drv_uart.h"
#include "es_conf_info_map.h"
#include <ald_uart.h>
#include <ald_usart.h>
#include <ald_gpio.h>
#include <ald_cmu.h>

#ifdef RT_USING_CONSOLE


/* es32 uart driver */
struct es32_uart
{
    uart_handle_t huart;
    IRQn_Type irq;
};                

/* es32 usart driver */
struct es32_usart
{
    usart_handle_t huart;
    IRQn_Type irq;
};

struct es32_uart *console_uart = RT_NULL;

static rt_err_t es32f0x_configure(struct es32_uart *uart)
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
        if((usart->huart.perh) == USART0)
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
        if((usart->huart.perh) == USART1)
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
        
        ald_usart_interrupt_config(&usart->huart, USART_IT_RXNE, ENABLE);                         
        
    }
    else
    {
       /*UART*/
       
#ifdef BSP_USING_UART0
        if((uart->huart.perh) == UART0)
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
        if((uart->huart.perh) == UART1)
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
        if((uart->huart.perh) == UART2)
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
        if((uart->huart.perh) == UART3)
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
        uart->huart.init.word_length = (uart_word_length_t)(3); 
        uart->huart.init.stop_bits   = (uart_stop_bits_t)(1);
        uart->huart.init.parity      = (uart_parity_t)(0);
        uart->huart.init.fctl        = UART_HW_FLOW_CTL_DISABLE; 

        ald_uart_init(&uart->huart);

        /* enable rx int */
        ald_uart_interrupt_config(&uart->huart, UART_IT_RXRD, ENABLE);
            
    }
    

    return RT_EOK;
}

#ifdef BSP_USING_UART0
/* UART0 device driver structure */
struct es32_uart uart0 =
{
    {UART0},
    UART0_IRQn
};


void UART0_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    ald_uart_irq_handler(&(uart0.huart));

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART0 */

#ifdef BSP_USING_UART1
/* UART1 device driver structure */
struct es32_uart uart1 =
{
    {UART1},
    UART1_IRQn
};


void UART1_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    ald_uart_irq_handler(&(uart1.huart));

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART1 */

#ifdef BSP_USING_UART2
/* UART2 device driver structure */
struct es32_uart uart2 =
{
    {UART2},
    BS16T1_UART2_IRQn
};


void BS16T1_UART2_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    ald_uart_irq_handler(&(uart2.huart));

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART2 */

#ifdef BSP_USING_UART3
/* UART3 device driver structure */
struct es32_uart uart3 =
{
    {UART3},
    BS16T2_UART3_IRQn
};

void BS16T2_UART3_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    ald_uart_irq_handler(&(uart3.huart));

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART3 */
        
#ifdef BSP_USING_USART0
/* USART0 device driver structure */
struct es32_usart usart0 =
{
    {USART0},
    USART0_IRQn
};


void USART0_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    ald_usart_irq_handler(&(usart0.huart));

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_USART0 */

#ifdef BSP_USING_USART1
/* USART1 device driver structure */
struct es32_usart usart1 =
{
    {USART1},
    USART1_IRQn
};


void USART1_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    ald_usart_irq_handler(&(usart1.huart));

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_USART1 */

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
//INIT_BOARD_EXPORT(rt_hw_uart_init);

static int es32f0x_putc(struct es32_uart *uart, char c)
{
    
    
    if((uint32_t)(uart->huart.perh) > (uint32_t)UART3)  /*根据外设物理地址区分UART和USART*/
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

void rt_hw_console_output(const char *str)
{
    rt_size_t len = 0;
    char *s =(char*)str;

    len = rt_strlen(str);

    if(!console_uart)return;

while(len--)
{
      if(*s == '\n')es32f0x_putc(console_uart,'\r');
      es32f0x_putc(console_uart,*(s++));
}
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

#endif
#endif
