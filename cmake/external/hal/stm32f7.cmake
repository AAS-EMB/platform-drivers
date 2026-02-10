file(GLOB stm32f7_hal_SOURCES
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_gpio_ex.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_tim.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_tim_ex.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_rcc.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_rcc_ex.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_gpio.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_dma.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_cortex.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_pwr.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_flash.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_flash_ex.c
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Src/stm32f7xx_hal_exti.c
)

add_library(stm32f7_hal STATIC
    ${stm32f7_hal_SOURCES}
)

target_include_directories(stm32f7_hal PUBLIC
    ${CMAKE_SOURCE_DIR}/external/stm32f7_hal/Inc
    ${CMAKE_SOURCE_DIR}/templates/stm32f7
)

target_link_libraries(stm32f7_hal PUBLIC
    stm32f7_cmsis
    platform_flags
)
