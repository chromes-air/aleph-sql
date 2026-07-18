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
#ifndef ALEPH_SQL_SQLDATABASE_SQLITE
#define ALEPH_SQL_SQLDATABASE_SQLITE

#ifdef ALEPH_SQLDATABASE_WIN32
#include <locale>
#include <codecvt>
#endif

#include <functional>
#include <system_error>
#include <tuple>
#include <sqlite3.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include "aleph/sql/sql_error_code.hpp"
#include "aleph/util/function_trais.hpp"
#include "aleph/util/async_error_code.hpp"
#include "aleph/thread/thread_manager.hpp"

namespace aleph {
  namespace sql_impl {
    class sql_database_sqlite3 : 
      public std::enable_shared_from_this<aleph::sql_impl::sql_database_sqlite3> {

      public:
      ~sql_database_sqlite3() {
        //Delete sqlite stmt pointer
        sqlite3_finalize(M_sqlite_stmt);
        //Close sqlite3 database 
        sqlite3_close(M_sqlite_ptr);
      }
      private:
      typedef typename aleph::sql_impl::sql_database_sqlite3 sql_database_sqlite3_implT;
      typedef typename std::vector<std::vector<std::string>> sqlite_result;
      public:
      sql_database_sqlite3& operator=(sql_database_sqlite3_implT& sqlite3_impl_copy) = delete;
      sql_database_sqlite3& operator=(sql_database_sqlite3_implT&& sqlite3_impl_move) {
        if (&sqlite3_impl_move != this) {
          //Move
          M_sqlite_ptr = std::move(sqlite3_impl_move.M_sqlite_ptr);
          M_sqlite_stmt = std::move(sqlite3_impl_move.M_sqlite_stmt);
          //Rest pointer
          sqlite3_impl_move.M_sqlite_stmt = nullptr;
          sqlite3_impl_move.M_sqlite_ptr = nullptr;
        }
        return *this;
      }

      sql_database_sqlite3(sql_database_sqlite3_implT& sqlite3_impl_copy) = delete;
      sql_database_sqlite3(sql_database_sqlite3_implT&& sqlite3_impl_move)
        :M_sqlite_ptr(sqlite3_impl_move.M_sqlite_ptr) ,
          M_sqlite_stmt(sqlite3_impl_move.M_sqlite_stmt)
            {}
      
      struct sqlite3_execute_result {
        bool sqlite3_execute_status = false;
        std::string sqlite3_execute_error;
        sqlite_result sqlite3_result_row;
        int sqlite3_affected_row = 0;
        int sqlite3_execute_error_code = SQL_SUCCESS;
 
        sqlite3_execute_result() = default;
        ~sqlite3_execute_result() = default;
      };

      sql_database_sqlite3() {
        sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        M_sqlite_ptr = nullptr; //Init sqlite_ptr`
        M_sqlite_stmt = nullptr; //Init sqlite_stmt`
      }
        /**
          Sync open sqlite3 database
          @param sqlite_database_name | Database files name
        */
          bool open_sqlite_database(
            const std::string& sqlite3_database_name
          ) noexcept;
        /**
          Sync execute select sqlite3 SQL code
          @param sqlite3_sql_code | Database code
          @param args | Patamter inforamtions
        */
        template<typename ...Args>
          struct sqlite3_execute_result execute_sqlite_select(
            const std::string& sqlite3_sql_code , 
            Args ...args
          ) noexcept {
            sqlite3_execute_result sql_execute_result_struct;
              if (!M_sqlite_ptr) {
                //Not open sqlite3 database file
                sql_execute_result_struct.sqlite3_execute_error = "Not open sqlite3 database file!";
                sql_execute_result_struct.sqlite3_execute_status = false;
                return sql_execute_result_struct;
              }
              //Compile sqlite3 code
              int complie_sql_ret = sqlite3_prepare_v2(M_sqlite_ptr, sqlite3_sql_code.c_str(),
                -1, &M_sqlite_stmt, nullptr);
                if (complie_sql_ret != SQLITE_OK) {
                  //Compile sqlite3 code failed
                  sql_execute_result_struct.sqlite3_execute_error_code = M_get_sql_error_code(complie_sql_ret);
                  sql_execute_result_struct.sqlite3_execute_status = false;
                  std::string sqlite3_complie_error_message = std::string(sqlite3_errmsg(M_sqlite_ptr));
                  sql_execute_result_struct.sqlite3_execute_error = sqlite3_complie_error_message;
                  return sql_execute_result_struct;
                }
                //Try bind sqlite3 code paramter
                int parmater_index = 1;
                (M_bind_args_paramter(M_sqlite_stmt, parmater_index++,
                  std::forward<Args>(args)), ...);
                  //Try execute sqlite3 code
                  int column_count = sqlite3_column_count(M_sqlite_stmt); 
                  int execute_ret;
                  while ((execute_ret = sqlite3_step(M_sqlite_stmt)) == SQLITE_ROW) {
                    std::vector<std::string> sqlite3_execute_result_row;
                    sqlite3_execute_result_row.reserve(column_count);
                    for (int index = 0; index < column_count; index ++) {
                      const char* column_text = reinterpret_cast<const char*>(
                        sqlite3_column_text(M_sqlite_stmt, index)
                      );
                      sqlite3_execute_result_row.push_back(column_text ? column_text : "NULL");
                    }
                    sql_execute_result_struct.sqlite3_result_row.push_back(sqlite3_execute_result_row);
                  }
                sql_execute_result_struct.sqlite3_execute_status = true;
              return sql_execute_result_struct;
          }

        /**
          Sync execute not select sqlite3 SQL code
          @param sqlite3_sql_code | Database code
          @param args | Patamter inforamtions
        */
        template<typename ...Args>
          struct sqlite3_execute_result execute_sqlite(
            const std::string& sqlite3_sql_code,
            Args&& ...args
          ) noexcept {
            sqlite3_execute_result sql_execute_result_struct;
              if (!M_sqlite_ptr) {
                //Not open sqlite3 database file
                sql_execute_result_struct.sqlite3_execute_error = "Not open sqlite3 database file!";
                sql_execute_result_struct.sqlite3_execute_status = false;
                return sql_execute_result_struct;
              }
                int complie_sql_ret = sqlite3_prepare_v2(M_sqlite_ptr, sqlite3_sql_code.c_str(),
                 -1, &M_sqlite_stmt, nullptr);
                  if (complie_sql_ret != SQLITE_OK) {
                    //Compile sqlite3 code failed
                    sql_execute_result_struct.sqlite3_execute_status = false;
                    std::string sqlite3_complie_error_message = std::string(sqlite3_errmsg(M_sqlite_ptr));
                    sql_execute_result_struct.sqlite3_execute_error = sqlite3_complie_error_message;
                    return sql_execute_result_struct;
                  }
                //Try bind sqlite3 code paramter
                int parmater_index = 1;
                (M_bind_args_paramter(M_sqlite_stmt, 
                  parmater_index ++, std::forward<Args>(args)),...);
                  //Try execute sqlite3 code
                  int execute_ret = sqlite3_step(M_sqlite_stmt);
                    if (execute_ret == SQLITE_DONE) {
                      //Execture success
                      sql_execute_result_struct.sqlite3_execute_status = true;
                      sql_execute_result_struct.sqlite3_affected_row = sqlite3_changes(M_sqlite_ptr);
                      return sql_execute_result_struct;
                    }
                    else {
                      sql_execute_result_struct.sqlite3_execute_error_code = M_get_sql_error_code(execute_ret);
                      sql_execute_result_struct.sqlite3_execute_error = "Exectue sqlite3 code failed! Error: " + 
                      std::string(sqlite3_errmsg(M_sqlite_ptr));
                      sql_execute_result_struct.sqlite3_execute_status = false;
                      return sql_execute_result_struct;
                    }
            sql_execute_result_struct.sqlite3_execute_error = "Cannot open sqlite3 database file";
            sql_execute_result_struct.sqlite3_execute_error_code = M_get_sql_error_code(SQLITE_CANTOPEN);
            sql_execute_result_struct.sqlite3_execute_status = false;
            return sql_execute_result_struct;
          }

        /**
          Async open sqlite3 database files
          @param sqlite_database_name | Database files name
          @param token | Callback function
        */
        template<typename complite_callback>
          auto async_open_sqlite_database(
            const std::string& sqlite_database_name,
            complite_callback&& token
          ) noexcept(
            noexcept(
              token(
                std::declval<const std::error_code&>(),
                std::declval<const std::string&>()
              )
            )
          ) -> 
              typename std::enable_if <
                aleph::function_trais::aleph_comparator_function_ret<
                  typename aleph::function_trais::aleph_function_ret_type<
                    complite_callback, 
                      const std::error_code&, 
                        const std::string&
                      >::type, 
                  void
                >::result
              ::value, 
              void
            >::type
           {
            M_thread_manager.get_thread()
              ->post_new_task([self = shared_from_this(),
                token = std::function<void(const std::error_code& , const std::string&)>(std::move(token)),
                  sqlite_database_name = std::move(sqlite_database_name)
              ]() mutable {
                //Call sync function
                bool it_open_success = self -> open_sqlite_database(sqlite_database_name);
                  if (it_open_success) {
                    //Open success
                    std::error_code ec;
                    self -> M_thread_manager.get_thread()
                      ->post_new_task(std::move(token), 
                        ec , "Open database success"
                    );
                    return; 
                  }
                //Open failed
                std::error_code ec = aleph::error_code::make_error_code(SQL_CANTOPEN);
                  //Get error message
                  const std::string error_message = std::string(sqlite3_errmsg(self -> M_sqlite_ptr));
                  self -> M_thread_manager.get_thread()
                    ->post_new_task(std::move(token), 
                      ec , error_message
                  );
                return;
              }
            );
          }

        /**
          Async execute sqlite3
          @param sqlite3_sql_code | Sqlite3 code
          @param token | Callback function
          @param paramter | Query paramter
        */
        template<typename complite_callback , typename ...Args>
        auto async_execute_sqlite_select(
          const std::string& sqlite3_sql_code,
          complite_callback&& token,
          Args&& ... paramter
        ) noexcept(
            noexcept(
              token(
                std::declval<const std::error_code&>(),
                std::declval<struct sqlite3_execute_result&&>(),
                std::declval<const std::string&>()
              )
            )
        ) -> typename std::enable_if<
          aleph::function_trais::aleph_comparator_function_ret<
            typename aleph::function_trais::aleph_function_ret_type<
              complite_callback, 
              const std::error_code& , 
                struct sqlite3_execute_result&& ,
                const std::string&  
            >::type, void
            >::result
          ::value,
          void
        >::type {
          M_thread_manager.get_thread()
            ->post_new_task(std::move([
              self = shared_from_this(),
              token = std::function<void(const std::error_code& , 
                struct sqlite3_execute_result&& , 
                  const std::string&)
                >(std::move(token)),
                sqlite3_sql_code = std::move(sqlite3_sql_code),
              paramter = std::make_tuple(std::forward<Args>(paramter) ...)
            ]() mutable {
              struct sqlite3_execute_result sql_execute_result_struct;
                std::apply([&](auto&& ... unpackage_paramter){
                  sql_execute_result_struct =  self -> execute_sqlite_select(
                    sqlite3_sql_code,
                    std::forward<
                      decltype(unpackage_paramter)
                      >(unpackage_paramter) ...
                  );
                  //Verify result
                  if (sql_execute_result_struct.sqlite3_execute_status) {
                    //Execute success
                    std::error_code ec;
                    self -> M_thread_manager.get_thread()
                      ->post_new_task(std::move(token) , ec , 
                        std::move(sql_execute_result_struct) ,
                         std::string("Execute OK")
                      );
                    return;
                  }
                  //Execute failed
                  //Get error code
                  std::error_code ec = aleph::error_code::make_error_code(sql_execute_result_struct.sqlite3_execute_error_code);
                    self -> M_thread_manager.get_thread()
                      ->post_new_task(std::move(token), ec ,
                        std::move(sql_execute_result_struct) , 
                          sql_execute_result_struct.sqlite3_execute_error
                      );
                  return;
                }, 
                std::move(paramter));
            })
          );
          return;
        }

        /**
          Async execute sqlite3
          @param sqlite3_sql_code | Sqlite3 code
          @param token | Callback function
          @param paramter | Query paramter
        */
        template<typename complite_callback , typename ...Args>
        auto async_execute_sqlite(
          const std::string& sqlite3_sql_code,
          complite_callback&& token,
          Args&& ...paramter
        ) noexcept(
            noexcept(
              token(
                std::declval<const std::error_code&>(),
                std::declval<struct sqlite3_execute_result&&>(),
                std::declval<const std::string&>()
              )
            )
        ) -> 
        typename std::enable_if<
          aleph::function_trais::aleph_comparator_function_ret<
            typename aleph::function_trais::aleph_function_ret_type<
              complite_callback, const std::error_code& , 
                struct sqlite3_execute_result&& , 
                  const std::string&
              >::type, void
            >::result
          ::value, 
          void
        >::type {
          M_thread_manager.get_thread()
            ->post_new_task(
              std::move([
                self = shared_from_this(),
                token = std::function<void(
                  const std::error_code& ,
                    struct sqlite3_execute_result&& ,
                    const std::string&
                )>(std::move(token)),
                sqlite3_sql_code = std::move(sqlite3_sql_code),
                paramter = std::make_tuple(std::forward
                  <Args>(paramter) ...
                )
              ]() mutable {
                struct sqlite3_execute_result sql_execute_result_struct;
                std::apply(std::move([&](auto&& ...unpackage_paramter){
                  sql_execute_result_struct = self -> execute_sqlite(sqlite3_sql_code, 
                    std::forward<decltype(unpackage_paramter)>
                      (unpackage_paramter) ...
                    );
                    //Verify result
                    if (sql_execute_result_struct.sqlite3_execute_status) {
                      //Execute success
                      std::error_code ec;
                      self -> M_thread_manager.get_thread()
                        ->post_new_task(std::move(token), ec , 
                        std::move(sql_execute_result_struct) , 
                        std::string("Execute success")
                      );
                      return;
                    }
                    //Exectute failed
                    //Create new error code
                    std::error_code ec = aleph::error_code::make_error_code(sql_execute_result_struct.sqlite3_execute_error_code);
                      self -> M_thread_manager.get_thread()
                        ->post_new_task(std::move(token), ec ,
                         std::move(sql_execute_result_struct) ,
                          std::string(sql_execute_result_struct.sqlite3_execute_error)
                        );
                    return;
                }),
                 std::move(paramter)
                );
              })
            );
        }

      private:
      
      sqlite3* M_sqlite_ptr;
      sqlite3_stmt* M_sqlite_stmt;
      aleph::thread::thread_manager& M_thread_manager = thread::thread_manager::get_instance();

      int M_get_sql_error_code(int sqlite3_ret) {
        switch (sqlite3_ret) {
          case SQLITE_OK: {return SQL_SUCCESS;}
          case SQLITE_ERROR: {return SQL_SYNTAX_ERROR;}
          case SQLITE_BUSY: {return SQL_DATABASE_LOCKED;}
          case SQLITE_LOCKED: {return SQL_TABLE_LOCKED;}
          case SQLITE_IOERR: {return SQL_IO_ERROR;}
          case SQLITE_CORRUPT: {return SQL_CORRUPT;}
          case SQLITE_NOTFOUND: {return SQL_NOTFOUNT;};
          case SQLITE_MISMATCH: {return SQL_MISMATCH;};
          case SQLITE_CANTOPEN: {return SQL_CANTOPEN;};
          case SQLITE_NOTADB: {return SQL_NOTADB;};
          case SQLITE_RANGE: {return SQL_RANGE;};
          case SQLITE_FULL: {return SQL_MEMORY_FULL;};
          case SQLITE_CONSTRAINT: {return SQL_UNIQUE_CONSTRAINT_FAILED;};
          default: {
            return SQL_SUCCESS;
          }
        };
      }

      template<typename paramterT>
      void M_bind_args_paramter(
        sqlite3_stmt* sqlite_stmt_ptr ,
        int idx , 
        paramterT&& paramter_value
      ) noexcept {
        using raw_type = std::decay_t<paramterT>;
          if constexpr (std::is_same_v<raw_type, int>) {
            //Parmter is int
            sqlite3_bind_int(sqlite_stmt_ptr, idx, paramter_value);
          } else if constexpr (std::is_same_v<raw_type, uint64_t>) {
            //Paramter is uint64
            sqlite3_bind_int64(sqlite_stmt_ptr, idx, paramter_value);
          } else if constexpr (std::is_same_v<raw_type, double>) {
            //Paramter is double
            sqlite3_bind_double(sqlite_stmt_ptr, idx, paramter_value);
          } else if constexpr (std::is_same_v<raw_type, std::string>) {
            //Paramter is Cpp char
            sqlite3_bind_text(sqlite_stmt_ptr, idx, paramter_value.c_str(), -1, SQLITE_STATIC);
          } else if constexpr (std::is_same_v<raw_type, const char*>) {
            //Paramter is const C char
            sqlite3_bind_text(sqlite_stmt_ptr, idx, paramter_value, -1, SQLITE_STATIC);
          } else if constexpr (std::is_same_v<raw_type, char*>) {
            sqlite3_bind_text(sqlite_stmt_ptr, idx, paramter_value, -1, SQLITE_STATIC);
          } else if constexpr (std::is_same_v<raw_type, std::nullptr_t>) {
            sqlite3_bind_null(sqlite_stmt_ptr, idx);
          }  else {
            static_assert(false, "Unsupported type for SQLite binding!");
          }
        return;
      }

      #ifdef ALEPH_SQLDATABASE_WIN32
        const std::wstring M_utf8_to_utf16(const std::string& utf8_str)
        noexcept {
          if (utf8_str.empty()) {
            PRINT_ERROR_LOGGER("Invalid utf8 char!");
            return nullptr;
          }
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          return converter.from_bytes(utf8_str);
        }
      #endif
    };
  }
  typedef typename aleph::sql_impl::sql_database_sqlite3::sqlite3_execute_result sqlite_execute_result;
}

#endif