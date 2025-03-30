//created by Oxfrd 8.11.2023

#include <cassert>
#include <chrono>

#include "ITimer.h"
#include "IGpio.h"
#include "IUart.h"

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

namespace board
{
std::shared_ptr<hal::mcu::mcuManager> init()
{
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
        auto gpio = std::make_shared<mcu::gpio::gpioOutput>(2, portB, hal::gpio::eTermination::ePullUp, false);
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

    // hal::timer::period_t timing(5.0);
    auto timer = std::make_shared<mcu::timer::countingTimer>(TIM2);//, timing);
    err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eTimer2), std::move(timer));
    checkErr(err);


    std::shared_ptr<mcu::timer::countingTimer> tim2{nullptr};
    {
        auto timGetter = tim2->getPtr(static_cast<uint16_t>(eResourcesList::eTimer2), mcu);
        if (timGetter.second == eError::eOk)
        {
            tim2 = std::dynamic_pointer_cast<mcu::timer::countingTimer>(timGetter.first);
        }
    }

    {
        auto interrupt = std::make_shared<mcu::interrupt::timeInterrupt>(TIM2_IRQn, tim2);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eIntTim2), std::move(interrupt));
        checkErr(err);
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

    {
        auto i2c1 = std::make_shared<mcu::i2c::I2c>(I2C1, B6, B7, i2cEventInt);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eI2c1), std::move(i2c1));
        checkErr(err);
    }

    std::shared_ptr<hal::i2c::II2c> I2C1Handle{nullptr};
    {
        auto getter = I2C1Handle->getPtr(static_cast<uint16_t>(eResourcesList::eI2c1),mcu);
        if (getter.second == eError::eOk)
        {
            I2C1Handle = std::dynamic_pointer_cast<hal::i2c::II2c>(getter.first);
        } else { checkErr(getter.second);}
    }

    {
         auto bmp = std::make_shared<module::BMP280>(I2C1Handle);
         err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eBMP280), std::move(bmp));
         checkErr(err);
    }

    {
        auto gpio = std::make_shared<mcu::gpio::gpioOutAndInput>(0, portD, hal::gpio::eMode::eInput, 
            hal::gpio::eTermination::ePullDown);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eGPIO_D0), std::move(gpio));
        checkErr(err);
    }

    std::shared_ptr<hal::interrupt::ITimeInterrupt> tim2interrupt{nullptr};
    {
        auto timGetter = tim2interrupt->getPtr(static_cast<uint16_t>(eResourcesList::eIntTim2), mcu);
        if (timGetter.second == eError::eOk)
        {
            tim2interrupt = timGetter.first;
        }
    }

    {
        auto delay = std::make_shared<mcu::delay::delay>(tim2interrupt);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eResourcesList::eDelay), std::move(delay));
        checkErr(err);
    }

    return std::move(mcu);
}

}   //namespace board
