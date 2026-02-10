add_library(stm32f1_cmsis INTERFACE)

message(${CMAKE_CURRENT_SOURCE_DIR}/external/cmsis/Core/Include)

target_include_directories(stm32f1_cmsis INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cmsis/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cmsis-device-f1/Include
)

target_compile_definitions(stm32f1_cmsis INTERFACE
    STM32F1    
    STM32F100xB
    USE_HAL_DRIVER
)

target_link_libraries(stm32f1_cmsis INTERFACE
    platform_flags
)
