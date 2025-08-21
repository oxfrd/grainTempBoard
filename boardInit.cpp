//created by Oxfrd 8.11.2023

#include <cassert>
#include <chrono>

#include "ITimer.h"
#include "IGpio.h"
#include "IUart.h"
#include "IOneWire.h"

#include "boardInit.h"
#include "gpioPort.h"
#include "gpioOutput.h"
#include "timer.h"
#include "timeInterrupt.h"
#include "gpioInput.h"
#include "gpioAlternate.h"
#include "uart.h"
#include "interrupt.h"
#include "i2c.h"
#include "BMP280.h"
#include "gpioOutAndInput.h"
#include "delay.h"
#include "oneWire.h"
#include "DS18B20.h"
#include "blockingTimer.h"

void checkErr(eError err)
{
    if(err != eError::eOk)
    {
        assert(0);
        while (1)
        {
            asm("NOP");
        }
    }
}

void SystemClock_Config(void) {
    // Włącz HSI
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0);

    // Ustaw napięcie regulatora na Scale 1 (VOS = 0b11)
    PWR->CR1 &= ~(PWR_CR1_VOS);
    PWR->CR1 |= PWR_CR1_VOS;
    while (PWR->SR2 & PWR_SR2_VOSF);

    // Wyłącz PLL
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY);

    // Konfiguruj PLL: HSI/1 * 10 / 2 = 80 MHz
    RCC->PLLCFGR = 0; // wyczyść rejestr
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
    RCC->PLLCFGR |= (0 << RCC_PLLCFGR_PLLM_Pos); // PLLM = 1
    RCC->PLLCFGR |= (10 << RCC_PLLCFGR_PLLN_Pos); // PLLN = 10
    RCC->PLLCFGR |= (0 << RCC_PLLCFGR_PLLR_Pos); // PLLR = 2
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // włącz PLLR (system clock)

    // Włącz PLL
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);

    // Ustaw Flash latency na 4 WS
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS;

    // Przełącz system clock na PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    SystemCoreClockUpdate();
}
namespace board
{
std::shared_ptr<hal::mcu::mcuManager> init()
{
    SystemClock_Config();
    
    auto mcu = std::make_shared<hal::mcu::mcuManager>(hal::mcu::mcuManager());
    if (mcu == nullptr)
    {
        assert(0);
    }

    auto portE = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(4));
    auto err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::ePortE),std::move(portE));
    checkErr(err);
    {
        auto getter = portE->getPtr(static_cast<uint16_t>(eResourcesList::ePortE),mcu);
        if (getter.second == eError::eOk)
        {
            portE = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    auto portB = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(1));
    err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::ePortB),std::move(portB));
    checkErr(err);
    {
        auto getter = portB->getPtr(static_cast<uint16_t>(eResourcesList::ePortB),mcu);
        if (getter.second == eError::eOk)
        {
            portB = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    auto portA = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(0));
    err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::ePortA),std::move(portA));
    checkErr(err);
    {
        auto getter = portA->getPtr(static_cast<uint16_t>(eResourcesList::ePortA),mcu);
        if (getter.second == eError::eOk)
        {
            portA = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    auto portD = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(3));
    err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::ePortD),std::move(portD));
    checkErr(err);
    {
        auto getter = portD->getPtr(static_cast<uint16_t>(eResourcesList::ePortD),mcu);
        if (getter.second == eError::eOk)
        {
            portD = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    {
        auto gpio = std::make_shared<mcu::gpio::gpioOutput>(8, portE, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_E8),std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<mcu::gpio::gpioOutput>(2, portB, hal::gpio::eTermination::ePullUp, false, hal::gpio::eSpeed::eVeryHigh);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_B2), std::move(gpio));
        checkErr(err);
    }
    
    {
        auto gpio = std::make_shared<mcu::gpio::gpioInput>(0, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_A0), std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<mcu::gpio::gpioInput>(1, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_A1), std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<mcu::gpio::gpioInput>(2, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_A2), std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<mcu::gpio::gpioInput>(3, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_A3), std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<mcu::gpio::gpioInput>(5, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_A5), std::move(gpio));
        checkErr(err);
    }

    {
        auto timer = std::make_shared<mcu::blockingTimer::blockingTimer>(TIM2, SystemCoreClock);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eTimer2), std::move(timer));
        checkErr(err);
    }

    std::shared_ptr<mcu::blockingTimer::blockingTimer> blockingTimer{nullptr};
    {
        auto timGetter = blockingTimer->getPtr(static_cast<uint16_t>(eResourcesList::eTimer2), mcu);
        if (timGetter.second == eError::eOk)
        {
            blockingTimer = std::dynamic_pointer_cast<mcu::blockingTimer::blockingTimer>(timGetter.first);
        }
    }

    {
        auto altFunGpio = std::make_shared<mcu::gpio::gpioAlternate>(5, portD);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_D5), std::move(altFunGpio));
        checkErr(err);
    }

    {
        auto altFunGpio = std::make_shared<mcu::gpio::gpioAlternate>(6, portD);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_D6), std::move(altFunGpio));
        checkErr(err);
    }


    std::shared_ptr<mcu::gpio::gpioAlternate> D5{nullptr};
    {
        auto getter = D5->getPtr(static_cast<uint16_t>(eResourcesList::eGPIO_D5),mcu);
        if (getter.second == eError::eOk)
        {
            D5 = std::dynamic_pointer_cast<mcu::gpio::gpioAlternate>(getter.first);
        } else { checkErr(getter.second);}
    }

    std::shared_ptr<mcu::gpio::gpioAlternate> D6{nullptr};
    {
        auto getter = D6->getPtr(static_cast<uint16_t>(eResourcesList::eGPIO_D6),mcu);
        if (getter.second == eError::eOk)
        {
            D6 = std::dynamic_pointer_cast<mcu::gpio::gpioAlternate>(getter.first);
        } else { checkErr(getter.second);}
    }
    
    {
        auto interrupt = std::make_shared<mcu::interrupt::interrupt>(USART2_IRQn);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eIntUART2), std::move(interrupt));
        checkErr(err);
    }

    std::shared_ptr<mcu::interrupt::interrupt> uart2int{nullptr};
    {
        auto getter = uart2int->getPtr(static_cast<uint16_t>(eResourcesList::eIntUART2),mcu);
        if (getter.second == eError::eOk)
        {
            uart2int = std::dynamic_pointer_cast<mcu::interrupt::interrupt>(getter.first);
        } else { checkErr(getter.second);}
    }

    {
        auto uart2 = std::make_shared<mcu::uart::uart>(USART2, D5, D6, uart2int, hal::uart::eBaudrate::e9600);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eUART2), std::move(uart2));
        checkErr(err);
    }

    {
        auto altFunGpio = std::make_shared<mcu::gpio::gpioAlternate>(6, portB);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_B6), std::move(altFunGpio));
        checkErr(err);
    }

    {
        auto altFunGpio = std::make_shared<mcu::gpio::gpioAlternate>(7, portB);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_B7), std::move(altFunGpio));
        checkErr(err);
    }
    
    {
        auto interrupt = std::make_shared<mcu::interrupt::interrupt>(I2C1_EV_IRQn);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eInterruptI2C1Event), std::move(interrupt));
        checkErr(err);
    }

    std::shared_ptr<mcu::interrupt::interrupt> i2cEventInt{nullptr};
    {
        auto getter = i2cEventInt->getPtr(static_cast<std::uint16_t>(eResourcesList::eInterruptI2C1Event), mcu);
        if (getter.second == eError::eOk)
        {
            i2cEventInt = std::dynamic_pointer_cast<mcu::interrupt::interrupt>(getter.first);
        } else { checkErr(getter.second);}
    }

    std::shared_ptr<mcu::gpio::gpioAlternate> B6{nullptr};
    {
        auto getter = B6->getPtr(static_cast<uint16_t>(eResourcesList::eGPIO_B6),mcu);
        if (getter.second == eError::eOk)
        {
            B6 = std::dynamic_pointer_cast<mcu::gpio::gpioAlternate>(getter.first);
        } else { checkErr(getter.second);}
    }

    std::shared_ptr<mcu::gpio::gpioAlternate> B7{nullptr};
    {
        auto getter = B7->getPtr(static_cast<uint16_t>(eResourcesList::eGPIO_B7),mcu);
        if (getter.second == eError::eOk)
        {
            B7 = std::dynamic_pointer_cast<mcu::gpio::gpioAlternate>(getter.first);
        } else { checkErr(getter.second);}
    }

    // {
    //     auto i2c1 = std::make_shared<mcu::i2c::I2c>(I2C1, B6, B7, i2cEventInt);
    //     err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eI2c1), std::move(i2c1));
    //     checkErr(err);
    // }

    // std::shared_ptr<hal::i2c::II2c> I2C1Handle{nullptr};
    // {
    //     auto getter = I2C1Handle->getPtr(static_cast<uint16_t>(eResourcesList::eI2c1),mcu);
    //     if (getter.second == eError::eOk)
    //     {
    //         I2C1Handle = std::dynamic_pointer_cast<hal::i2c::II2c>(getter.first);
    //     } else { checkErr(getter.second);}
    // }

    // {
    //      auto bmp = std::make_shared<module::BMP280>(I2C1Handle);
    //      err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eBMP280), std::move(bmp));
    //      checkErr(err);
    // }

    {
        auto gpio = std::make_shared<mcu::gpio::gpioOutAndInput>(0, portD, hal::gpio::eMode::eInput, 
            hal::gpio::eTermination::eFloating);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_D0), std::move(gpio));
        checkErr(err);
    }

    {
        auto obj = std::make_shared<mcu::delay::delay>(blockingTimer);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eDelay), std::move(obj));
        checkErr(err);
    }

    std::shared_ptr<hal::delay::IDelay> delay{nullptr};
    {
        auto getter = delay->getPtr(static_cast<uint16_t>(eResourcesList::eDelay),mcu);
        if (getter.second == eError::eOk)
        {
            delay = getter.first;
        } else { checkErr(getter.second);}
    }

    auto portC = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(2));
    err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::ePortC),std::move(portC));
    checkErr(err);
    {
        auto getter = portC->getPtr(static_cast<uint16_t>(eResourcesList::ePortC),mcu);
        if (getter.second == eError::eOk)
        {
            portC = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    {
        auto pin = std::make_shared<mcu::gpio::gpioOutAndInput>(14, portC);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_C14), std::move(pin));
        checkErr(err);
    }

    std::shared_ptr<mcu::gpio::gpioOutAndInput> oneWire1Pin{nullptr};
    {
        auto getter = oneWire1Pin->getPtr(static_cast<uint16_t>(eResourcesList::eGPIO_C14),mcu);
        if (getter.second == eError::eOk)
        {
            oneWire1Pin = std::dynamic_pointer_cast<mcu::gpio::gpioOutAndInput>(getter.first);
        } else { checkErr(getter.second);}
    }

    std::shared_ptr<hal::gpio::IGpioOutAndInput> D0{nullptr};
    {
        auto getter = D0->getPtr(static_cast<uint16_t>(board::eResourcesList::eGPIO_D0), mcu);
        if (getter.second == eError::eOk)
        {
            D0 = getter.first;
        } else { checkErr(getter.second);}
    }

    {
        auto oneWire = std::make_shared<mcu::oneWire::oneWire>(D0, delay);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eOneWire1), std::move(oneWire));
        checkErr(err);
    }

    std::shared_ptr<mcu::oneWire::oneWire> oneWire1{nullptr};
    {
        auto getter = oneWire1->getPtr(static_cast<uint16_t>(eResourcesList::eOneWire1),mcu);
        if (getter.second == eError::eOk)
        {
            oneWire1 = std::dynamic_pointer_cast<mcu::oneWire::oneWire>(getter.first);
        } else { checkErr(getter.second);}
    }

    uint8_t testSensorAddress[8] = {0x28, 0xda, 0x20, 0xbd, 0x00, 0x00, 0x00, 0x00};
    uint8_t lowestSensorAddress[8] = {0x28, 0xe8, 0x3b, 0xbd, 0x00, 0x00, 0x00, 0x00};
    uint8_t middleSensorAddress[8] = {0x28, 0xb9, 0xf5, 0xbf, 0x00, 0x00, 0x00, 0x00};
    uint8_t highestSensorAddress[8] = {0x28, 0xbc, 0xe6, 0xbf, 0x00, 0x00, 0x00, 0x00};

    {
        auto DS18B20 = std::make_shared<module::DS18B20>(oneWire1, delay, testSensorAddress);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eDS18B20_1), std::move(DS18B20));
        checkErr(err);
    }

    {
        auto DS18B20 = std::make_shared<module::DS18B20>(oneWire1, delay, lowestSensorAddress);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eDS18B20_2), std::move(DS18B20));
        checkErr(err);
    }

    {
        auto DS18B20 = std::make_shared<module::DS18B20>(oneWire1, delay, middleSensorAddress);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eDS18B20_3), std::move(DS18B20));
        checkErr(err);
    }

    {
        auto DS18B20 = std::make_shared<module::DS18B20>(oneWire1, delay, highestSensorAddress);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eDS18B20_4), std::move(DS18B20));
        checkErr(err);
    }

    {
        auto interrupt = std::make_shared<mcu::interrupt::interrupt>(USART1_IRQn);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eIntUART1), std::move(interrupt));
        checkErr(err);
    }

    std::shared_ptr<mcu::interrupt::interrupt> uart1int{nullptr};
    {
        auto getter = uart1int->getPtr(static_cast<uint16_t>(eResourcesList::eIntUART1),mcu);
        if (getter.second == eError::eOk)
        {
            uart1int = std::dynamic_pointer_cast<mcu::interrupt::interrupt>(getter.first);
        } else { checkErr(getter.second);}
    }

    {
        auto uart1 = std::make_shared<mcu::uart::uart>(USART1, B6, B7, uart1int, hal::uart::eBaudrate::e9600);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eUART1), std::move(uart1));
        checkErr(err);
    }

    return std::move(mcu);
}

}   //namespace board
