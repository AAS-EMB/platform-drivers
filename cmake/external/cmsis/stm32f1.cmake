add_library(stm32f1_cmsis INTERFACE)

target_include_directories(stm32f1_cmsis INTERFACE
    ${CMAKE_SOURCE_DIR}/external/cmsis/Core/Include
    ${CMAKE_SOURCE_DIR}/external/cmsis-device-f1/Include
)

target_compile_definitions(stm32f1_cmsis INTERFACE
    STM32F1    
    STM32F100xB
    USE_HAL_DRIVER
)

target_link_libraries(stm32f1_cmsis INTERFACE
    platform_flags
)
