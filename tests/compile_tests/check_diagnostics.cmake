# Checks that every static_assert message in the public headers is verified by a compile test. Usage:
#   cmake -D SOURCE_DIR=<repository> -P check_diagnostics.cmake
#
# A message counts as tested if a case in tests/compile_tests/cases mentions it in a '// wuzp-static-assert' or
# '// wuzp-error' annotation (regex escapes are ignored when comparing).
#
# Messages that are known to be untested are listed in untested_diagnostics.txt, one per line. The check fails if a
# message is untested but not listed, and also if a listed message is tested or no longer exists, so that the list
# only ever shrinks.

cmake_minimum_required(VERSION 3.22)

if(NOT DEFINED SOURCE_DIR)
    message(FATAL_ERROR "SOURCE_DIR is not defined.")
endif()
set(_baseline_file "${CMAKE_CURRENT_LIST_DIR}/untested_diagnostics.txt")

# Collect the distinct static_assert messages of all public headers. A message is the last string literal before the
# closing parenthesis of the static_assert.
file(GLOB_RECURSE _headers "${SOURCE_DIR}/wuzp*/include/*.hpp")
set(_messages "")
foreach(_header IN LISTS _headers)
    file(READ "${_header}" _content)
    string(REGEX MATCHALL "static_assert[ \t\n]*\\([^;]*\"[ \t\n]*\\)" _asserts "${_content}")
    foreach(_assert IN LISTS _asserts)
        if(_assert MATCHES "\"([^\"]*)\"[ \t\n]*\\)$")
            list(APPEND _messages "${CMAKE_MATCH_1}")
        endif()
    endforeach()
endforeach()
list(REMOVE_DUPLICATES _messages)
list(SORT _messages)

# Collect the annotations of all compile tests, without regex escapes.
file(GLOB _cases "${CMAKE_CURRENT_LIST_DIR}/cases/*.cpp")
set(_annotations "")
foreach(_case IN LISTS _cases)
    file(READ "${_case}" _content)
    string(REGEX MATCHALL "// wuzp-(error|static-assert)[^\n]*" _matches "${_content}")
    string(APPEND _annotations "${_matches}\n")
endforeach()
string(REPLACE "\\" "" _annotations "${_annotations}")

set(_untested "")
set(_tested_count 0)
foreach(_message IN LISTS _messages)
    string(FIND "${_annotations}" "${_message}" _position)
    if(_position EQUAL -1)
        list(APPEND _untested "${_message}")
    else()
        math(EXPR _tested_count "${_tested_count} + 1")
    endif()
endforeach()

file(STRINGS "${_baseline_file}" _baseline REGEX "^[^#]")
set(_new_untested "${_untested}")
if(NOT _baseline STREQUAL "")
    list(REMOVE_ITEM _new_untested ${_baseline})
endif()
set(_obsolete "${_baseline}")
if(NOT _untested STREQUAL "")
    list(REMOVE_ITEM _obsolete ${_untested})
endif()

list(LENGTH _messages _total)
list(LENGTH _untested _untested_count)
message(STATUS "${_tested_count} of ${_total} static_assert messages are verified by a compile test, "
    "${_untested_count} are listed as untested in untested_diagnostics.txt.")

set(_failed FALSE)
if(NOT _new_untested STREQUAL "")
    list(JOIN _new_untested "\n  " _text)
    message(SEND_ERROR "These static_assert messages have no compile test. Add one in tests/compile_tests/cases:\n  ${_text}")
    set(_failed TRUE)
endif()
if(NOT _obsolete STREQUAL "")
    list(JOIN _obsolete "\n  " _text)
    message(SEND_ERROR "These messages are listed in untested_diagnostics.txt, but are now tested or no longer exist. "
        "Remove them from the list:\n  ${_text}")
    set(_failed TRUE)
endif()
if(_failed)
    message(FATAL_ERROR "The diagnostics check failed.")
endif()
