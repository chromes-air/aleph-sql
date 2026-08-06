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
#ifndef SQL_TYPE_TRAIS
#define SQL_TYPE_TRAIS

#include "aleph/sql/sql_type.hpp"

namespace aleph {
  namespace sql_type_trais {
    template<sql_type sqlT>
    struct is_sqlite_type {
      static constexpr bool is_sqlite_T = (sqlT == sql_type::SQLITE);
    };
    template<sql_type sqlT>
    struct is_mysql_type {
      static constexpr bool is_mysql_T = (sqlT == sql_type::MYSQL);
    };
  }
}

#endif