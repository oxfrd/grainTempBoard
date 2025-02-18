//created by Oxfrd 8.11.2023

#include <cassert>
#include <chrono>

#include "ITimer.h"
#include "IGpio.h"
#include "IUart.h"

#include "mcuInit.h"
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

namespace mcu
{
std::shared_ptr<hal::mcu::mcuManager> init()
{
    auto mcu = std::make_shared<hal::mcu::mcuManager>(hal::mcu::mcuManager());
    if (mcu == nullptr)
    {
        assert(0);
    }

    auto portE = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(4));
    auto err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::ePortE),std::move(portE));
    checkErr(err);
    {
        auto getter = portE->getPtr(static_cast<uint16_t>(eMcuResources::ePortE),mcu);
        if (getter.second == eError::eOk)
        {
            portE = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    auto portB = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(1));
    err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::ePortB),std::move(portB));
    checkErr(err);
    {
        auto getter = portB->getPtr(static_cast<uint16_t>(eMcuResources::ePortB),mcu);
        if (getter.second == eError::eOk)
        {
            portB = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    auto portA = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(0));
    err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::ePortA),std::move(portA));
    checkErr(err);
    {
        auto getter = portA->getPtr(static_cast<uint16_t>(eMcuResources::ePortA),mcu);
        if (getter.second == eError::eOk)
        {
            portA = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    auto portD = std::make_shared<mcu::gpio::gpioPort>(mcu::gpio::gpioPort(3));
    err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::ePortD),std::move(portD));
    checkErr(err);
    {
        auto getter = portD->getPtr(static_cast<uint16_t>(eMcuResources::ePortD),mcu);
        if (getter.second == eError::eOk)
        {
            portD = std::dynamic_pointer_cast<mcu::gpio::gpioPort>(getter.first);
        }
    }

    {
        auto gpio = std::make_shared<gpio::gpioOutput>(8, portE, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_E8),std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<gpio::gpioOutput>(2, portB, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_B2), std::move(gpio));
        checkErr(err);
    }
    
    {
        auto gpio = std::make_shared<gpio::gpioInput>(0, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_A0), std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<gpio::gpioInput>(1, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_A1), std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<gpio::gpioInput>(2, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_A2), std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<gpio::gpioInput>(3, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_A3), std::move(gpio));
        checkErr(err);
    }

    {
        auto gpio = std::make_shared<gpio::gpioInput>(5, portA, hal::gpio::eTermination::ePullUp, false);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_A5), std::move(gpio));
        checkErr(err);
    }

    // hal::timer::period_t timing(5.0);
    auto timer = std::make_shared<timer::countingTimer>(TIM2);//, timing);
    err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eTimer2), std::move(timer));
    checkErr(err);


    std::shared_ptr<timer::countingTimer> tim2{nullptr};
    {
        auto timGetter = tim2->getPtr(static_cast<uint16_t>(eMcuResources::eTimer2), mcu);
        if (timGetter.second == eError::eOk)
        {
            tim2 = std::dynamic_pointer_cast<timer::countingTimer>(timGetter.first);
        }
    }

    {
        auto interrupt = std::make_shared<interrupt::timeInterrupt>(TIM2_IRQn, tim2);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eIntTim2), std::move(interrupt));
        checkErr(err);
    }

    {
        auto altFunGpio = std::make_shared<gpio::gpioAlternate>(5, portD);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_D5), std::move(altFunGpio));
        checkErr(err);
    }

    {
        auto altFunGpio = std::make_shared<gpio::gpioAlternate>(6, portD);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_D6), std::move(altFunGpio));
        checkErr(err);
    }


    std::shared_ptr<gpio::gpioAlternate> D5{nullptr};
    {
        auto getter = D5->getPtr(static_cast<uint16_t>(eMcuResources::eGPIO_D5),mcu);
        if (getter.second == eError::eOk)
        {
            D5 = std::dynamic_pointer_cast<gpio::gpioAlternate>(getter.first);
        } else { checkErr(getter.second);}
    }

    std::shared_ptr<gpio::gpioAlternate> D6{nullptr};
    {
        auto getter = D6->getPtr(static_cast<uint16_t>(eMcuResources::eGPIO_D6),mcu);
        if (getter.second == eError::eOk)
        {
            D6 = std::dynamic_pointer_cast<gpio::gpioAlternate>(getter.first);
        } else { checkErr(getter.second);}
    }
    
    {
        auto interrupt = std::make_shared<interrupt::interrupt>(USART2_IRQn);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eIntUART2), std::move(interrupt));
        checkErr(err);
    }

    std::shared_ptr<interrupt::interrupt> uart2int{nullptr};
    {
        auto getter = uart2int->getPtr(static_cast<uint16_t>(eMcuResources::eIntUART2),mcu);
        if (getter.second == eError::eOk)
        {
            uart2int = std::dynamic_pointer_cast<interrupt::interrupt>(getter.first);
        } else { checkErr(getter.second);}
    }

    {
        auto uart2 = std::make_shared<uart::uart>(USART2, D5, D6, uart2int, hal::uart::eBaudrate::e9600);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eUART2), std::move(uart2));
        checkErr(err);
    }

    {
        auto altFunGpio = std::make_shared<gpio::gpioAlternate>(6, portB);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_B6), std::move(altFunGpio));
        checkErr(err);
    }

    {
        auto altFunGpio = std::make_shared<gpio::gpioAlternate>(7, portB);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eGPIO_B7), std::move(altFunGpio));
        checkErr(err);
    }
    
    {
        auto interrupt = std::make_shared<interrupt::interrupt>(I2C1_EV_IRQn);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eInterruptI2C1Event), std::move(interrupt));
        checkErr(err);
    }

    std::shared_ptr<interrupt::interrupt> i2cEventInt{nullptr};
    {
        auto getter = i2cEventInt->getPtr(static_cast<std::uint16_t>(eMcuResources::eInterruptI2C1Event), mcu);
        if (getter.second == eError::eOk)
        {
            i2cEventInt = std::dynamic_pointer_cast<interrupt::interrupt>(getter.first);
        } else { checkErr(getter.second);}
    }

    std::shared_ptr<gpio::gpioAlternate> B6{nullptr};
    {
        auto getter = B6->getPtr(static_cast<uint16_t>(eMcuResources::eGPIO_B6),mcu);
        if (getter.second == eError::eOk)
        {
            B6 = std::dynamic_pointer_cast<gpio::gpioAlternate>(getter.first);
        } else { checkErr(getter.second);}
    }

    std::shared_ptr<gpio::gpioAlternate> B7{nullptr};
    {
        auto getter = B7->getPtr(static_cast<uint16_t>(eMcuResources::eGPIO_B7),mcu);
        if (getter.second == eError::eOk)
        {
            B7 = std::dynamic_pointer_cast<gpio::gpioAlternate>(getter.first);
        } else { checkErr(getter.second);}
    }

    {
        auto i2c1 = std::make_shared<i2c::I2c>(I2C1, B6, B7, i2cEventInt);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eI2c1), std::move(i2c1));
        checkErr(err);
    }

    std::shared_ptr<hal::i2c::II2c> I2C1Handle{nullptr};
    {
        auto getter = I2C1Handle->getPtr(static_cast<uint16_t>(eMcuResources::eI2c1),mcu);
        if (getter.second == eError::eOk)
        {
            I2C1Handle = std::dynamic_pointer_cast<hal::i2c::II2c>(getter.first);
        } else { checkErr(getter.second);}
    }

    {
        auto bmp = std::make_shared<module::BMP280>(I2C1Handle);
        err = mcu->reserveResource(static_cast<std::uint16_t>(eMcuResources::eBMP280), std::move(bmp));
        checkErr(err);
    }

    return std::move(mcu);
}

}   //namespace mcu
