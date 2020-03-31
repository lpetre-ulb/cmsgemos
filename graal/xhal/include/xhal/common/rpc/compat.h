/**
 * @file
 * @brief Compatibility functionalities for C++11
 *
 * This file includes all the functionalities that could advantageously be
 * replaced with newer versions of C++, e.g. C++14 and C++17.
 *
 * Since the content of this file implements missing functionalties in the
 * C++11 standard, the coding style follows what is found in the STL.
 *
 * @author Laurent Pétré <lpetre@ulb.ac.be>
 */

#ifndef XHAL_COMMON_RPC_COMPAT_H
#define XHAL_COMMON_RPC_COMPAT_H

#include <cstddef>
#include <tuple>

namespace xhal {
  namespace common {
    namespace rpc {
      namespace compat {

        /**
         * @brief This class can encapsulates any type, including @c void
         *
         * @c void is an incomplete type and cannot be easily used in fully generic
         * templates. This class is designed to hold any type, including `void` so that
         * it can be used in generic templates.
         *
         * The embedded object can be retrieved with the @c get() methods.
         *
         * In C++17 any code using this class is easily replaced with the
         * <tt>constexpr if</tt> statement.
         */
        template<typename T>
        struct void_holder
        {
          T t;
          T &get() { return t; }
          const T &get() const { return t; }
        };

        /**
         * @brief Specialization for the @c void type
         */
        template<>
        struct void_holder<void>
        {
          void get() const { return; }
        };

        /**
         * @brief This template class defines an indices sequence
         *
         * Please look at the @c integer_sequence from C++14 to get more
         * information.
         */
        template<std::size_t... N>
        struct index_sequence
        {
        };

        /**
         * @brief Generates an indices sequence
         *
         * The code follows the usual O(n) implementation.
         *
         * You can have a look at https://blog.galowicz.de/2016/06/24/integer_sequences_at_compile_time/
         * for more information.
         */
        template<std::size_t... I>
        struct index_sequence_gen;

        /**
         * @brief Non-terminal call
         */
        template<std::size_t I, std::size_t... N>
        struct index_sequence_gen<I, N...>
        {
          // I is the recursion index
          // N... are the generated indices.
          using type = typename index_sequence_gen<I-1, I-1, N...>::type;
        };

        /**
         * @brief Terminal call
         */
        template<std::size_t... N>
        struct index_sequence_gen<0, N...>
        {
          using type = index_sequence<N...>;
        };

        /**
         * @brief Helper making an index sequence from @c 0 to @c N-1
         *
         * Remind that an index sequence is the non-type template parameter of a
         * specialization of the @c index_sequence template class.
         */
        template<std::size_t N>
        using make_index_sequence = typename index_sequence_gen<N>::type;

        /**
         * @brief Calls a function with arguments from a @c std::tuple
         *
         * This function is the implementation part of a specialized @c std::apply
         * implementation. More information can be found in the C++17 standard.
         *
         * Please note that these functions are applied to our case and do not
         * pretend to respect the C++17 standard in any way. Moreover the @c void
         * return case is handled with our @c void_holder container. This imposes
         * the first template argument to be explicitly set when calling @c tuple_apply
         */
        template<typename F,
                 typename... Args,
                 std::size_t... N
                >
        auto tuple_apply_impl(F &&f, const std::tuple<Args...> &tuple, index_sequence<N...>)
          -> decltype(std::forward<F>(f)(std::get<N>(tuple)...))
        {
          return std::forward<F>(f)(std::get<N>(tuple)...);
        }

        /**
         * @brief Generic case
         */
        template<typename R,
                 typename F,
                 typename... Args,
                 typename std::enable_if<!std::is_void<R>::value, int>::type = 0
                >
        void_holder<R> tuple_apply(F &&f, const std::tuple<Args...> &tuple)
        {
          return void_holder<R>{
            tuple_apply_impl(std::forward<F>(f), tuple, make_index_sequence<sizeof...(Args)>{})
          };
        }

        /**
         * @brief Specialization for the @c void type
         */
        template<typename R,
                 typename F,
                 typename... Args,
                 typename std::enable_if<std::is_void<R>::value, int>::type = 0
                >
        void_holder<void> tuple_apply(F &&f, const std::tuple<Args...> &tuple)
        {
          tuple_apply_impl(std::forward<F>(f), tuple, make_index_sequence<sizeof...(Args)>{});
          return {};
        }

      }
    }
  }
}

#endif
