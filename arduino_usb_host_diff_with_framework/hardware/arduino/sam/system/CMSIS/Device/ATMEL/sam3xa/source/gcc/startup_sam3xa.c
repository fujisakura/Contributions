/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#include "sam3xa.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
extern void MFS0RX_IRQHandler(void);
extern void USB1_Handler(void);

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

// Arduino: we must setup hardware before doing this
// void __libc_init_array(void);

// Arduino: handlers weak symbols moved into main

/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table = {

	/* Configure Initial Stack Pointer, using linker-generated symbols */
	(void*) (&_estack),
	(void*) Reset_Handler,

	(void*) NMI_Handler,
	(void*) HardFault_Handler,
	(void*) MemManage_Handler,
	(void*) BusFault_Handler,
	(void*) UsageFault_Handler,
	(void*) (0UL),           /* Reserved */
	(void*) (0UL),           /* Reserved */
	(void*) (0UL),           /* Reserved */
	(void*) (0UL),           /* Reserved */
	(void*) SVC_Handler,
	(void*) DebugMon_Handler,
	(void*) (0UL),           /* Reserved */
	(void*) PendSV_Handler,
	(void*) SysTick_Handler,

	/* Configurable interrupts */
	(void*) SUPC_Handler,    /*  0  Supply Controller */
	(void*) RSTC_Handler,    /*  1  Reset Controller */
	(void*) RTC_Handler,     /*  2  Real Time Clock */
	(void*) RTT_Handler,     /*  3  Real Time Timer */
	(void*) WDT_Handler,     /*  4  Watchdog Timer */
	(void*) PMC_Handler,     /*  5  PMC */
	(void*) EFC0_Handler,    /*  6  EFC 0 */
#if 0
	(void*) EFC1_Handler,    /*  7  EFC 1 */
#endif
	(void*) MFS0RX_IRQHandler,    /*  7  FM3 MFS0 Receive (UART0) */
	(void*) UART_Handler,    /*  8  UART */
#ifdef _SAM3XA_SMC_INSTANCE_
	(void*) SMC_Handler,     /*  9  SMC */
#else
	(void*) (0UL),           /*  9 Reserved */
#endif /* _SAM3XA_SMC_INSTANCE_ */
#ifdef _SAM3XA_SDRAMC_INSTANCE_
	(void*) SDRAMC_Handler,  /* 10  SDRAMC */
#else
	(void*) (0UL),           /* 10 Reserved */
#endif /* _SAM3XA_SDRAMC_INSTANCE_ */
	(void*) PIOA_Handler,    /* 11 Parallel IO Controller A */
	(void*) PIOB_Handler,    /* 12 Parallel IO Controller B */
#ifdef _SAM3XA_PIOC_INSTANCE_
	(void*) PIOC_Handler,    /* 13 Parallel IO Controller C */
#else
	(void*) (0UL),           /* 13 Reserved */
#endif /* _SAM3XA_PIOC_INSTANCE_ */
#ifdef _SAM3XA_PIOD_INSTANCE_
	(void*) PIOD_Handler,    /* 14 Parallel IO Controller D */
#else
	(void*) (0UL),           /* 14 Reserved */
#endif /* _SAM3XA_PIOD_INSTANCE_ */
#ifdef _SAM3XA_PIOE_INSTANCE_
	(void*) PIOE_Handler,    /* 15 Parallel IO Controller E */
#else
	(void*) (0UL),           /* 15 Reserved */
#endif /* _SAM3XA_PIOE_INSTANCE_ */
#ifdef _SAM3XA_PIOF_INSTANCE_
	(void*) PIOF_Handler,    /* 16 Parallel IO Controller F */
#else
	(void*) (0UL),           /* 16 Reserved */
#endif /* _SAM3XA_PIOF_INSTANCE_ */
	(void*) USART0_Handler,  /* 17 USART 0 */
	(void*) USART1_Handler,  /* 18 USART 1 */
	(void*) USART2_Handler,  /* 19 USART 2 */
#ifdef _SAM3XA_USART3_INSTANCE_
	(void*) USART3_Handler,  /* 20 USART 3 */
#else
	(void*) (0UL),           /* 20 Reserved */
#endif /* _SAM3XA_USART3_INSTANCE_ */
	(void*) HSMCI_Handler,   /* 21 MCI */
	(void*) TWI0_Handler,    /* 22 TWI 0 */
	(void*) TWI1_Handler,    /* 23 TWI 1 */
	(void*) SPI0_Handler,    /* 24 SPI 0 */
#ifdef _SAM3XA_SPI1_INSTANCE_
	(void*) SPI1_Handler,    /* 25 SPI 1 */
#else
	(void*) (0UL),           /* 25 Reserved */
#endif /* _SAM3XA_SPI1_INSTANCE_ */
	(void*) SSC_Handler,     /* 26 SSC */
	(void*) TC0_Handler,     /* 27 Timer Counter 0 */
	(void*) TC1_Handler,     /* 28 Timer Counter 1 */
	(void*) TC2_Handler,     /* 29 Timer Counter 2 */
	(void*) TC3_Handler,     /* 30 Timer Counter 3 */
	(void*) TC4_Handler,     /* 31 Timer Counter 4 */
	(void*) TC5_Handler,     /* 32 Timer Counter 5 */
#ifdef _SAM3XA_TC2_INSTANCE_
	(void*) TC6_Handler,     /* 33 Timer Counter 6 */
	(void*) TC7_Handler,     /* 34 Timer Counter 7 */
	(void*) TC8_Handler,     /* 35 Timer Counter 8 */
#else
	(void*) (0UL),           /* 33 Reserved */
	(void*) (0UL),           /* 34 Reserved */
	(void*) (0UL),           /* 35 Reserved */
#endif /* _SAM3XA_TC2_INSTANCE_ */
	(void*) PWM_Handler,     /* 36 PWM */
#if 0
	(void*) ADC_Handler,     /* 37 ADC controller */
#endif
	(void*) USB1_Handler,    /* 37 FM3 USB1 HOST */
	(void*) DACC_Handler,    /* 38 DAC controller */
	(void*) DMAC_Handler,    /* 39 DMA Controller */
	(void*) UOTGHS_Handler,  /* 40 USB OTG High Speed */
	(void*) TRNG_Handler,    /* 41 True Random Number Generator */
#ifdef _SAM3XA_EMAC_INSTANCE_
	(void*) EMAC_Handler,    /* 42 Ethernet MAC */
#else
	(void*) (0UL),           /* 42 Reserved */
#endif /* _SAM3XA_EMAC_INSTANCE_ */
	(void*) CAN0_Handler,    /* 43 CAN Controller 0 */
	(void*) CAN1_Handler    /* 44 CAN Controller 1 */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
	uint32_t *pSrc, *pDest;

	/* Initialize the relocate segment */
	pSrc = &_etext;
	pDest = &_srelocate;

	if (pSrc != pDest) {
		for (; pDest < &_erelocate;) {
			*pDest++ = *pSrc++;
		}
	}

	/* Clear the zero segment */
	for (pDest = &_szero; pDest < &_ezero;) {
		*pDest++ = 0;
	}

	/* Set the vector table base address */
	pSrc = (uint32_t *) & _sfixed;
	SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

	if (((uint32_t) pSrc >= IRAM0_ADDR) && ((uint32_t) pSrc < NFC_RAM_ADDR)) {
		SCB->VTOR |= (1UL) << SCB_VTOR_TBLBASE_Pos;
	}

	/* Initialize the C library */

	// Arduino: we must setup hardware before doing this
	//__libc_init_array();

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1);
}
