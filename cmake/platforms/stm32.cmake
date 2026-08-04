include_guard(GLOBAL)

function(sdk_configure_stm32_platform mcu platform_define)
    target_compile_definitions(platform_flags INTERFACE ${platform_define})
    target_compile_options(platform_flags INTERFACE
        -mcpu=${mcu}
        -mthumb
        -ffunction-sections
        -fdata-sections
        $<$<COMPILE_LANGUAGE:CXX>:-fcoroutines>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    )
    target_link_options(platform_flags INTERFACE
        -mcpu=${mcu}
        -mthumb
        -Wl,--gc-sections
    )
    target_link_libraries(platform_flags INTERFACE c m)
endfunction()
