//----------------------------------------------------------------------------------------------------------------------
// MIT License
// Copyright (c) 2022 bevanwsjones
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ---------------------------------------------------------------------------------------------------------------------
// File Name: Macros.h
// Description: Defines a number of global functions and macros for the project.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_MACROS_H
#define DISA_MACROS_H

#include <ostream>
#include <source_location>
#include <string>

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Logging and Error handling
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \enum Log_Level
 * \brief Logging Level, used to defines the severity of messages to the console.
 */
enum class Log_Level : uint8_t {
  Error,
  Warning,
  Info,
  Debug,
};//Log_Level

/**
 * \brief Adds additional information to messages about to be printed to screen, such as file and line numbers.
 * \param[in] level Logging level enumerator, determine prefixing of returned string.
 * \param[in] location Source code location object, containing the origin of the console message.
 * \return Formatted string: '[logging level | file name::line number]:'
 */
inline std::basic_string<char> console_format(const Log_Level level, const std::source_location &location) {
  auto const iter = std::basic_string_view<char>(location.file_name()).find_last_of('/');
  const auto suffix = std::basic_string<char>(location.file_name()).substr(iter + 1) + "::"
                      + std::to_string(location.line()) + "]: ";
  switch (level) {
    case Log_Level::Debug:
      return "[Debug|" + std::basic_string(suffix);
    case Log_Level::Info:
      return "[Info |" + std::basic_string(suffix);
    case Log_Level::Warning:
      return "[Warn|" + std::basic_string(suffix);
    case Log_Level::Error:
      return "[Error|" + std::basic_string(suffix);
  }
}//console_format

/**
 * \brief Debug level console write out.
 * \param[in] message The message to print to the output stream.
 * \param[in] location Source code location object, containing the origin of the console message.
 */
inline void debug(const std::string_view message,
                  const std::source_location location = std::source_location::current()) {
  std::cout << "\n" << console_format(Log_Level::Debug, location) << message << std::flush;
}//debug

/**
 * \brief General information level console write out.
 * \param[in] message The message to print to the output stream.
 * \param[in] location Source code location object, containing the origin of the console message.
 */
inline void
info(const std::string_view message, const std::source_location location = std::source_location::current()) {
  std::cout << "\n" << console_format(Log_Level::Info, location) << message << std::flush;
}//info

/**
 * \brief Warning level console write out.
 * \param[in] message The message to print to the output stream.
 * \param[in] location Source code location object, containing the origin of the console message.
 */
inline void warning(const std::string_view message,
                    const std::source_location location = std::source_location::current()) {
  std::cout << "\n" << console_format(Log_Level::Warning, location) << message << std::flush;
}//warning

/**
 * \brief Error level console write out, writes to std::cerr and exits.
 * \param[in] message The message to print to the error stream.
 * \param[in] location Source code location object, containing the origin of the console message.
 */
inline void
error(const std::string_view message, const std::source_location location = std::source_location::current()) {
  std::cerr << "\n" << console_format(Log_Level::Error, location) << message << std::endl;
  exit(1);
}//error

/**
 * \brief Checks if a condition is true, if it is not the program will exit.
 * \param[in] condition The condition to check.
 * \param[in] message The message to print to the error stream in the event of a false condition.
 * \param[in] location Source code location object, containing the origin of the console message.
 */
inline void assert_check(const bool &condition, const std::string_view message,
                         const std::source_location location = std::source_location::current()) {
  if (!condition) error(message, location);
  else return;
}//assert_check

/**
 * \brief Checks if a condition is true in debug build, if it is not the program will exit.
 * \param[in] condition The condition to check.
 * \param[in] message The message to print to the error stream in the event of a false condition.
 * \param[in] location Source code location object, containing the origin of the console message.
 */
inline void assert_check_debug(const bool &condition, const std::string_view message,
                               const std::source_location location = std::source_location::current()) {
#ifdef DISA_DEBUG
  if (!condition) error(message, location);
  else return;
#endif
}//assert_check

// ---------------------------------------------------------------------------------------------------------------------
// Looping Macros
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \brief Traditional integer type for-loop macro, will start at 0, and end at max_iter - 1.
 * \param[in, out] index The for loop index.
 * \param[in] max_iter Maximum number of iterations.
 */
#define FOR(index, max_iter) for(auto (index) = static_cast<decltype(max_iter)>(0); (index) < (max_iter); ++(index))

/**
 * \brief Range based for-loop macro
 * \param[in, out] element Constant reference to the elements in the container.
 * \param[in] container Container to loop over
 */
#define FOR_EACH(element, container) for(const auto& (element) : (container))

/**
 * \brief Range based for-loop macro
 * \param[in, out] element Constant reference to the elements in the container.
 * \param[in] container Container to loop over
 */
#define FOR_EACH_REF(element, container) for(auto& (element) : (container))

}//Disa

#endif //DISA_MACROS_H
