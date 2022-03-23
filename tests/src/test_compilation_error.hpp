#include <regex>
#include <string>

#include <gtest/gtest.h>

/**
 * Compile code.
 * @param code Code to compile.
 * @return Pair of boolean indicating success and a string containing the standard output.
 */
std::pair<bool, std::string> compile(const std::string& code);

#define ASSERT_NO_COMPILATION_ERROR(code)         \
  const auto result = compile(std::string{code}); \
  const auto compiles = result.first;             \
  ASSERT_TRUE(compiles) << "Failed to compile: " + result.second;
#define ASSERT_COMPILATION_ERROR_WITH_MESSAGE(code, message)                                       \
  const auto result = compile(std::string{code});                                                  \
  const auto compiles = result.first;                                                              \
  ASSERT_FALSE(compiles) << "Expected to fail compilation, but it succeeded: " + result.second;    \
  if (!std::string{message}.empty()) {                                                             \
    const auto searchString = std::string{"error: "} + std::string{message};                       \
    ASSERT_TRUE(std::regex_search(result.second, std::regex(searchString)))                        \
        << "Failed to find error message regex '" + searchString + "' in '" + result.second + "'"; \
  }
#define ASSERT_COMPILATION_ERROR(code) ASSERT_COMPILATION_ERROR_WITH_MESSAGE(code, std::string{})
#define ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, message)     \
  /* Use regex to match the error messages of different compilers. */   \
  std::string staticAssertionMessage{"static.assert.+failed"};          \
  if (!std::string{message}.empty()) {                                  \
    staticAssertionMessage += std::string{".+"} + std::string{message}; \
  }                                                                     \
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(code, staticAssertionMessage)
#define ASSERT_STATIC_ASSERTION_FAILURE(code) ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, std::string{})
