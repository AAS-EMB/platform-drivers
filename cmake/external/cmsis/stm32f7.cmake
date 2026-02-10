add_library(stm32f7_cmsis INTERFACE)

target_include_directories(stm32f7_cmsis INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cmsis/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cmsis-device-f7/Include
)

target_compile_definitions(stm32f7_cmsis INTERFACE
    STM32F7
    STM32F767xx
    USE_HAL_DRIVER
)

target_link_libraries(stm32f7_cmsis INTERFACE
    platform_flags
)