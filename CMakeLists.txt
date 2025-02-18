cmake_minimum_required(VERSION 3.25)

add_library(MCU_IMPLEMENTATION
        mcuInit.cpp
)

set(MANUFACTURER ST)
set(MCU_MODEL STM32L476)

set(HAL_DIR_PATH ${CMAKE_SOURCE_DIR}/HAL CACHE PATH "Hal path")
set(MCU_PATH ${CMAKE_SOURCE_DIR}/HAL/core/${MANUFACTURER}/${MCU_MODEL}/halLayerImpl)

add_subdirectory(${HAL_DIR_PATH}/lib)
add_subdirectory(${CMAKE_SOURCE_DIR}/HAL/core)
add_subdirectory(${MCU_PATH}/gpio)
add_subdirectory(${MCU_PATH}/timer)
add_subdirectory(${MCU_PATH}/interrupt)
add_subdirectory(${MCU_PATH}/uart)
add_subdirectory(${MCU_PATH}/i2c)

target_link_libraries(MCU_IMPLEMENTATION PUBLIC 
                        HAL_MCU_LAYER
                        HAL_MODULES
                        MCU_RELATED 
                        MODULES_IMPL
                        GPIO_IMPLEMENTATION
                        TIMER_IMPLEMENTATION 
                        INTERRUPT_IMPLEMENTATION
                        UART_IMPLEMENTATION
                        I2C_IMPLEMENTATION
                        )

target_include_directories(MCU_IMPLEMENTATION 
        PUBLIC  ${MCU_PATH}/../Include
        PUBLIC  ${MCU_PATH}/gpio
        PRIVATE ${MCU_PATH}/timer
        PUBLIC  ${MCU_PATH}/interrupt
        PUBLIC  ${MCU_PATH}/uart
        PUBLIC  ${MCU_PATH}/i2c
        )
