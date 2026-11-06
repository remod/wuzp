# Compiler warnings for wuzp's own targets (tests, examples, benchmarks, compile tests). Third-party headers are
# included as system headers, so only warnings in wuzp's code are reported.

option(WUZP_WARNINGS_AS_ERRORS "Treat compiler warnings in the tests, examples and benchmarks as errors." OFF)

# The warning options, as a list.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WUZP_WARNING_OPTIONS /W4 /permissive-)
    if(WUZP_WARNINGS_AS_ERRORS)
        list(APPEND WUZP_WARNING_OPTIONS /WX)
    endif()
else()
    set(WUZP_WARNING_OPTIONS -Wall -Wextra -Wpedantic)
    if(WUZP_WARNINGS_AS_ERRORS)
        list(APPEND WUZP_WARNING_OPTIONS -Werror)
    endif()
endif()

# Stricter warnings that users might enable in their own code. The public headers must not trigger them.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WUZP_STRICT_WARNING_OPTIONS ${WUZP_WARNING_OPTIONS})
else()
    set(WUZP_STRICT_WARNING_OPTIONS ${WUZP_WARNING_OPTIONS} -Wshadow -Wconversion -Wsign-conversion -Wold-style-cast)
endif()

function(wuzp_set_warnings target)
    target_compile_options(${target} PRIVATE ${WUZP_WARNING_OPTIONS})
endfunction()
