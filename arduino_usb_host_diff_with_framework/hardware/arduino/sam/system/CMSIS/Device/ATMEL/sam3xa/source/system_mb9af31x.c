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

#include "mcu.h"

/** \file system_mb9af31x.c
 **
 ** FM3 system initialization functions
 ** All adjustments can be done in belonging header file.
 **
 ** History:
 ** 2011-05-16 V1.0 MWi original version
 ******************************************************************************/

/**
 ******************************************************************************
 ** System Clock Frequency (Core Clock) Variable according CMSIS
 ******************************************************************************/
uint32_t SystemCoreClock = __HCLK;

/**
 ******************************************************************************
 ** \brief  Update the System Core Clock with current core Clock retrieved from
 ** cpu registers.
 ** \param  none
 ** \return none
 ******************************************************************************/
void SystemCoreClockUpdate (void) {
  uint32_t masterClk;
  uint32_t u32RegisterRead; // Workaround variable for MISRA C rule conformance

  switch ((FM3_CRG->SCM_CTL >> 5) & 0x07) {
    case 0:                                 /* internal High-speed Cr osc.    */
      masterClk = __CLKHC;
      break;

    case 1:                                 /* external main osc.             */
      masterClk = __CLKMO;
      break;

    case 2:                                 /* PLL clock                      */
  // Workaround for preventing MISRA C:1998 Rule 46 (MISRA C:2004 Rule 12.2)
  // violation:
  //   "Unordered accesses to a volatile location"
      u32RegisterRead = (__CLKMO  * (((FM3_CRG->PLL_CTL2) & 0x1F) + 1));
      masterClk = (u32RegisterRead / (((FM3_CRG->PLL_CTL1 >> 4) & 0x0F) + 1));
      break;

    case 4:                                 /* internal Low-speed CR osc.     */
      masterClk = __CLKLC;
      break;

    case 5:                                 /* external Sub osc.              */
      masterClk = __CLKSO;
      break;

    default:
      masterClk = 0Ul;
      break;
  }

  switch (FM3_CRG->BSC_PSR & 0x07) {
    case 0:
      SystemCoreClock = masterClk;
      break;

    case 1:
      SystemCoreClock = masterClk / 2;
      break;

    case 2:
      SystemCoreClock = masterClk / 3;
      break;

    case 3:
      SystemCoreClock = masterClk / 4;
      break;

    case 4:
      SystemCoreClock = masterClk / 6;
      break;

    case 5:
      SystemCoreClock = masterClk /8;
      break;

    case 6:
      SystemCoreClock = masterClk /16;
      break;

    default:
      SystemCoreClock = 0Ul;
      break;
  }

}

/**
 ******************************************************************************
 ** \brief  Setup the microcontroller system. Initialize the System and update
 ** the SystemCoreClock variable.
 **
 ** \param  none
 ** \return none
 ******************************************************************************/
void SystemInit (void) {

  static uint32_t u32IoRegisterRead;  // Workaround variable for MISRA C rule conformance
  
#if (HWWD_DISABLE)                                 /* HW Watchdog Disable */
  FM3_HWWDT->WDG_LCK = 0x1ACCE551;                 /* HW Watchdog Unlock */
  FM3_HWWDT->WDG_LCK = 0xE5331AAE;
  FM3_HWWDT->WDG_CTL = 0;                          /* HW Watchdog stop */
#endif

#if (CLOCK_SETUP)                                   /* Clock Setup */
  FM3_CRG->BSC_PSR   = BSC_PSR_Val;                /* set System Clock presacaler */
  FM3_CRG->APBC0_PSR = APBC0_PSR_Val;              /* set APB0 presacaler */
  FM3_CRG->APBC1_PSR = APBC1_PSR_Val;              /* set APB1 presacaler */
  FM3_CRG->APBC2_PSR = APBC2_PSR_Val;              /* set APB2 presacaler */
  FM3_CRG->SWC_PSR   = SWC_PSR_Val | (1UL << 7);   /* set SW Watchdog presacaler */
  FM3_CRG->TTC_PSR   = TTC_PSR_Val;                /* set Trace Clock presacaler */

  FM3_CRG->CSW_TMR   = CSW_TMR_Val;                /* set oscillation stabilization wait time */
  
  if (SCM_CTL_Val & (1UL << 1)) {                    /* Main clock oscillator enabled ? */
    FM3_CRG->SCM_CTL |= (1UL << 1);                /* enable main oscillator */ 
    while (!(FM3_CRG->SCM_STR & (1UL << 1)));      /* wait for Main clock oscillation stable */
  }
  
  if (SCM_CTL_Val & (1UL << 3)) {                    /* Sub clock oscillator enabled ? */
    FM3_CRG->SCM_CTL |= (1UL << 3);                /* enable sub oscillator */ 
    while (!(FM3_CRG->SCM_STR & (1UL << 3)));      /* wait for Sub clock oscillation stable */
  }

  FM3_CRG->PSW_TMR   = PSW_TMR_Val;                /* set PLL stabilization wait time */
  FM3_CRG->PLL_CTL1  = PLL_CTL1_Val;               /* set PLLM and PLLK */
  FM3_CRG->PLL_CTL2  = PLL_CTL2_Val;               /* set PLLN          */
  
  if (SCM_CTL_Val & (1UL << 4)) {                    /* PLL enabled ? */
    FM3_CRG->SCM_CTL  |= (1UL << 4);               /* enable PLL */ 
    while (!(FM3_CRG->SCM_STR & (1UL << 4)));      /* wait for PLL stable */
  }

  FM3_CRG->SCM_CTL  |= (SCM_CTL_Val & 0xE0);       /* Set Master Clock switch */ 
  
  // Workaround for preventing MISRA C:1998 Rule 46 (MISRA C:2004 Rule 12.2)
  // violations:
  //   "Unordered reads and writes to or from same location" and
  //   "Unordered accesses to a volatile location"
  do                                              
  {                                               
    u32IoRegisterRead = (FM3_CRG->SCM_CTL & 0xE0); 
  }while ((FM3_CRG->SCM_STR & 0xE0) != u32IoRegisterRead);
#endif // (CLOCK_SETUP)
  
#if (CR_TRIM_SETUP)
  /* CR Trimming Data  */
  if( 0x000003FF != (FM3_FLASH_IF->CRTRMM & 0x000003FF) )
  {
    /* UnLock (MCR_FTRM) */
    FM3_CRTRIM->MCR_RLR = 0x1ACCE554;
    /* Set MCR_FTRM */
    FM3_CRTRIM->MCR_FTRM = FM3_FLASH_IF->CRTRMM;
    /* Lock (MCR_FTRM) */
    FM3_CRTRIM->MCR_RLR = 0x00000000;
  }
#endif // (CR_TRIM_SETUP)

  /* Update the System Core Clock */
  SystemCoreClockUpdate();
}



