/**
 * @file Exception.h
 * XHAL exception base class
 *
 * @author Mykhailo Dalchenko
 * @version 1.0
 */

#ifndef XHAL_COMMON_UTILS_EXCEPTION_H
#define XHAL_COMMON_UTILS_EXCEPTION_H

#include <string>
#include <exception>
#include <assert.h>

#define XHAL_COMMON_UTILS_DEFINE_EXCEPTION(EXCEPTION_NAME)              \
namespace xhal {                                                        \
  namespace common {                                                    \
    namespace utils {                                                   \
      class EXCEPTION_NAME : public std::exception {                    \
        public:                                                         \
          EXCEPTION_NAME(std::string message) :                         \
            msg(message) {}                                             \
                                                                        \
          virtual ~EXCEPTION_NAME() {}                                  \
                                                                        \
          virtual const char* what() const noexcept (true) override     \
          {                                                             \
              return msg.c_str();                                       \
          }                                                             \
                                                                        \
          std::string msg;                                              \
                                                                        \
        private:                                                        \
          EXCEPTION_NAME();                                             \
      };                                                                \
    }                                                                   \
  }                                                                     \
}

XHAL_COMMON_UTILS_DEFINE_EXCEPTION(XHALException)
XHAL_COMMON_UTILS_DEFINE_EXCEPTION(XHALXMLParserException)
XHAL_COMMON_UTILS_DEFINE_EXCEPTION(XHALRPCException)
XHAL_COMMON_UTILS_DEFINE_EXCEPTION(XHALRPCNotConnectedException)

#endif // XHAL_COMMON_UTILS_EXCEPTION_H
