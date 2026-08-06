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

#include "../include/aleph/sql/impl/mysql_impl/mysql_execute.hpp"
#include <cstring>
#include <mysql/mysql.h>
 
bool aleph::mysql_impl::mysql_execute_command::complite_command(
    const char* command_code,
    MYSQL_STMT* mysql_stmt
) {
    if (mysql_stmt == nullptr) {
        PRINT_ERROR_LOGGER("Invliad mysql_stmt!");
        return false;
    }
     int complite_result = mysql_stmt_prepare(
        mysql_stmt, command_code, 
        strlen(command_code));
        if (complite_result == 0) {
            //Complite success
            return true;
        }
    //Complite failed
    const char* complite_command_error = mysql_stmt_error(mysql_stmt);
    PRINT_ERROR_LOGGER(complite_command_error);
    return false;
}