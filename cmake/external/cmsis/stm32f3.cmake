add_library(stm32f3_cmsis INTERFACE)

target_include_directories(stm32f3_cmsis INTERFACE
    ${CMAKE_SOURCE_DIR}/external/cmsis/Core/Include
    ${CMAKE_SOURCE_DIR}/external/cmsis-device-f3/Include
)

target_compile_definitions(stm32f3_cmsis INTERFACE
    STM32F3
    STM32F303xC
    USE_HAL_DRIVER
)

target_link_libraries(stm32f3_cmsis INTERFACE
    platform_flags
)