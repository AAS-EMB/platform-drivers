include("${CMAKE_CURRENT_LIST_DIR}/stm32.cmake")
sdk_configure_stm32_platform(cortex-m4 PLATFORM_STM32F3)

sdk_require_file("external/cmsis/Core/Include/core_cm4.h")
sdk_require_file("external/cmsis-device-f3/Include/stm32f303xc.h")
sdk_require_file("external/stm32f3_hal/Inc/stm32f3xx_hal.h")
include("${CMAKE_CURRENT_LIST_DIR}/../external/cmsis/stm32f3.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../external/hal/stm32f3.cmake")
target_link_libraries(platform INTERFACE stm32f3_hal)
