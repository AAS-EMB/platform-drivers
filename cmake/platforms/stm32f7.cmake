include("${CMAKE_CURRENT_LIST_DIR}/stm32.cmake")
sdk_configure_stm32_platform(cortex-m7 PLATFORM_STM32F7)

sdk_require_file("external/cmsis/Core/Include/core_cm7.h")
sdk_require_file("external/cmsis-device-f7/Include/stm32f767xx.h")
sdk_require_file("external/stm32f7_hal/Inc/stm32f7xx_hal.h")
include("${CMAKE_CURRENT_LIST_DIR}/../external/cmsis/stm32f7.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../external/hal/stm32f7.cmake")
target_link_libraries(platform INTERFACE stm32f7_hal)
