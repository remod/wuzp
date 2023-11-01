#include "test_compilation_error.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

// Note: The double expansion is required to turn the value of the compiler definition into a string.
#define _TO_STRING(ARG) #ARG
#define TO_STRING(ARG) _TO_STRING(ARG)

std::string getCMakeListsContent() {
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
         "add_compile_definitions(VALUES_IMPLEMENTATION=" TO_STRING(VALUES_IMPLEMENTATION) ")\n"
         "add_compile_definitions(VALUES_IMPLEMENTATION_WUZP=" TO_STRING(VALUES_IMPLEMENTATION_WUZP) ")\n"
         "add_compile_definitions(VALUES_IMPLEMENTATION_EIGEN=" TO_STRING(VALUES_IMPLEMENTATION_EIGEN) ")\n"
         "add_compile_definitions(VALUES_IMPLEMENTATION_BLITZ=" TO_STRING(VALUES_IMPLEMENTATION_BLITZ) ")\n"
         "\n"
         "add_subdirectory(" TO_STRING(REPO_DIR) "/wuzp wuzp_build)\n"
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
         "add_subdirectory(" TO_STRING(REPO_DIR) "/wuzp_boost wuzp_boost_build)\n"
#endif
#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
         "add_subdirectory(" TO_STRING(REPO_DIR) "/wuzp_eigen wuzp_eigen_build)\n"
#elif VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_BLITZ
         "add_subdirectory(" TO_STRING(REPO_DIR) "/wuzp_blitz wuzp_blitz_build)\n"
#endif
         "\n"
         "add_executable(tests test.cpp)\n"
         "target_include_directories(tests PRIVATE " TO_STRING(REPO_DIR) "/tests/src)\n"
         "target_link_libraries(tests PRIVATE wuzp)\n"
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
         "target_link_libraries(tests PRIVATE wuzp_boost)\n"
#endif
#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
         "target_link_libraries(tests PRIVATE wuzp_eigen)\n"
#elif VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_BLITZ
         "target_link_libraries(tests PRIVATE wuzp_blitz)\n"
#endif
      ;
}

void createFile(const std::string& name, const std::string& content) {
  std::ofstream file;
  file.open(name);
  if (!file.is_open()) {
    throw std::runtime_error{"Failed to open file."};
  }
  file << content;
  file.close();
}

std::pair<int, std::string> exec(const char* cmd) {
  std::array<char, 128> buffer{};
  std::string stdOut;
  FILE* pipe{popen(cmd, "r")};
  if (pipe == nullptr) {
    throw std::runtime_error{"popen() failed."};
  }
  while (feof(pipe) == 0) {
    if (fgets(buffer.data(), 128, pipe) != nullptr) {
      stdOut += buffer.data();
    }
  }
  const int ret{pclose(pipe)};
  return {ret, stdOut};
}

std::pair<bool, std::string> compile(const std::string& code) {
  const std::string directory{std::string{"test_compilation_"} + std::to_string(std::hash<std::string>{}(code))};
  std::filesystem::create_directories(directory);
  createFile(directory + "/CMakeLists.txt", getCMakeListsContent());
  createFile(directory + "/test.cpp", code);
  const auto result{exec((std::string{"cd "} + directory +
                          " && cmake -D CMAKE_C_COMPILER=" TO_STRING(CMAKE_C_COMPILER) " -D CMAKE_CXX_COMPILER=" TO_STRING(
                              CMAKE_CXX_COMPILER) " . && make 2>&1")
                             .c_str())};
  return {result.first == EXIT_SUCCESS, result.second};
}
