#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

// Note: The double expansion is required to turn the value of the compiler definition into a string.
#define _TO_STRING(ARG) #ARG
#define TO_STRING(ARG) _TO_STRING(ARG)

inline std::string getCMakeListsContent() {
  return "cmake_minimum_required(VERSION 3.12)\n"
         "project(test_compilation)\n"
         "\n"
         "set(CMAKE_CXX_STANDARD 20)\n"
         "\n"
         "add_compile_options(-Wall -Wextra -Wpedantic)\n"
         "\n"
         "add_compile_definitions(UNITS_IMPLEMENTATION=" TO_STRING(UNITS_IMPLEMENTATION) ")\n"
         "add_compile_definitions(UNITS_IMPLEMENTATION_WUZP=" TO_STRING(UNITS_IMPLEMENTATION_WUZP) ")\n"
         "add_compile_definitions(UNITS_IMPLEMENTATION_BOOST=" TO_STRING(UNITS_IMPLEMENTATION_BOOST) ")\n"
         "add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION=" TO_STRING(VALUE_MATRIX_IMPLEMENTATION) ")\n"
         "add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION_WUZP=" TO_STRING(VALUE_MATRIX_IMPLEMENTATION_WUZP) ")\n"
         "add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION_EIGEN=" TO_STRING(VALUE_MATRIX_IMPLEMENTATION_EIGEN) ")\n"
         "\n"
         "add_subdirectory(" TO_STRING(REPO_DIR) "/wuzp wuzp_build)\n"
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
         "add_subdirectory(" TO_STRING(REPO_DIR) "/wuzp_boost wuzp_boost_build)\n"
#endif
#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
         "add_subdirectory(" TO_STRING(REPO_DIR) "/wuzp_eigen wuzp_eigen_build)\n"
#endif
         "\n"
         "add_executable(tests test.hpp.cpp)\n"
         "target_link_libraries(tests PRIVATE wuzp)\n"
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
         "target_link_libraries(tests PRIVATE wuzp_boost)\n"
#endif
#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
         "target_link_libraries(tests PRIVATE wuzp_eigen)\n"
#endif
      ;
}

inline void createFile(const std::string& name, const std::string& content) {
  std::ofstream file;
  file.open(name);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file.");
  }
  file << content;
  file.close();
}

inline std::pair<int, std::string> exec(const char* cmd) {
  std::array<char, 128> buffer{};
  std::string stdOut;
  FILE* pipe = popen(cmd, "r");
  if (pipe == nullptr) {
    throw std::runtime_error("popen() failed.");
  }
  while (feof(pipe) == 0) {
    if (fgets(buffer.data(), 128, pipe) != nullptr) {
      stdOut += buffer.data();
    }
  }
  const int ret = pclose(pipe);
  return {ret, stdOut};
}

/**
 * Compile code.
 * @param code Code to compile.
 * @return Pair of boolean indicating success and a string containing the standard output.
 */
inline std::pair<bool, std::string> compile(const std::string& code) {
  const std::string directory = std::string{"test_compilation_"} + std::to_string(std::hash<std::string>()(code));
  std::filesystem::create_directories(directory);
  createFile(directory + "/CMakeLists.txt", getCMakeListsContent());
  createFile(directory + "/test.hpp.cpp", code);
  const auto result = exec((std::string{"cd "} + directory +
                            " && cmake -D CMAKE_C_COMPILER=" TO_STRING(CMAKE_C_COMPILER) " -D CMAKE_CXX_COMPILER=" TO_STRING(
                                CMAKE_CXX_COMPILER) " . && make 2>&1")
                               .c_str());
  return {result.first == EXIT_SUCCESS, result.second};
}

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
