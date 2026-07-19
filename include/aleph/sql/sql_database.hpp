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
#ifndef ALEPH_SQL_SQLDATABASE
#define ALEPH_SQL_SQLDATABASE

#include <aleph/sql/sql_type.hpp>
#include <memory>
#include <type_traits>
#include "aleph/util/sqltype_trais.hpp"

#ifdef __linux__
  #define ALEPH_SQLDATABASE_LINUX
#elif defined(_WIN32) || defined(_WIN64)
  #define ALEPH_SQLDATABASE_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
  #define ALEPH_SQLDATABASE_MACOS
#else
  #define ALEPH_SQLDATABASE_UNKNOWN
#endif

namespace aleph {
  namespace sql_impl {class sql_database_sqlite3;}
  namespace sql_impl {class sql_database_mysql;}
  class sql_database {
    public:
    template<aleph::sql_type sql_Tp>
      static typename std::enable_if<
        aleph::sql_type_trais::is_slqite_type<
          sql_Tp
        >::is_sqlite_T , std::shared_ptr<
          aleph::sql_impl::sql_database_sqlite3
        >
      >::type create_sql() noexcept {
        typedef typename aleph::sql_impl::sql_database_sqlite3 sqlite3_Tp;
          typedef typename std::shared_ptr<aleph::sql_impl::sql_database_sqlite3> sqlite3_sharedTp;
            sqlite3_sharedTp new_sqlite_ptr = std::make_shared<aleph::sql_impl::sql_database_sqlite3>();
          return new_sqlite_ptr;
      }
    
    template<aleph::sql_type sql_Tp>
      static typename std::enable_if<
        aleph::sql_type_trais::is_mysql_type<
          sql_Tp
        >::is_mysql_T , std::shared_ptr<
          aleph::sql_impl::sql_database_mysql
        >
      >::type create_sql() noexcept {
        typedef typename aleph::sql_impl::sql_database_mysql mysql_Tp;
          typedef typename std::shared_ptr<aleph::sql_impl::sql_database_mysql> mysql_sharedTp;
            mysql_sharedTp new_mysql_ptr = std::make_shared<aleph::sql_impl::sql_database_mysql>();
          return new_mysql_ptr;
      }
    
  };
  
  typedef typename std::shared_ptr<aleph::sql_impl::sql_database_sqlite3> sqlite3_ptr;
    typedef typename std::shared_ptr<aleph::sql_impl::sql_database_mysql> mysql_ptr;
}

#endif