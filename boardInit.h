//created by Oxfrd 8.11.2023
#pragma once

#include "IMcu.h"

#define HSE_VALUE 32768000

namespace board
{
    enum class eResourcesList 
    {
        ePortB,
        ePortE,
        eGPIO_E8,
        eGPIO_B2,
        eTimer2,
        eIntTim2,
        ePortA,
        eGPIO_A0,
        eGPIO_A1,
        eGPIO_A2,
        eGPIO_A3,
        eGPIO_A5,
        ePortD,
        eGPIO_D5,
        eGPIO_D6,
        eUART2,
        eIntUART2,
        eGPIO_B6,
        eGPIO_B7,
        eI2c1,
        eBMP280,
        eInterruptI2C1Event,
        eGPIO_D0,
        eDelay,
        ePortC,
        eGPIO_C14,
        eOneWire1,
        eDS18B20_1,
        eEeeprom1
    };

    std::shared_ptr<hal::mcu::mcuManager> init();
}   //board