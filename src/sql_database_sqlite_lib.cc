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
#include "../include/aleph/sql/impl/sql_database_sqlite.hpp"
#include "../include/aleph/util/loggeer.hpp"
#include "../include/aleph/sql/sql_database.hpp"
#include <string>

bool aleph::sql_impl::sql_database_sqlite3::open_sqlite_database 
  (const std::string& sqlite3_database_name) noexcept {
    if (sqlite3_database_name.empty()) {
      PRINT_ERROR_LOGGER("Invlaid database file name!");
      return false;
    }
    #if defined(ALEPH_SQLDATABASE_LINUX) || defined (ALEPH_SQLDATABASE_MACOS)
      int open_sqlite_database_ret = sqlite3_open_v2(sqlite3_database_name.c_str(), &M_sqlite_ptr ,
       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, nullptr);
        #elif defined(ALEPH_SQLDATABASE_WIN32)
          std::wstring sqlite3_database_name_wstring = M_utf8_to_utf16(sqlite3_database_name);
            int open_sqlite_database_ret = sqlite3_open_v2(sqlite3_database_name.c_str(), &M_sqlite_ptr ,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, nullptr);
    #endif
    if (open_sqlite_database_ret != SQLITE_OK) {
      //Open sqlite3 database failed
      std::string sqlite3_open_error_message = std::string(sqlite3_errmsg(M_sqlite_ptr));
      std::string logger_message = 
      "Open sqlite3 database '" + 
      sqlite3_database_name + "' failed ERROR: " + 
      sqlite3_open_error_message;
      return false;
    }
    //Open sqlite3 database success
    return true;
  }