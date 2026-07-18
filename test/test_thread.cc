#include <aleph/aleph_sql.hpp>
#include <iostream>

int main() {
  aleph::thread::thread_manager& thread = aleph::thread::thread_manager::get_instance();
  thread.init_thread(3);
  aleph::thread::sql_thread_shared_ptr new_thread = thread.get_thread();
    new_thread->post_new_task([](){
      std::cerr << "OK" << std::endl;
    });
    new_thread->post_new_task([](const std::string& name){
      std::cerr << "HELLO '" << name << "' Mis" << std::endl;
    } , "Nana");
  return 0;
}