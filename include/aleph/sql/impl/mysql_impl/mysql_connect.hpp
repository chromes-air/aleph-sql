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

#ifndef ALEPH_SQL_SQLDATABASE_MYSQL_CONNECT
#define ALEPH_SQL_SQLDATABASE_MYSQL_CONNECT

#include <chrono>
#include <cstdint>
#include <exception>
#include <memory>
#include <mysql/mysql.h>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include "aleph/util/loggeer.hpp"

namespace aleph {
  namespace sql_impl {
    namespace mysql {
      class mysql_connect {
        public:

          ~mysql_connect() {
              if (M_mysql_handler) {
                  mysql_close(M_mysql_handler);
                  M_mysql_handler = nullptr;
              }
              M_connectd = false;
              M_is_configured = false;
          }

          mysql_connect(
            const std::string& auth_mysql_hostname,
            const std::string& auth_mysql_password,
            const std::string& auth_mysql_username,
            uint16_t auth_mysql_port
          ) noexcept {
            M_connectd = false;
            M_mysql_connect_auth_info.auth_mysql_hostname = std::move(auth_mysql_hostname);
            M_mysql_connect_auth_info.auth_mysql_password = std::move(auth_mysql_password);
            M_mysql_connect_auth_info.auth_mysql_username = std::move(auth_mysql_username);
            M_mysql_connect_auth_info.auth_mysql_port = std::move(auth_mysql_port);
            return;
          }

          void mysql_options_set(
            bool enable_tls ,
            bool enable_custom_creat,
            bool enable_custom_tls_key
          ) noexcept {
            M_mysql_connect_option.is_enable_tls = enable_tls;
            M_mysql_connect_option.is_enable_custom_creat = enable_custom_creat;
            M_mysql_connect_option.is_enable_custom_key = enable_custom_tls_key;
            return;
          }

          void set_tls_custom_creat(const std::string& creat_path) noexcept {
            M_mysql_connect_option.custom_creat_path = creat_path;
            return;
          }

          void set_tls_custom_key(const std::string& custom_key) {
            M_mysql_connect_option.custom_key = custom_key;
            return;
          }

          bool is_connectd() const {
            return M_connectd && M_mysql_handler != nullptr;
          }

          void options_configure() {
            if (M_is_configured) {
              PRINT_WARNING_LOGGER("Configure faild already configure");
            }
            //Init MySQL pointer
            M_mysql_handler = mysql_init(nullptr);
            if (!M_mysql_handler) {
              //Init MySQL pointer failed
              PRINT_ERROR_LOGGER("Initiazed error 'mysql_init()' failed");
              return;
            }
            //Configure CA tls options
            if (M_mysql_connect_option.is_enable_tls) {
              if (M_mysql_connect_option.is_enable_custom_creat) {
                //Load tls CA reat
                mysql_options(M_mysql_handler , MYSQL_OPT_SSL_CA , M_mysql_connect_option.custom_creat_path.c_str());
              }
              if (M_mysql_connect_option.is_enable_custom_key) {
                mysql_options(M_mysql_handler, MYSQL_OPT_SSL_KEY, M_mysql_connect_auth_info.auth_mysql_password.c_str());
              }
            }
          }

          bool mysql_connect_to_service_retry(
            const char* mysql_database_name,
            unsigned long client_flags,
            unsigned connect_timeout,
            int mysql_max_retry_count
          ) noexcept {
            //Setting timeout
            mysql_options(M_mysql_handler, MYSQL_OPT_CONNECT_TIMEOUT , &connect_timeout);
            //Try connect
            if (is_connectd()) {
              //Connected
              return true;
            }
            if (M_mysql_handler != nullptr && M_connectd) {
              mysql_close(M_mysql_handler);
              M_mysql_handler = nullptr;
              M_connectd = false;
            }
            //Connect
            try {
              MYSQL* mysql_connect_result = mysql_real_connect(
                M_mysql_handler, M_mysql_connect_auth_info.auth_mysql_hostname.c_str(),
                 M_mysql_connect_auth_info.auth_mysql_username.c_str(), 
                 M_mysql_connect_auth_info.auth_mysql_password.c_str(),
                  mysql_database_name, 
                  M_mysql_connect_auth_info.auth_mysql_port,
                  nullptr, 
                  client_flags
                );
                if (mysql_connect_result) {
                  //Connect success
                  return true;
                }
              //Connect failed retry
              int retry_count = 0; std::random_device rd;
              std::mt19937 gen(rd());
                for (int attempt = 0; attempt <= mysql_max_retry_count; ++ attempt) {
                  retry_count ++; //Update retry count
                    if (attempt > 0) {
                      int delay_ms = 100*(1 << (attempt - 1));
                      std::uniform_real_distribution<> dist(delay_ms / 2 , delay_ms * 3 / 2);
                      int jitered_delay = dist(gen);
                      std::this_thread::sleep_for(std::chrono::milliseconds(jitered_delay));
                    }
                    //Rtry connect
                    MYSQL* mysql_retry_connect_result = mysql_real_connect(
                      M_mysql_handler, M_mysql_connect_auth_info.auth_mysql_hostname.c_str(),
                      M_mysql_connect_auth_info.auth_mysql_username.c_str(), 
                      M_mysql_connect_auth_info.auth_mysql_password.c_str(),
                        mysql_database_name, 
                        M_mysql_connect_auth_info.auth_mysql_port,
                        nullptr, 
                        client_flags
                      );
                      if (mysql_retry_connect_result) {
                        //Retry success
                        return true;
                      }
                      std::string error_message = "Retry failed: " + std::string(mysql_error(M_mysql_handler));
                      PRINT_WARNING_LOGGER(std::move(error_message.c_str()));
                    //Retry continue
                    continue;
                }
            } catch (std::exception& mysql_connect_error) {
              PRINT_ERROR_LOGGER(mysql_connect_error.what());
              return false;
            }
            return false;
          }

          bool mysql_connect_to_service(
            const char* mysql_database_name,
            unsigned long client_flags = 0
          ) noexcept {
            if (is_connectd()) {
              //Connected
              return true;
            }

            if (M_mysql_handler != nullptr && M_connectd) {
              mysql_close(M_mysql_handler);
              M_mysql_handler = nullptr;
              M_connectd = false;
            }

            //Not conected

            try {
              MYSQL* mysql_connect_result = mysql_real_connect(
                M_mysql_handler, M_mysql_connect_auth_info.auth_mysql_hostname.c_str(),
                 M_mysql_connect_auth_info.auth_mysql_username.c_str(), 
                 M_mysql_connect_auth_info.auth_mysql_password.c_str(),
                  mysql_database_name, 
                  M_mysql_connect_auth_info.auth_mysql_port,
                  nullptr, 
                  client_flags
                );

                if (!mysql_connect_result) {
                  //Connect failed
                  PRINT_ERROR_LOGGER(mysql_error(M_mysql_handler));
                  return false;
                }
              //Connect success
              return true;

            } catch (std::exception& mysql_connect_error) {
              PRINT_ERROR_LOGGER(mysql_connect_error.what());
              return false;
            }
           return false;
          }

        private:

        struct mysql_connect_service_configure {
          bool is_enable_custom_creat = false;
          bool is_enable_custom_key = false;
          bool is_enable_tls = false;

          std::string custom_creat_path;
          std::string custom_key;

          mysql_connect_service_configure() = default;
          ~mysql_connect_service_configure() = default;
        };

        struct mysql_auth_data {
          std::string auth_mysql_password;
          std::string auth_mysql_username;
          std::string auth_mysql_hostname;
          uint16_t auth_mysql_port = 3306; //Default mysql port
          
          mysql_auth_data() = default;
          ~mysql_auth_data() = default;

          mysql_auth_data(mysql_auth_data&) = delete;
          mysql_auth_data& operator=(mysql_auth_data&) = delete;
          mysql_auth_data(mysql_auth_data&& mysql_auth_data_move) noexcept
            : auth_mysql_port(std::move(mysql_auth_data_move.auth_mysql_port)),
              auth_mysql_password(std::move(mysql_auth_data_move.auth_mysql_password)),
              auth_mysql_hostname(std::move(mysql_auth_data_move.auth_mysql_hostname)) 
              {};
            mysql_auth_data& operator=(mysql_auth_data&& mysql_auth_data_move) noexcept
             {
              if (&mysql_auth_data_move != this) {
                auth_mysql_hostname = std::move(mysql_auth_data_move.auth_mysql_hostname);
                auth_mysql_password = std::move(mysql_auth_data_move.auth_mysql_password);
                auth_mysql_username = std::move(mysql_auth_data_move.auth_mysql_username);
                auth_mysql_port = std::move(mysql_auth_data_move.auth_mysql_port);
              }
              return *this;
            }
        };
        
        MYSQL* M_mysql_handler = nullptr;
        bool M_connectd;
        bool M_is_configured;

        typedef typename aleph::sql_impl::mysql::mysql_connect::mysql_auth_data mysql_auth_inforamtionT;
          mysql_auth_inforamtionT M_mysql_connect_auth_info;
          typedef typename aleph::sql_impl::mysql::mysql_connect::mysql_connect_service_configure mysql_connect_config;
            mysql_connect_config M_mysql_connect_option;
      };


      class mysql_connect_pool {

        public:

        typedef typename aleph::sql_impl::mysql::mysql_connect connect_meta;
          static mysql_connect_pool& get_instance() {
            static mysql_connect_pool this_instance;
            return this_instance;
          }

          void push_new_connect(const std::shared_ptr<mysql_connect>& new_connect) noexcept {
            M_conenctions.push(new_connect);
            return;
          }

        private:
          std::queue<std::shared_ptr<connect_meta>> M_conenctions;
      };
    }
  }
}

#endif