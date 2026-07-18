#include <aleph/aleph_sql.hpp>
#include <iostream>
#include <string>
#include <system_error>
#include <utility>

void do_register(aleph::sqlite3_ptr db , const char* register_name) {
  if (register_name == nullptr) {
    PRINT_WARNING_LOGGER("Invalid username");
    return;
  }
  db->async_execute_sqlite("INSERT INTO test (name, age) VALUES (?, ?)", 
    std::move([&register_name](const std::error_code& ec , 
      const aleph::sqlite_execute_result&& result,
        const std::string& error_message){
          if (ec) {
            if (result.sqlite3_execute_error_code == SQL_UNIQUE_CONSTRAINT_FAILED) {
              PRINT_WARNING_LOGGER("User already exists cannot register");
            } else {
              PRINT_ERROR_LOGGER(ec.message().c_str());
              return;
            }
          } else {
            std::cout << "Register success change '" 
                << result.sqlite3_affected_row 
                  << " line" << std::endl;
            return;
          }
        }), register_name , 16);
      for (int index = 0; index < 6; index ++) {
        PRINT_INFO_LOGGER("Registering ......");
      }
  return;
}

void do_query(aleph::sqlite3_ptr db , const char* query_name , const char* register_name) {
  db->async_execute_sqlite_select("SELECT id , name , age FROM test WHERE name = ?", 
    std::move([db = std::move(db) , register_name = std::move(register_name)](const std::error_code& ec, 
      const aleph::sqlite_execute_result&& result ,
       const std::string& error_message){
        if (ec) {
          PRINT_ERROR_LOGGER((ec.message() + "|" + result.sqlite3_execute_error).c_str());
          return;
        }
        if (result.sqlite3_result_row.empty()) {
          PRINT_INFO_LOGGER("Information empty");
        } else {
          for (auto&& result_row : result.sqlite3_result_row) {
            std::cout << "Query success : \n UserID: " 
              << result_row[0] 
              << "\n UserName: " << result_row[1]
              << "\n UserAge: " << result_row[2]
              << std::endl;
          }
        }
        do_register(db, register_name);
        return;
      }), query_name);
    for (int index = 0; index < 6; index ++) {
      PRINT_INFO_LOGGER("Querying ......");
    }
  return;
}

int main (int argc , char* argv[]) {
  aleph::thread::thread_manager& new_thread = aleph::thread::thread_manager::get_instance();
  new_thread.init_thread(3);
  aleph::sqlite3_ptr new_sqlite3_db = aleph::sql_database::create_sql<aleph::sql_type::SQLITE>();
    new_sqlite3_db->async_open_sqlite_database("test.db" , std::move(
      [new_sqlite3_db = std::move(new_sqlite3_db) , argv](const std::error_code& ec , const std::string& error_message){
        if (ec) {
          PRINT_ERROR_LOGGER(ec.message().c_str());
          return;
        }
        PRINT_INFO_LOGGER("Open success!");
        do_query(std::move(new_sqlite3_db), argv[1] , argv[2]);
        return;
      }
  ));
}