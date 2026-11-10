# Helpers for compile tests: small programs that are compiled at test time to check that they compile, or that they
# fail to compile with a specific error.
#
# Each test case is a .cpp file in cases/, annotated with comments:
#
#   // wuzp-expect: error | compiles
#       Required. Whether the case must fail to compile or compile successfully.
#   // wuzp-requires: <conditions>
#       Optional. The case only runs in configurations matching the conditions; otherwise it is reported as disabled.
#   // wuzp-error[(<conditions>)]: <regex>
#       An error the compiler must report: <regex> must match within a single line that contains "error".
#   // wuzp-static-assert[(<conditions>)]: <regex>
#       A static assertion that must fail, with a message matching <regex>.
#
# Conditions are comma-separated key=value pairs, all of which must hold. A value may list alternatives with "|".
# Keys: units (wuzp, boost, mp_units, archetype), values (eigen, blitz, archetype), compiler (gnu, clang, appleclang, msvc).
#
# A case that must fail needs at least one expected error that applies to the configuration, so that a case can never
# pass because it fails for an unrelated reason. Annotations must not contain ";", since CMake uses it as list separator.

# Sets <out_var> to TRUE if the conditions hold for the configuration, given as the variables WUZP_COMPILE_TEST_UNITS,
# WUZP_COMPILE_TEST_VALUES and WUZP_COMPILE_TEST_COMPILER.
function(wuzp_compile_test_conditions_hold conditions out_var)
    set(_holds TRUE)
    string(REPLACE "," ";" _conditions "${conditions}")
    foreach(_condition IN LISTS _conditions)
        string(STRIP "${_condition}" _condition)
        if(NOT _condition MATCHES "^([a-z]+)=([a-z_|]+)$")
            message(FATAL_ERROR "Invalid condition '${_condition}', expected <key>=<value>[|<value>...].")
        endif()
        set(_key ${CMAKE_MATCH_1})
        set(_values ${CMAKE_MATCH_2})
        if(_key STREQUAL "units")
            set(_actual ${WUZP_COMPILE_TEST_UNITS})
        elseif(_key STREQUAL "values")
            set(_actual ${WUZP_COMPILE_TEST_VALUES})
        elseif(_key STREQUAL "compiler")
            set(_actual ${WUZP_COMPILE_TEST_COMPILER})
        else()
            message(FATAL_ERROR "Unknown condition key '${_key}', expected units, values or compiler.")
        endif()
        if(NOT _actual MATCHES "^(${_values})$")
            set(_holds FALSE)
        endif()
    endforeach()
    set(${out_var} ${_holds} PARENT_SCOPE)
endfunction()

# Parses the annotations of a test case. Sets, in the parent scope:
#   <prefix>_EXPECT    "error" or "compiles"
#   <prefix>_ENABLED   TRUE if the requirements hold for the configuration
#   <prefix>_ERRORS    List of regexes of the expected errors that apply to the configuration
function(wuzp_compile_test_parse file prefix)
    file(READ "${file}" _content)
    if(_content MATCHES "// wuzp-[^\n]*;")
        message(FATAL_ERROR "${file}: annotations must not contain ';'.")
    endif()
    string(REGEX MATCHALL "// wuzp-[^\n]*" _annotations "${_content}")

    set(_expect "")
    set(_enabled TRUE)
    set(_errors "")
    foreach(_annotation IN LISTS _annotations)
        if(_annotation MATCHES "^// wuzp-expect: *(error|compiles) *$")
            set(_expect ${CMAKE_MATCH_1})
        elseif(_annotation MATCHES "^// wuzp-requires: *(.+)$")
            wuzp_compile_test_conditions_hold("${CMAKE_MATCH_1}" _holds)
            if(NOT _holds)
                set(_enabled FALSE)
            endif()
        elseif(_annotation MATCHES "^// wuzp-(error|static-assert)(\\(([^)]*)\\))?: *(.+)$")
            set(_kind ${CMAKE_MATCH_1})
            set(_conditions "${CMAKE_MATCH_3}")
            set(_regex "${CMAKE_MATCH_4}")
            set(_holds TRUE)
            if(NOT _conditions STREQUAL "")
                wuzp_compile_test_conditions_hold("${_conditions}" _holds)
            endif()
            if(_holds)
                if(_kind STREQUAL "static-assert")
                    # GCC: "static assertion failed: <message>", Clang: "static assertion failed due to requirement
                    # '...': <message>" or "static_assert failed ...", MSVC: "static_assert failed: '<message>'".
                    set(_regex "static.assert[^\n]*failed[^\n]*${_regex}")
                endif()
                list(APPEND _errors "${_regex}")
            endif()
        else()
            message(FATAL_ERROR "${file}: invalid annotation '${_annotation}'.")
        endif()
    endforeach()

    if(_expect STREQUAL "")
        message(FATAL_ERROR "${file}: missing annotation '// wuzp-expect: error' or '// wuzp-expect: compiles'.")
    endif()
    if(_expect STREQUAL "compiles" AND NOT _errors STREQUAL "")
        message(FATAL_ERROR "${file}: a case that compiles cannot expect errors.")
    endif()

    set(${prefix}_EXPECT ${_expect} PARENT_SCOPE)
    set(${prefix}_ENABLED ${_enabled} PARENT_SCOPE)
    set(${prefix}_ERRORS "${_errors}" PARENT_SCOPE)
endfunction()
