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
#ifndef ALEPH_SQL_FUNCTION_TARIS
#define ALEPH_SQL_FUNCTION_TARIS

#include <type_traits>
#include <utility>

namespace aleph {
  namespace function_trais {
    template<typename Tp , typename Up>
    struct aleph_comparator_function_ret {
      using result = std::false_type;
    };
    template<typename Tp>
    struct aleph_comparator_function_ret<Tp, Tp> {
      using result = std::true_type;
    };
    template<typename F , typename ...Args>
    struct aleph_function_ret_type {
      using type = decltype(std::declval<F>()(std::declval<Args>() ...));
    };
  }
}

#endif