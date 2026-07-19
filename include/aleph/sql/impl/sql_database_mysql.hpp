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

#ifndef ALEPH_SQL_SQLDATABASE_MYSQL
#define ALEPH_SQL_SQLDATABASE_MYSQL

#include <cstdint>
#include <memory>
#include <string>
#include "aleph/sql/impl/mysql_impl/mysql_connect.hpp"


namespace aleph {
  namespace sql_impl {
    class sql_database_mysql : 
      public std::enable_shared_from_this<sql_database_mysql> {
        
        private:
        typedef typename aleph::sql_impl::mysql::mysql_connect mysql_connectionT;
          typedef typename aleph::sql_impl::mysql::mysql_connect_pool mysql_connect_pool;
        public:

        struct mysql_option {
          unsigned mysql_connect_tiemout = 5; //Default timeout
          unsigned mysql_write_timeout = 30; //Default timeout
          unsigned mysql_read_timeout = 30; //Default timeout
          
          int mysql_max_retry_count = 10; //Default max retry
          bool mysql_enable_retry = true; //Default enable retry
          bool mysql_enable_tls = false; //Default not enbale
          bool mysql_enable_custom_creat = false; //Default not custom
          bool mysql_enable_custom_tls_key = false; //Default not custom
          
          std::string mysql_CA_creat_path;
          std::string mysql_tls_key;
        };

        /**
          Sync connect MySQL service
          @param mysql_service_hostname | Database service hostname
          @param mysql_service_password | Database service manager password
          @param mysql_username | Databse service manmager username
        */
        bool mysql_connect_database(
          const std::string& mysql_service_hostname,
          const std::string& mysql_service_password,
          const std::string& mysql_username,
          uint16_t mysql_service_port,
          const std::string& mysql_database_name,
          struct mysql_option mysql_service_options,
          unsigned long client_flgas
        ) noexcept {
          M_mysql_connect_manager = std::make_shared<mysql_connectionT>(
              mysql_service_hostname ,
              mysql_service_password , 
              mysql_username , 
              mysql_service_port
            );

            //Configure connect options
            if (mysql_service_options.mysql_enable_tls) {
              if (mysql_service_options.mysql_enable_custom_creat) {
                M_mysql_connect_manager->mysql_options_set(mysql_service_options.mysql_enable_tls,
                  true, mysql_service_options.mysql_enable_custom_tls_key);
              }
              if (mysql_service_options.mysql_enable_custom_tls_key) {
                 M_mysql_connect_manager->mysql_options_set(mysql_service_options.mysql_enable_tls,
                  true, mysql_service_options.mysql_enable_custom_tls_key);
              }
            }
            
            M_mysql_connect_manager->options_configure();
            
            if (mysql_service_options.mysql_enable_retry) {
              //Call with retry connect
              bool is_connect_result = M_mysql_connect_manager->mysql_connect_to_service_retry(mysql_database_name.c_str(), 
                client_flgas, mysql_service_options.mysql_connect_tiemout,
                  mysql_service_options.mysql_max_retry_count
                );
                if (is_connect_result) {
                  //Connect success
                  return true;
                } else {
                  //Connect failed
                  return false;
                }
              return false;
            }
            //Call with basic connect not retry
            bool is_connect_result = M_mysql_connect_manager->mysql_connect_to_service(
              mysql_database_name.c_str() , client_flgas
            );
              if (is_connect_result) {
                //Connect success
                //Push to connect pool
                mysql_connect_pool::get_instance();

                return true;
              }
            //Connect failed
          return false;
        }

        private:

        std::shared_ptr<aleph::sql_impl::mysql::mysql_connect> M_mysql_connect_manager;
    };
  }
}

#endif
