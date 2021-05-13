#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

inline std::string getCMakeListsContent() {
  return "cmake_minimum_required(VERSION 3.10.2)\n"
         "project(test_compilation)\n"
         "\n"
         "set(CMAKE_CXX_STANDARD 17)\n"
         "\n"
         "find_package(Boost 1.56 REQUIRED)\n"
         "find_package(Eigen3 3.3 REQUIRED NO_MODULE)\n"
         "\n"
         "add_executable(test test.cpp)\n"
         "target_include_directories(test PRIVATE ../../include ${Boost_INCLUDE_DIRS})\n"
         "target_link_libraries(test Eigen3::Eigen)\n";
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

#define ASSERT_NO_COMPILATION_ERROR(code) ASSERT_TRUE(compile(code).first)
#define ASSERT_COMPILATION_ERROR(code) ASSERT_FALSE(compile(code).first)
#define ASSERT_STATIC_ASSERTION_FAILURE(code) \
  const auto result = compile(code);  \
  ASSERT_FALSE(result.first);         \
  ASSERT_NE(result.second.find("static assertion failed"), std::string::npos)\
