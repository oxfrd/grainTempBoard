#pragma once

#include <cstdint>


void delayMe(std::uint32_t ms);

#ifdef __cplusplus
 extern "C" {
#endif
    __attribute__((interrupt)) void TIM2_IRQHandler();
#ifdef __cplusplus
}
#endif
