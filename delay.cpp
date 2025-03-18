#include "delay.h"
#include "stm32l476xx.h"

static std::uint32_t mgDelay1msCnt = 0;

void delayMe(std::uint32_t ms)
{
    std::uint32_t finalTimestamp = mgDelay1msCnt + ms;
    while(mgDelay1msCnt <= finalTimestamp)
    {
        asm("NOP");
    }
}

__attribute__((interrupt)) void TIM2_IRQHandler()
{
    ++mgDelay1msCnt;
    TIM2->SR &= ~TIM_SR_UIF;
}
