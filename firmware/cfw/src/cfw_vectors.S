/*
 * cfw_vectors.S — Cortex-M3 vector table + reset for the DSO152 CFW.
 *
 * The CH32F103C8 has ~62 external interrupt vectors. This table lives at
 * the start of FLASH (.cfv_vector_table @ 0x08000000). Most handlers are
 * weak aliases to Default_Handler (an infinite loop) so missing ones are
 * safe; a full build overrides the USB/ADC/EXTI ones with real handlers.
 */
    .section .cfv_vector_table, "a"
    .global cfv_vector_table
    .type  cfv_vector_table, %object

cfv_vector_table:
    .word  0x20005000          /* 0: initial SP = top of 20KB SRAM */
    .word  Reset_Handler       /* 1: reset */
    .word  NMI_Handler
    .word  HardFault_Handler
    .word  MemManage_Handler
    .word  BusFault_Handler
    .word  UsageFault_Handler
    .word  0                  /* 7-10: reserved */
    .word  0
    .word  0
    .word  0
    .word  SVCall_Handler
    .word  DebugMon_Handler
    .word  0                  /* 15: reserved */
    .word  PendSV_Handler
    .word  SysTick_Handler

    /* CH32F103 external IRQs (vector 16..76) */
    .word  WWDG_IRQHandler           /* 16 */
    .word  PVD_IRQHandler           /* 17 */
    .word  TAMP_IRQHandler          /* 18 */
    .word  RTC_IRQHandler           /* 19 */
    .word  EXTI0_IRQHandler         /* 20 */
    .word  EXTI1_IRQHandler         /* 21 */
    .word  EXTI2_IRQHandler         /* 22 */
    .word  EXTI3_IRQHandler         /* 23 */
    .word  EXTI4_IRQHandler         /* 24 */
    .word  ADC1_2_IRQHandler        /* 25 */
    .word  USB_LP_CAN1_RX0_IRQHandler /* 26 */
    .word  CAN1_RX1_IRQHandler      /* 27 */
    .word  CAN1_SCE_IRQHandler     /* 28 */
    .word  BKP_IRQHandler           /* 29 */
    .word  RTC_OVF_IRQHandler       /* 30 */
    .word  FLASH_ERR_IRQHandler     /* 31 */
    .word  TIM2_IRQHandler          /* 32 */
    .word  TIM3_IRQHandler          /* 33 */
    .word  TIM4_IRQHandler          /* 34 */
    .word  I2C1_IRQHandler          /* 35 */
    .word  I2C2_IRQHandler          /* 36 */
    .word  SPI1_IRQHandler          /* 37 */
    .word  SPI2_IRQHandler          /* 38 */
    .word  USART1_IRQHandler        /* 39 */
    .word  USART2_IRQHandler        /* 40 */
    .word  USART3_IRQHandler        /* 41 */
    .word  EXTI15_10_IRQHandler     /* 42 */
    .word  RTCAlarm_IRQHandler      /* 43 */
    .word  TIM8_IRQHandler          /* 44 */
    .word  TIM5_IRQHandler          /* 45 */
    .word  TIM6_IRQHandler          /* 46 */
    .word  TIM7_IRQHandler          /* 47 */
    .word  ETH_WKUP_IRQHandler      /* 48 */
    .word  ETH_IRQHandler           /* 49 */
    .word  CAN1_TX_IRQHandler       /* 50 */
    .word  ISO7816_IRQHandler       /* 51 */
    .word  LCD_IRQHandler           /* 52 */
    .word  TIM15_IRQHandler         /* 53 */
    .word  TIM16_IRQHandler         /* 54 */
    .word  TIM17_IRQHandler         /* 55 */
    .word  USBFS_IRQHandler         /* 56 */
    .word  USBFS_WKUP_IRQHandler    /* 57 */
    .word  DMA1_channel6to1_IRQHandler /* 58 */
    .word  ADC3_IRQHandler          /* 59 */
    .word  USBFS_WKUP_EXTI10_IRQHandler /* 60 */
    .word  TIM1_TRG_BRK_IRQHandler  /* 61 */
    .word  TIM1_UP_IRQHandler       /* 62 */
    .word  TIM1_GLOBAL_IRQHandler   /* 63 */
    .word  TIM1_CC_IRQHandler       /* 64 */
    .word  EXTI9_5_IRQHandler       /* 65 */
    .word  TIM10_IRQHandler         /* 66 */
    .word  TIM11_IRQHandler         /* 67 */
    .word  TIM12_IRQHandler         /* 68 */
    .word  SPI4_IRQHandler          /* 69 */
    .word  SPI5_IRQHandler          /* 70 */
    .word  SPI6_IRQHandler          /* 71 */
    .word  TIM14_IRQHandler         /* 72 */
    .word  TIM13_IRQHandler         /* 73 */

/* ------------------------------------------------------------------ */
/* Reset handler: init stack, bring up clocks, then jump to cfw_main.  */
/* ------------------------------------------------------------------ */
    .section .text
    .thumb
    .global Reset_Handler
    .type  Reset_Handler, %function
Reset_Handler:
    ldr   r0, =_estack
    mov   sp, r0
    bl    cfw_init_data
    bl    system_init
    bl    cfw_redirect_vectors
    bl    cfw_main
    b     .                       /* cfw_main never returns */
    .size  Reset_Handler, . - Reset_Handler

/* Default IRQ handler: infinite loop. */
Default_Handler:
    b     .
    .size  Default_Handler, . - Default_Handler

/* Weak-alias every unimplemented handler to Default_Handler. */
    .macro def_irq name
    .weak  \name
    .set   \name, Default_Handler
    .endm

    def_irq NMI_Handler
    def_irq HardFault_Handler
    def_irq MemManage_Handler
    def_irq BusFault_Handler
    def_irq UsageFault_Handler
    def_irq SVCall_Handler
    def_irq DebugMon_Handler
    def_irq PendSV_Handler
    def_irq SysTick_Handler
    def_irq WWDG_IRQHandler
    def_irq PVD_IRQHandler
    def_irq TAMP_IRQHandler
    def_irq RTC_IRQHandler
    def_irq EXTI0_IRQHandler
    def_irq EXTI1_IRQHandler
    def_irq EXTI2_IRQHandler
    def_irq EXTI3_IRQHandler
    def_irq EXTI4_IRQHandler
    def_irq ADC1_2_IRQHandler
    def_irq USB_LP_CAN1_RX0_IRQHandler
    def_irq CAN1_RX1_IRQHandler
    def_irq CAN1_SCE_IRQHandler
    def_irq BKP_IRQHandler
    def_irq RTC_OVF_IRQHandler
    def_irq FLASH_ERR_IRQHandler
    def_irq TIM2_IRQHandler
    def_irq TIM3_IRQHandler
    def_irq TIM4_IRQHandler
    def_irq I2C1_IRQHandler
    def_irq I2C2_IRQHandler
    def_irq SPI1_IRQHandler
    def_irq SPI2_IRQHandler
    def_irq USART1_IRQHandler
    def_irq USART2_IRQHandler
    def_irq USART3_IRQHandler
    def_irq EXTI15_10_IRQHandler
    def_irq RTCAlarm_IRQHandler
    def_irq TIM8_IRQHandler
    def_irq TIM5_IRQHandler
    def_irq TIM6_IRQHandler
    def_irq TIM7_IRQHandler
    def_irq ETH_WKUP_IRQHandler
    def_irq ETH_IRQHandler
    def_irq CAN1_TX_IRQHandler
    def_irq ISO7816_IRQHandler
    def_irq LCD_IRQHandler
    def_irq TIM15_IRQHandler
    def_irq TIM16_IRQHandler
    def_irq TIM17_IRQHandler
    def_irq USBFS_IRQHandler
    def_irq USBFS_WKUP_IRQHandler
    def_irq DMA1_channel6to1_IRQHandler
    def_irq ADC3_IRQHandler
    def_irq USBFS_WKUP_EXTI10_IRQHandler
    def_irq TIM1_TRG_BRK_IRQHandler
    def_irq TIM1_UP_IRQHandler
    def_irq TIM1_GLOBAL_IRQHandler
    def_irq TIM1_CC_IRQHandler
    def_irq EXTI9_5_IRQHandler
    def_irq TIM10_IRQHandler
    def_irq TIM11_IRQHandler
    def_irq TIM12_IRQHandler
    def_irq SPI4_IRQHandler
    def_irq SPI5_IRQHandler
    def_irq SPI6_IRQHandler
    def_irq TIM14_IRQHandler
    def_irq TIM13_IRQHandler
