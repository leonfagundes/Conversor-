add_executable(conversor_tests
    core_tests.cpp
)

target_link_libraries(conversor_tests
    PRIVATE
        conversor_core
)

add_test(NAME conversor_core_tests COMMAND conversor_tests)
