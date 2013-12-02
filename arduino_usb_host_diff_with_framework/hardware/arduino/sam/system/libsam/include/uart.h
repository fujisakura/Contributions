/*******************************************************************************
* Copyright (C) 2013 Spansion LLC. All Rights Reserved. 
*
* This software is owned and published by: 
* Spansion LLC, 915 DeGuigne Dr. Sunnyvale, CA  94088-3453 ("Spansion").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND 
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with Spansion 
* components. This software is licensed by Spansion to be adapted only 
* for use in systems utilizing Spansion components. Spansion shall not be 
* responsible for misuse or illegal use of this software for devices not 
* supported herein.  Spansion is providing this software "AS IS" and will 
* not be responsible for issues arising from incorrect user implementation 
* of the software.  
*
* SPANSION MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS), 
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING, 
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED 
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED 
* WARRANTY OF NONINFRINGEMENT.  
* SPANSION SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT, 
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT 
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, 
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR 
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA, 
* SAVINGS OR PROFITS, 
* EVEN IF SPANSION HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED 
* FROM, THE SOFTWARE.  
*
* This software may be replicated in part or whole for the licensed use, 
* with the restriction that this Disclaimer and Copyright notice must be 
* included with each copy of this software, whether used in part or whole, 
* at all times.  
*/

/************************************************************************/
/** \file uart.c
 **
 ** uart file
 **
 ** History:
 **   - 2011-03-30  1.0  MSc  First Version for FM3
 *************************************************************************/

#ifndef __UART_H__
#define __UART_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#ifndef INCLUDE_FROM_CHIP_H
#include "mcu.h"
#else
#include "sysdef_depend.h"
#endif
#include "../source/spansion/base_type.h"
#include <string.h>

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
#define USE_PRINTF      0
#if (USE_PRINTF == 1)
    #include <stdio.h>
#endif
#define  HEAP_SIZE     16

#ifndef CLOCK_FREQ
    #define CLOCK_FREQ 40000000UL
#endif

#define UART_BAUDRATE(x)  ((SystemCoreClock / 2 + (x / 2)) / x - 1)
/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

boolean_t Uart_Init(uint8_t u8Uart, uint16_t Baudrate);
void Uart_Putch(uint8_t u8Uart, char_t c);
void putch(char_t c);
char_t Uart_Getch(uint8_t u8Uart);
char_t getch();
void Uart_Puts(uint8_t u8Uart, char_t* String);
int puts(const char_t* String);
void puthex(uint32_t n, uint8_t digits);
void putdec(uint32_t x);
unsigned long ASCIItobin(uint8_t k);
void receive_line(void);
int receive_line_echo(int *cnt);
int scan_line(char_t *str) ;
unsigned long Inputhex(uint8_t digits);
char upcase(char_t k);

/* Low-Level functions */
#if (USE_PRINTF == 1)
    int __write(int , char *, unsigned int);
    int __close(int);
    int __read(int , char *, unsigned int);
#endif /* (USE_PRINTF == 1) */

#endif /* __UART_H__ */
