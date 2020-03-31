/**
 * @file
 * @brief Centralize all the template helper tools
 *
 * Since the content of this file could be integrated into a C++
 * standard template metaprogramming, the coding style follows
 * what is found in the STL.
 *
 * @author Laurent Pétré <lpetre@ulb.ac.be>
 */

#ifndef XHAL_COMMON_RPC_HELPER_H
#define XHAL_COMMON_RPC_HELPER_H

#include <tuple>
#include <type_traits>

namespace xhal {
  namespace common {
    namespace rpc {
      namespace helper {

        /**
         * @brief Allows to extract types of a functor
         *
         * This templated class provides the return type and argument types
         * of a functor as traits.
         *
         * * @c return_type is the functor return type
         * * @c decay_args_type are the decayed (cv-qualifiers and reference are
         *   removed) argument types in a @c std::tuple
         * * @c forward_as_tuple returns a function converting its arguments
         *   to the arguments functor types
         *
         * Inspired by https://stackoverflow.com/a/10400131
         */
        template<typename>
        struct functor_traits;

        template<typename Obj,
                 typename R,
                 typename... Args
                >
        struct functor_traits<R (Obj::*)(Args...) const>
        {
          using return_type = R;
          using decay_args_type = std::tuple<typename std::decay<Args>::type...>;

          static constexpr inline auto forward_as_tuple() {
            return [](const Args&... args){ return std::forward_as_tuple(args...); };
          }
        };

        /**
         * @brief Implementation of @c functor_return_t
         *
         * Should not be used as a function.
         */
        template<typename Func,
                 typename Traits = functor_traits<Func>,
                 typename R = typename Traits::return_type
                >
        R functor_return_t_impl(Func);

        /**
         * @brief Return type of the @c Obj functor
         *
         * Only works with @c const call operators since @c functor_traits is
         * only defined for those.
         */
        template<typename Obj>
        using functor_return_t = decltype(functor_return_t_impl(&Obj::operator()));

        /**
         * @brief Implementation of @c functor_decay_args_t
         *
         * Should not be used as a function.
         */
        template<typename Func,
                 typename Traits = functor_traits<Func>,
                 typename Args = typename Traits::decay_args_type
                >
        Args functor_decay_args_t_impl(Func);

        /**
         * @brief @c std::tuple whose types are the functor argument types
         *
         * Only works with @c const call operators since @c functor_traits is
         * only defined for those.
         */
        template<typename Obj>
        using functor_decay_args_t = decltype(functor_decay_args_t_impl(&Obj::operator()));

        /**
         * @brief Helper function to forward as a tuple matching the functor signature
         */
        template<typename Obj>
        constexpr inline auto get_forward_as_tuple()
        {
          return functor_traits<decltype(&Obj::operator())>::forward_as_tuple();
        }

        /**
         * @brief Checks whether the template parameter @c T is a @c bool
         */
        template<typename T>
        using is_bool = std::is_same<typename std::decay<T>::type, bool>;

        /**
         * @brief Checks whether the template parameter @c T is a @c std::tuple
         */
        template<typename T>
        struct is_tuple_impl :
          std::false_type
        {
        };

        template<typename... T>
        struct is_tuple_impl<std::tuple<T...>> :
          std::true_type
        {
        };

        /**
         * @brief Helper alias for cv-qualified and reference types
         */
        template<typename T>
        using is_tuple = is_tuple_impl<typename std::decay<T>::type>;

        /**
         * @brief Defines the @c is_func_present class which allows to check the existence of @func
         *
         * The function parameter types are given as the @c Args templated arguments while the
         * lookup result is available in the member @c value.
         *
         * \details
         *
         * Internally the class work as follow.
         *
         * Two overloads of the @c test function are defined. The first one always returns the @no
         * type while the second one return the type which would be returned if func(Args...) was
         * called.
         *
         * As per SFINAE, the second function is not considered if the func(Args...) does not
         * exist. Whether func(Args...) exists is as simple as checking the return type of
         * test<AArgs...>.
         *
         * When both overloads exist (i.e. func(Args...) exists), the second overload is prefered
         * because the argument @c 0  matches better @c int than @c ...
         */
        #define XHAL_COMMON_RPC_HELPER_FUNCTION_PRESENT(func)                                 \
        template <typename... Args>                                                           \
        class is_##func##_present                                                             \
        {                                                                                     \
          struct no {};                                                                       \
                                                                                              \
          template <typename... AArgs>                                                        \
          static no test(...);                                                                \
                                                                                              \
          template <typename... AArgs>                                                        \
          static decltype(func(std::declval<AArgs&>()...)) test(int);                         \
                                                                                              \
        public:                                                                               \
                                                                                              \
          static constexpr bool value = !std::is_same<no, decltype(test<Args...>(0))>::value; \
        };

        XHAL_COMMON_RPC_HELPER_FUNCTION_PRESENT(serialize)
        XHAL_COMMON_RPC_HELPER_FUNCTION_PRESENT(save)
        XHAL_COMMON_RPC_HELPER_FUNCTION_PRESENT(load)

        #undef XHAL_COMMON_RPC_HELPER_FUNCTION_PRESENT

      }
    }
  }
}

#endif // XHAL_COMMON_RPC_HELPER_H
