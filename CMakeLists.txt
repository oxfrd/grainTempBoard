cmake_minimum_required(VERSION 3.25)

add_library(MCU_IMPLEMENTATION
        mcuInit.cpp
)
add_subdirectory(HAL)
add_subdirectory(gpio)
add_subdirectory(timer)
add_subdirectory(interrupt)
add_subdirectory(uart)
add_subdirectory(i2c)

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
        PRIVATE ${CMAKE_SOURCE_DIR}/core/ST/${MCU_MODEL}/Include
        PUBLIC  ${CMAKE_SOURCE_DIR}/core/ST/${MCU_MODEL}/halLayer/gpio
        PRIVATE  ${CMAKE_SOURCE_DIR}/core/ST/${MCU_MODEL}/halLayer/timer
        PUBLIC  ${CMAKE_SOURCE_DIR}/core/ST/${MCU_MODEL}/halLayer/interrupt
        PUBLIC  ${CMAKE_SOURCE_DIR}/core/ST/${MCU_MODEL}/halLayer/uart
        PUBLIC  ${CMAKE_SOURCE_DIR}/core/ST/${MCU_MODEL}/halLayer/i2c
        )
