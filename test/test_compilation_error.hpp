#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
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
         "add_compile_definitions(MATRIX_IMPLEMENTATION=" TO_STRING(MATRIX_IMPLEMENTATION) ")\n"
         "add_compile_definitions(MATRIX_IMPLEMENTATION_WUZP=" TO_STRING(MATRIX_IMPLEMENTATION_WUZP) ")\n"
         "add_compile_definitions(MATRIX_IMPLEMENTATION_EIGEN=" TO_STRING(MATRIX_IMPLEMENTATION_EIGEN) ")\n"
         "\n"
         "add_executable(tests test.cpp)\n"
         "target_include_directories(tests PRIVATE " TO_STRING(CMAKE_CURRENT_SOURCE_DIR) "/include)\n"
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
         "\n"
         "find_package(Boost 1.56 REQUIRED)\n"
         "target_include_directories(tests PRIVATE ${Boost_INCLUDE_DIRS})\n"
#endif
#if MATRIX_IMPLEMENTATION == MATRIX_IMPLEMENTATION_EIGEN
         "\n"
         "find_package(Eigen3 3.3 REQUIRED NO_MODULE)\n"
         "target_link_libraries(tests Eigen3::Eigen)\n"
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
  std::filesystem::create_directories("test_compilation");
  createFile("test_compilation/CMakeLists.txt", getCMakeListsContent());
  createFile("test_compilation/test.cpp", code);
  const auto result = exec("cd test_compilation && cmake . && make 2>&1");
  return {result.first == EXIT_SUCCESS, result.second};
}

#define ASSERT_NO_COMPILATION_ERROR(code)         \
  const auto result = compile(std::string{code}); \
  ASSERT_TRUE(result.first) << result.second;
#define ASSERT_COMPILATION_ERROR(code)            \
  const auto result = compile(std::string{code}); \
  ASSERT_FALSE(result.first) << result.second;
#define ASSERT_COMPILATION_ERROR_WITH_MESSAGE(code, message)                 \
  const auto result = compile(std::string{code});                            \
  ASSERT_FALSE(result.first) << result.second;                               \
  const auto searchString = std::string{"error: "} + std::string{(message)}; \
  ASSERT_NE(result.second.find(searchString), std::string::npos) << result.second;
#define ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, message)     \
  std::string staticAssertionMessage{"static assertion failed"};        \
  if (!std::string{message}.empty()) {                                  \
    staticAssertionMessage += std::string{": "} + std::string{message}; \
  }                                                                     \
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(code, staticAssertionMessage)
#define ASSERT_STATIC_ASSERTION_FAILURE(code) ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, std::string{})
