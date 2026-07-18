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
#ifndef ALEPH_SQL_THREAD
#define ALEPH_SQL_THREAD

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <deque>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include "aleph/util/function_trais.hpp"
#include "aleph/util/loggeer.hpp"

namespace aleph {
  namespace thread {
    class sql_thread : public std::enable_shared_from_this<sql_thread> {
      private:

      struct thread_meta {
        std::thread this_thread;
        std::atomic<bool> thread_idle{true};
        uint16_t thred_processed{0};
      };

      typedef typename std::shared_ptr<struct thread_meta> thread_meta_sharedptrT;
        std::deque<std::function<void()>> M_tasks;
        std::vector<thread_meta_sharedptrT> M_worke_threads;
        mutable std::mutex M_task_mutex_;
        mutable std::mutex M_running_count_mutex_;
        mutable std::atomic<int> M_thread_running_count;
        std::atomic<bool> M_stop{true};
        std::condition_variable M_condition_variable;
        int M_thread_timeout = 10; //Defalt paramter

      public:

      sql_thread(sql_thread& ) = delete;
      sql_thread& operator=(sql_thread&) = delete;
      sql_thread& operator=(sql_thread&&) = delete;
      sql_thread(sql_thread&&) = delete;

      template<typename complie_token , typename ... Args>
      typename std::enable_if<
        aleph::function_trais::aleph_comparator_function_ret<
          typename aleph::function_trais::aleph_function_ret_type<
            complie_token, Args ...
          >::type, void
        >::result::value , 
        void
      >::type post_new_task(
        complie_token&& token,
        Args&& ...args
      ) noexcept {
        std::function<void()> new_task = [_task_func = std::move(token) ,
          ..._task_parmater = std::move(args)]() mutable {
            _task_func(std::move(_task_parmater) ...);
          };
        {
          //Add new running count lock
          std::lock_guard<std::mutex> running_count_lock(M_task_mutex_);
          //Update running count
            M_thread_running_count ++;
            //Add new task to tasks lists
            M_tasks.push_back(std::move(new_task));
        }
        //Notify one thread
        M_condition_variable.notify_one();
        return;
      }
      
      ~sql_thread() {
        M_stop.store(true);
        M_condition_variable.notify_all();
        //Wait all thread task done
        typedef typename std::vector<std::shared_ptr<struct thread_meta>> thread_meta_vecotr_tp;
          for (thread_meta_vecotr_tp::const_iterator index_it = M_worke_threads.begin(); 
            index_it != M_worke_threads.end(); index_it ++) {
              if (index_it->get()->this_thread.joinable()) {
                try {
                  index_it->get()->this_thread.join();
                } catch (std::exception& join_error) {
                  PRINT_ERROR_LOGGER(join_error.what());
                };
              }
            }
      }
      
      explicit sql_thread(size_t create_thread_conut = std::thread::hardware_concurrency()) 
        :M_stop(false){
          //Create thread
            for (size_t thread_index = 0; thread_index < create_thread_conut; thread_index++) {
              thread_meta_sharedptrT new_thread_meta = std::make_shared<struct thread_meta>();
              //Add new thread task
              new_thread_meta->this_thread = std::thread([this](){
                while (true) {
                  std::function<void()> _task_func;
                  {
                    //Add new unique lock
                    std::unique_lock<std::mutex> task_unique_lock(M_task_mutex_);

                    M_condition_variable.wait(
                      task_unique_lock , [this]() { 
                        //Task is empty and thread not stop wait
                        return M_tasks.empty() || M_stop.load();
                      }
                    );

                    if (M_tasks.empty() && M_stop.load()) {
                      //Task is empty but thread stop exit
                      break;
                    }

                    if (M_tasks.empty()) {
                      //If task is empty but thread not stop continue
                      continue;
                    }

                    _task_func = std::forward<std::function<void()>>(M_tasks.front());
                    //Update task queue list
                    M_tasks.pop_front();
                  }

                  if (_task_func) {
                    _task_func();
                    {
                      //Add running count unique_lock
                      std::unique_lock<std::mutex> running_count_lock(M_running_count_mutex_);
                      
                      if (M_thread_running_count.load() > 0) {
                        //Update running count
                        M_thread_running_count --;
                      }
                      if (M_thread_running_count.load() == 0 && M_tasks.empty()) {
                        //If thread running count = 0 and tasks is empty notify all thread
                        M_condition_variable.notify_all();
                      }
                    }
                  }
                }
              });
              //Setting threa meta
              new_thread_meta->thread_idle.store(true);
              new_thread_meta->thred_processed = 1;
              M_worke_threads.push_back(std::move(new_thread_meta));
            }
      }
    };

    typedef typename std::shared_ptr<aleph::thread::sql_thread> sql_thread_shared_ptr;
  }
}

#endif