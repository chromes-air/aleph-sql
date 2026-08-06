#ifndef ALEPH_SQL_SQLDATABASE_MYSQL_EXECUTE
#define ALEPH_SQL_SQLDATABASE_MYSQL_EXECUTE

#include <mysql/mysql.h>
#include "aleph/util/logger.hpp"

namespace aleph {
    namespace mysql_impl {
        class mysql_execute_command {
            public:

            mysql_execute_command() = default;

                template<typename ...Args>
                bool command_execute(
                    const char* mysql_command,
                    MYSQL* mysql_handler,
                    Args&& ... paramter
                ) {
                    //Verification paramter
                    if (mysql_command == nullptr || mysql_handler == nullptr) {
                        PRINT_ERROR_LOGGER("Invalid command or mysql_handler!");
                        return false;
                    }

                    //Create new stmt
                    MYSQL_STMT* new_mysql_stmt = mysql_stmt_init(mysql_handler);
                     if (new_mysql_stmt == nullptr) {
                        //Initlized failed
                        PRINT_ERROR_LOGGER("Initlized stmt failed!");
                        return false;
                     }
                     
                     if (complite_command(mysql_command, new_mysql_stmt)) {
                        
                     }
                    return false;
                }

            private:

            bool complite_command(const char* command_code , MYSQL_STMT* mysql_stmt);
            bool command_execute_impl ();

        };
    }
}

#endif
