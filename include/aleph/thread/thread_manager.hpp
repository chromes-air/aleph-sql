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
#ifndef ALEPH_SQL_THREAD_MANAGER
#define ALEPH_SQL_THREAD_MANAGER

#include "sql_thread.hpp"
#include "aleph/util/logger.hpp"
#include <atomic>
#include <cstddef>
#include <memory>
#include <thread>
#include <utility>

namespace aleph {
  namespace thread {
    class thread_manager {
      public:
      thread_manager() = default;
      ~thread_manager() = default;
      thread_manager& operator=(thread_manager&) = delete;
      
      thread_manager& operator=(thread_manager&& thread_manager_move) {
        if (&thread_manager_move != this) {
          M_sql_thread_ptr = std::move(thread_manager_move.M_sql_thread_ptr);
          M_is_thread_create = std::move(thread_manager_move.M_is_thread_create);
        }
        return *this;
      }

      thread_manager(thread_manager&& thread_manager_move):
        M_is_thread_create(std::move(thread_manager_move.M_is_thread_create)),
          M_sql_thread_ptr(std::move(thread_manager_move.M_sql_thread_ptr)) 
          {}

      static thread_manager& get_instance() {
        static aleph::thread::thread_manager instance_this;
        return instance_this;
      }

      void init_thread(size_t thread_count = std::thread::hardware_concurrency()) {
        if (M_is_thread_create->load()) {
          //Already initialzed
          PRINT_WARNING_LOGGER("Thread manager already initialized");
          return;
        }
        //Initialzed thread
        //Change status
        M_is_thread_create->store(true);
        M_sql_thread_ptr = std::make_shared<aleph::thread::sql_thread>(thread_count);
        return;
      }

      aleph::thread::sql_thread_shared_ptr& get_thread() {
        if (!M_sql_thread_ptr) {
          //Not initialzed thread
          PRINT_ERROR_LOGGER("Thread not initiazed");
          throw; 
        }
        return M_sql_thread_ptr;
      }

      private:
      std::shared_ptr<std::atomic<bool>> M_is_thread_create = std::make_shared<std::atomic<bool>>(false);
      aleph::thread::sql_thread_shared_ptr M_sql_thread_ptr;
    };
  }
}

#endif