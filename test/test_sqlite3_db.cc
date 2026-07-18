#include <aleph/aleph_sql.hpp>
#include <iostream>
#include <ostream>

int main(int argc, char* argv[]) {
  aleph::sqlite3_ptr new_sqlite3 = aleph::sql_database::create_sql<aleph::sql_type::SQLITE>();
  new_sqlite3->open_sqlite_database("test.db");
    aleph::sqlite_execute_result sqlite_add_result = 
      new_sqlite3->execute_sqlite(
        "INSERT INTO test (name, age) VALUES (?, ?)",
          argv[1] , argv[2]);
        if (sqlite_add_result.sqlite3_execute_status) {
          std::cerr << "Insert success name:'" << argv[1] << "' age :" 
            << argv[2] << " change : " << sqlite_add_result.sqlite3_affected_row 
              << " line" << std::endl;
        } else {
          if (sqlite_add_result.sqlite3_execute_error_code == SQL_UNIQUE_CONSTRAINT_FAILED) {
            std::cerr << "Insert name '" << argv[1] << "' failed already exists" << std::endl;
          
          } else {
            std::cerr << "Update failed error: " 
              << sqlite_add_result.sqlite3_execute_error << std::endl;
            return -1;
          }
        }
      aleph::sqlite_execute_result sqlite_query_result = 
        new_sqlite3->execute_sqlite_select(
        "SELECT id , name , age FROM test WHERE name = ?"
        , argv[1] );
        if (sqlite_query_result.sqlite3_execute_error.empty() || 
            sqlite_query_result.sqlite3_execute_status) {
          if (sqlite_query_result.sqlite3_result_row.empty()) {
            std::cout << "Not found '" << argv[1] << "' " << std::endl;
            return -1;
          }
          for (const auto& row : sqlite_query_result.sqlite3_result_row) {
            std::cout << "Query success id: " << row[0]  << " / name: " << row[1]  << "/ age: " << row[2] << std::endl;
          }
          return 0;
        } else {
          std::cout << "Error: " << sqlite_query_result.sqlite3_execute_error << std::endl;
          return -1;
        }
    return 0;
}