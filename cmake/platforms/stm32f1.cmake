include("${CMAKE_CURRENT_LIST_DIR}/stm32.cmake")
sdk_configure_stm32_platform(cortex-m3 PLATFORM_STM32F1)

sdk_require_file("external/cmsis/Core/Include/core_cm3.h")
sdk_require_file("external/cmsis-device-f1/Include/stm32f100xb.h")
sdk_require_file("external/stm32f1_hal/Inc/stm32f1xx_hal.h")
include("${CMAKE_CURRENT_LIST_DIR}/../external/cmsis/stm32f1.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../external/hal/stm32f1.cmake")
target_link_libraries(platform INTERFACE stm32f1_hal)
