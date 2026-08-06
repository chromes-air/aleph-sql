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

#ifndef ALEPH_SQL_LOGGER
#define ALEPH_SQL_LOGGER

#include <iostream>
#include <string>

namespace aleph {

    enum class LOGGER_LEVEL {
        ERROR,
        INFO,
        WARNING
    };

    class logger {
        public:

            logger() = default;
            ~logger() = default;
            
            const char* level_to_str(
                aleph::LOGGER_LEVEL logger_level
            ) {
                switch (logger_level) {
                    case LOGGER_LEVEL::INFO: {
                        return "INFO";
                    }
                    case aleph::LOGGER_LEVEL::ERROR: {
                        return "ERROR";
                    }
                    case aleph::LOGGER_LEVEL::WARNING: {
                        return "WARNING";
                    }
                    default: {return "DEFAULT-OUTOPT";}
                }
            }

            /**
                @param logger_message // Outpot message
                @param logger_level // Outpot level
                @param logger_local_file //Outpot lcoal file name
                @param logger_local_function // Outpot function name
                @param logger_local_line_number //Outpot local line number
                @note This method noexcept
            */
            void outopt_logger(
                const std::string& logger_message,
                aleph::LOGGER_LEVEL logger_level,
                const std::string& logger_local_file,
                const std::string& logger_local_function,
                int logger_local_line_number
            ) noexcept {
                //Verification paramter

                bool is_use_defaule_meesage = false;
                const std::string default_message = "NEW LOGGER";

                if (logger_message.empty()) {
                    //Outopt default message
                    is_use_defaule_meesage = true;
                }

                const char* logger_level_str = level_to_str(logger_level);
                    if (logger_level_str == nullptr) {
                        std::cerr << "Logger level case to cstring failed!" << std::endl;
                        return;
                    }
                    
                    //Call impl function
                    if (is_use_defaule_meesage) {
                        //Use default message
                        outopt_logger_impl(default_message, 
                        logger_level_str,
                             logger_local_file, 
                             logger_local_function,
                              logger_local_line_number
                            );
                        return;
                    }

                    //Use paramter logger message
                    outopt_logger_impl(logger_message, 
                        logger_level_str,
                            logger_local_file, 
                            logger_local_function, 
                            logger_local_line_number
                        );
                    return;
            }

         
        private:

        void outopt_logger_impl(
            const std::string& logger_message,
            const char* logger_level,
            const std::string& logger_local_file,
            const std::string& logger_local_function,
            int logger_local_line_number
        ) noexcept;

    };
}//Aleph end

#ifndef DISABLE_ALEPH_LOGGER_OUTPOT
    //Outopt info logger
    #define PRINT_INFO_LOGGER(__M_outpot_msg__)\
        aleph::logger().outopt_logger(\
            __M_outpot_msg__,\
            aleph::LOGGER_LEVEL::INFO,\
            __FILE__,\
            __FUNCTION__,\
            __LINE__\
        )
    //Outopt error logger
    #define PRINT_ERROR_LOGGER(__M_outpot_msg__)\
        aleph::logger().outopt_logger(\
            __M_outpot_msg__,\
            aleph::LOGGER_LEVEL::ERROR,\
            __FILE__,\
            __FUNCTION__,\
            __LINE__\
        )
    //Outopt warning logger
    #define PRINT_WARNING_LOGGER(__M_outpot_msg__)\
        aleph::logger().outopt_logger(\
            __M_outpot_msg__,\
            aleph::LOGGER_LEVEL::WARNING,\
            __FILE__,\
            __FUNCTION__,\
            __LINE__\
        )
#else
#define PRINT_INFO_LOGGER(__M_outpot_msg__) ((void)0)
#define PRINT_ERROR_LOGGER(__M_outpot_msg__) ((void)0)
#define PRINT_WARNING_LOGGER(__M_outpot_msg__) ((void)0)
#endif

#endif