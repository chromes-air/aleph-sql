/**
  Copyright [chrmoes-air] [0901]

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#ifndef ALPEH_SQL_LOGGER
#define ALPEH_SQL_LOGGER

#include <iostream>
#include <string>

namespace aleph {
  class logger {
    public:
    enum class logger_level {
      ERROR, INFO,
      WARNING, FATAL
    };
    typedef typename logger::logger_level M_logger_levelT;
      void looger_print(M_logger_levelT logger_level , const char* logger_msg , 
        const std::string& logger_file , int logger_line , 
        const char* logger_function) noexcept {
          std::cerr << "[" 
            << M_level_to_str(logger_level) 
            << "] "
            << logger_file
            << " - "
            << logger_line
            << " -> "
            << "'" 
            << logger_msg 
            << "' -> " 
            << logger_function 
            << "'"
            << std::endl;
          return;
        }

    private:

    const char* M_level_to_str(M_logger_levelT logger_level) 
    noexcept {
      switch (logger_level) {
        case logger_level::ERROR: {
          return "ERROR";
        };
        case logger_level::INFO: {
          return "INFO";
        };
        case logger_level::WARNING: {
          return "WARNING";
        };
        case logger_level::FATAL: {
          return "FATAL";
        };
        default:
          return "INFO";
      }
    }
  };
}
#ifdef ALPEH_SQL_LOGGER_DISABLE
  #define PRINT_ERROR_LOGGER(msg) ((void)0)
  #define PRINT_INFO_LOGGER(msg) ((void)0)
  #define PRINT_WARNING_LOGGER(msg) ((void)0)
  #define PRINT_FATAL_LOGGER(msg) ((void)0)
#else
#define PRINT_ERROR_LOGGER(msg)\
  aleph::logger().looger_print(\
    aleph::logger::logger_level::ERROR , msg , __FILE__ , \
      __LINE__ , __FUNCTION__\
  )
#define PRINT_INFO_LOGGER(msg)\
  aleph::logger().looger_print(\
    aleph::logger::logger_level::INFO , msg , __FILE__ , \
      __LINE__ , __FUNCTION__\
  )
#define PRINT_WARNING_LOGGER(msg)\
  aleph::logger().looger_print(\
    aleph::logger::logger_level::WARNING , msg , __FILE__ , \
      __LINE__ , __FUNCTION__\
  )
#define PRINT_FATAL_LOGGER(msg)\
  aleph::logger().looger_print(\
    aleph::logger::logger_level::FATAL , msg , __FILE__ , \
      __LINE__ , __FUNCTION__\
  )
#endif
#endif