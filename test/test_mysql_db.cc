#include <aleph/aleph_sql.hpp>
#include <aleph/sql/impl/sql_database_mysql.hpp>
#include <aleph/sql/sql_type.hpp>
#include <aleph/util/logger.hpp>

int main () {
  aleph::mysql_ptr mysqlptr = aleph::sql_database::create_sql<aleph::sql_type::MYSQL>();
    aleph::mysql_connect_option new_options;
    new_options.mysql_enable_retry = false;
    new_options.mysql_connect_tiemout = 10;
    bool is_connect = mysqlptr->mysql_connect_database("localhost", 
      "O20100829o",
       "root", 3306, 
       "test", 
       new_options,
        0
      );
       if (is_connect) {
        PRINT_INFO_LOGGER("Connect success");
        return 0;
       }
      PRINT_INFO_LOGGER("Connect failed");
    return -1;
}