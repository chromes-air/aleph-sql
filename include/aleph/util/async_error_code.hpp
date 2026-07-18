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
#ifndef ALEPH_SQL_ASYNC_ERROR_CODE
#define ALEPH_SQL_ASYNC_ERROR_CODE

#include <system_error>
#include "aleph/sql/sql_error_code.hpp"

namespace aleph {
  namespace error_code {
    class aleph_sql_error_category : public std::error_category {
      const char * name() const noexcept override {
        return "sql_error";
      }
      std::string message(int ev) const override {
        switch (ev) {
          case SQL_SUCCESS: {
            return "Sqlite3 OK";
          }
          case SQL_UNIQUE_CONSTRAINT_FAILED: {
            return "The same resource already exists";
          }
          case SQL_SYNTAX_ERROR: {
            return "There's a mistake in the SQL syntax";
          }
          case SQL_MOMEM: {
            return "SQL memory is full";
          }
          case SQL_MEMORY_FULL: {
            return "Not enough disk space";
          }
          case SQL_IO_ERROR: {
            return "Database write/output error";
          }
          case SQL_TOOBIG: {
            return "Content is too large";
          }
          case SQL_DATABASE_LOCKED: {
            return "The database has been locked";
          }
          case SQL_CANTOPEN: {
            return "Can't open this database";
          }
          case SQL_NOTFOUNT: {
            return "Database not found";
          }
          case SQL_NOTADB: {
            return "Non-database resources";
          }
          case SQL_SCHEMA: {
            return "Database resources are corrupted";
          }
          case SQL_RANGE: {
            return "Exceeds the legal range of the database";
          }
          default:
            return "Not found sql error";
        };
      }
    };

    inline const aleph_sql_error_category& get_sql_error_category() noexcept {
      static aleph_sql_error_category instance_this;
      return instance_this;
    }

    inline std::error_code make_error_code(int ev) noexcept {
      return std::error_code(ev , get_sql_error_category());
    }
    
  }
}

#endif