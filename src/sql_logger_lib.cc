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

#include "aleph/util/logger.hpp"
#include <exception>
#include <string>

void aleph::logger::outopt_logger_impl(
  const std::string& logger_message,
  const char* logger_level,
  const std::string& logger_local_file,
  const std::string& logger_local_function,
  int logger_local_line_number
) noexcept {
  try {
    //Sefa outopt
    std::cout << "[" 
        << logger_level
        << "]" 
        << 
        " - "
        <<
        logger_message
        <<
        " - "
        <<
        " LOCAL <"
        <<
        logger_local_file 
        << "|"
        << logger_local_line_number
        << "|"
        << logger_local_function
        << ">"
        <<
        std::endl;  
  } catch (const std::exception& outopt_exception) {
    std::cerr << "Outpot failed: " << outopt_exception.what() << std::endl;
    return;
  }
}