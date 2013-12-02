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
#ifndef   _SYSDEF_MB9BF618T_H_
#define   _SYSDEF_MB9BF618T_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define FM3_PERIPH_BASE (0x40000000UL)
#define FM3_GPIO_BASE   (FM3_PERIPH_BASE + 0x33000UL)
#define MFS0_UART_BASE  (FM3_PERIPH_BASE + 0x38000UL)
#define MFS1_UART_BASE  (FM3_PERIPH_BASE + 0x38100UL)
#define MFS2_UART_BASE  (FM3_PERIPH_BASE + 0x38200UL)
#define MFS3_UART_BASE  (FM3_PERIPH_BASE + 0x38300UL)

#define __IO	volatile
/******************************************************************************
 * GPIO_MODULE General purpose I/O registers
 ******************************************************************************/
typedef struct
{ 
    __IO uint32_t PFR0;
    __IO uint32_t PFR1;
    __IO uint32_t PFR2;
    __IO uint32_t PFR3;
    __IO uint32_t PFR4;
    __IO uint32_t PFR5;
    __IO uint32_t PFR6;
    __IO uint32_t PFR7;
    __IO uint32_t PFR8;
    __IO uint32_t PFR9;
    __IO uint32_t PFRA;
    __IO uint32_t PFRB;
    __IO uint32_t PFRC;
    __IO uint32_t PFRD;
    __IO uint32_t PFRE;
    __IO uint32_t PFRF;
        uint8_t RESERVED0[192];
    __IO uint32_t PCR0;
    __IO uint32_t PCR1;
    __IO uint32_t PCR2;
    __IO uint32_t PCR3;
    __IO uint32_t PCR4;
    __IO uint32_t PCR5;
    __IO uint32_t PCR6;
    __IO uint32_t PCR7;
    __IO uint32_t PCR8;
    __IO uint32_t PCR9;
    __IO uint32_t PCRA;
    __IO uint32_t PCRB;
    __IO uint32_t PCRC;
    __IO uint32_t PCRD;
    __IO uint32_t PCRE;
    __IO uint32_t PCRF;
        uint8_t RESERVED1[192];
    __IO uint32_t DDR0;
    __IO uint32_t DDR1;
    __IO uint32_t DDR2;
    __IO uint32_t DDR3;
    __IO uint32_t DDR4;
    __IO uint32_t DDR5;
    __IO uint32_t DDR6;
    __IO uint32_t DDR7;
    __IO uint32_t DDR8;
    __IO uint32_t DDR9;
    __IO uint32_t DDRA;
    __IO uint32_t DDRB;
    __IO uint32_t DDRC;
    __IO uint32_t DDRD;
    __IO uint32_t DDRE;
    __IO uint32_t DDRF;
        uint8_t RESERVED2[192];
    __IO uint32_t PDIR0;
    __IO uint32_t PDIR1;
    __IO uint32_t PDIR2;
    __IO uint32_t PDIR3;
    __IO uint32_t PDIR4;
    __IO uint32_t PDIR5;
    __IO uint32_t PDIR6;
    __IO uint32_t PDIR7;
    __IO uint32_t PDIR8;
    __IO uint32_t PDIR9;
    __IO uint32_t PDIRA;
    __IO uint32_t PDIRB;
    __IO uint32_t PDIRC;
    __IO uint32_t PDIRD;
    __IO uint32_t PDIRE;
    __IO uint32_t PDIRF;
        uint8_t RESERVED3[192];
    __IO uint32_t PDOR0;
    __IO uint32_t PDOR1;
    __IO uint32_t PDOR2;
    __IO uint32_t PDOR3;
    __IO uint32_t PDOR4;
    __IO uint32_t PDOR5;
    __IO uint32_t PDOR6;
    __IO uint32_t PDOR7;
    __IO uint32_t PDOR8;
    __IO uint32_t PDOR9;
    __IO uint32_t PDORA;
    __IO uint32_t PDORB;
    __IO uint32_t PDORC;
    __IO uint32_t PDORD;
    __IO uint32_t PDORE;
    __IO uint32_t PDORF;
        uint8_t RESERVED4[192];
    __IO uint32_t ADE;
        uint8_t RESERVED5[124];
    __IO uint32_t SPSR;
        uint8_t RESERVED6[124];
    __IO uint32_t EPFR00;
    __IO uint32_t EPFR01;
    __IO uint32_t EPFR02;
    __IO uint32_t EPFR03;
    __IO uint32_t EPFR04;
    __IO uint32_t EPFR05;
    __IO uint32_t EPFR06;
    __IO uint32_t EPFR07;
    __IO uint32_t EPFR08;
    __IO uint32_t EPFR09;
    __IO uint32_t EPFR10;
    __IO uint32_t EPFR11;
    __IO uint32_t EPFR12;
    __IO uint32_t EPFR13;
    __IO uint32_t EPFR14;
    __IO uint32_t EPFR15;
        uint8_t RESERVED7[192];
    __IO uint32_t PZR0;
    __IO uint32_t PZR1;
    __IO uint32_t PZR2;
    __IO uint32_t PZR3;
    __IO uint32_t PZR4;
    __IO uint32_t PZR5;
    __IO uint32_t PZR6;
    __IO uint32_t PZR7;
    __IO uint32_t PZR8;
    __IO uint32_t PZR9;
    __IO uint32_t PZRA;
    __IO uint32_t PZRB;
    __IO uint32_t PZRC;
    __IO uint32_t PZRD;
    __IO uint32_t PZRE;
    __IO uint32_t PZRF;
}FM3_GPIO_TypeDef;

#define FM3_GPIO        ((FM3_GPIO_TypeDef *)FM3_GPIO_BASE)

/******************************************************************************
 * MFS_UART_MODULE UART asynchronous channel registers
 ******************************************************************************/
typedef struct
{
    __IO  uint8_t SMR;
    __IO  uint8_t SCR;
        uint8_t RESERVED0[2];
    __IO  uint8_t ESCR;
    __IO  uint8_t SSR;
        uint8_t RESERVED1[2];
  union {
      __IO uint16_t RDR;
      __IO uint16_t TDR;
  };
        uint8_t RESERVED2[2];
  union {
      __IO uint16_t BGR;
    struct {
      __IO  uint8_t BGR0;
        __IO  uint8_t BGR1;
    };
  };
}FM3_MFS03_UART_TypeDef;

#define UART0           ((FM3_MFS03_UART_TypeDef *)MFS0_UART_BASE)
#define UART1           ((FM3_MFS03_UART_TypeDef *)MFS1_UART_BASE)
#define UART2           ((FM3_MFS03_UART_TypeDef *)MFS2_UART_BASE)
#define UART3           ((FM3_MFS03_UART_TypeDef *)MFS3_UART_BASE)

/*
 *  UART Register Bit Definition
 */
#define SMR_SOE          0x01U
#define SMR_BDS          0x04U
#define SMR_SBL          0x08U
#define SMR_WUCR         0x10U
#define SMR_MD_UART      0x00U
#define SMR_MD_UART_MP   0x20U
#define SMR_MD_SIO       0x40U
#define SMR_MD_LIN       0x60U
#define SMR_MD_I2C       0x80U

#define SCR_TXE          0x01U
#define SCR_RXE          0x02U
#define SCR_TBIE         0x04U
#define SCR_TIE          0x08U
#define SCR_RIE          0x10U
#define SCR_UPGL         0x80U

#define SSR_TBI          0x01U
#define SSR_TDRE         0x02U
#define SSR_RDRF         0x04U
#define SSR_ORE          0x08U
#define SSR_FRE          0x10U
#define SSR_PE           0x20U
#define SSR_REC          0x80U

#define ESCR_P           0x08U
#define ESCR_PEN         0x10U
#define ESCR_INV         0x20U
#define ESCR_ESBL        0x40U
#define ESCR_FLWEN       0x80U
#define ESCR_DATABITS_8  0x00U
#define ESCR_DATABITS_5  0x01U
#define ESCR_DATABITS_6  0x02U
#define ESCR_DATABITS_7  0x03U
#define ESCR_DATABITS_9  0x04U

#ifdef __cplusplus
}
#endif

#endif /* _SYSDEF_MB9BF618T_H_ */
