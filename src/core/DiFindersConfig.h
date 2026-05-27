#pragma once

#ifndef DI_FINDERS_ENABLE_DEBUG
#define DI_FINDERS_ENABLE_DEBUG 0
#endif

#ifndef DI_FINDERS_ENABLE_TEST_API
#define DI_FINDERS_ENABLE_TEST_API 0
#endif

#ifndef DI_FINDERS_DEFAULT_ADC_MAX
#define DI_FINDERS_DEFAULT_ADC_MAX 1023U
#endif

/* SPI burst / UART RX buffer fast path (ESP32, RP2040, STM32). Off on AVR. */
#ifndef DI_FINDERS_ENABLE_DMA
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)
#define DI_FINDERS_ENABLE_DMA 1
#else
#define DI_FINDERS_ENABLE_DMA 0
#endif
#endif
