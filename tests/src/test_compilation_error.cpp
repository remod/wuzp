#include "test_compilation_error.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <stdexcept>

#include "compile_test_config.hpp"

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

namespace {

// Quotes an argument for the POSIX shell.
std::string quote(const std::string& argument) {
  std::string quoted{"'"};
  for (const char c : argument) {
    if (c == '\'') {
      quoted += "'\\''";
    } else {
      quoted += c;
    }
  }
  return quoted + "'";
}

std::string readFile(const std::string& name) {
  std::ifstream file{name};
  if (!file.is_open()) {
    throw std::runtime_error{"Failed to open file '" + name + "'."};
  }
  return {std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

}  // namespace

std::pair<bool, std::string> compile(const std::string& code) {
  using namespace compile_test_config;  // NOLINT
  const std::string directory{std::string{"test_compilation_"} + std::to_string(std::hash<std::string>{}(code))};
  const std::string buildDirectory{directory + "/build"};
  std::filesystem::create_directories(directory);
  createFile(directory + "/CMakeLists.txt", readFile(kCMakeListsFile));
  createFile(directory + "/test.cpp", code);

  std::string configure{quote(kCMakeCommand) + " -S " + quote(directory) + " -B " + quote(buildDirectory) + " -G " + quote(kGenerator) +
                        " -D CMAKE_CXX_COMPILER=" + quote(kCxxCompiler)};
  if (std::string{kMakeProgram}.length() > 0) {
    configure += " -D CMAKE_MAKE_PROGRAM=" + quote(kMakeProgram);
  }
  if (std::string{kToolchainFile}.length() > 0) {
    configure += " -D CMAKE_TOOLCHAIN_FILE=" + quote(kToolchainFile);
  }
  const std::string build{quote(kCMakeCommand) + " --build " + quote(buildDirectory)};
  const auto result{exec((configure + " 2>&1 && " + build + " 2>&1").c_str())};
  return {result.first == EXIT_SUCCESS, result.second};
}
