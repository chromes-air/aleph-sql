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
#ifndef ALEPH_SQL
#define ALEPH_SQL

#include "aleph/sql/sql_database.hpp"
#include "aleph/sql/sql_error_code.hpp"
#include "aleph/sql/sql_type.hpp"
#include "aleph/thread/sql_thread.hpp"
#include "aleph/thread/thread_manager.hpp"
#include "aleph/sql/impl/sql_database_mysql.hpp"
#include "aleph/sql/impl/sql_database_sqlite.hpp"
#include "aleph/util/async_error_code.hpp"
#include "aleph/util/function_trais.hpp"
#include "aleph/util/sqltype_trais.hpp"
#include "aleph/util/loggeer.hpp"


#endif