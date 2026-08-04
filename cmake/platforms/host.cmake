find_package(Threads REQUIRED)

target_compile_definitions(platform_flags INTERFACE PLATFORM_HOST)
if(BUILD_TESTS)
    target_compile_definitions(platform_flags INTERFACE PLATFORM_TESTING)
endif()
target_link_libraries(platform_flags INTERFACE Threads::Threads)
