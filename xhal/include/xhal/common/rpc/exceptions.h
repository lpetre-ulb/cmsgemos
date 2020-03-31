/**
 * @file
 * @brief This file contains helpers for exception handling.
 *
 * @author Louis Moureaux <lmoureau@ulb.ac.be>
 */

#ifndef XHAL_COMMON_RPC_EXCEPTIONS_H
#define XHAL_COMMON_RPC_EXCEPTIONS_H

#include "xhal/extern/wiscrpcsvc.h"

namespace xhal {
  namespace common {
    namespace rpc {
      namespace helper {

        /**
         * @brief Retrieves a user-friendly message for the given exception.
         *
         * Specialization for @c std::exception.
         */
        std::string getExceptionMessage(const std::exception &e);

        /**
         * @brief Retrieves a user-friendly message for the given exception.
         *
         * Specialization for @c wisc::RPCMsg::BadKeyException.
         *
         * @note This function is never called because the constructor of @c BadKeyException calls
         *       @c std::abort. It is kept in case the issue is corrected in the future.
         */
        std::string getExceptionMessage(const wisc::RPCMsg::BadKeyException &e);

        /**
         * @brief Retrieves a user-friendly message for the given exception.
         *
         * Specialization for @c wisc::RPCMsg::TypeException.
         */
        std::string getExceptionMessage(const wisc::RPCMsg::TypeException &e);

        /**
         * @brief Retrieves a user-friendly message for the given exception.
         *
         * Specialization for @c wisc::RPCMsg::BufferTooSmallException.
         */
        std::string getExceptionMessage(const wisc::RPCMsg::BufferTooSmallException &e);

        /**
         * @brief Retrieves a user-friendly message for the given exception.
         *
         * Specialization for @c wisc::RPCMsg::CorruptMessageException.
         */
        std::string getExceptionMessage(const wisc::RPCMsg::CorruptMessageException &e);

        /**
         * @brief Retrieves a user-friendly message for the given exception.
         *
         * Specialization for @c wisc::RPCSvc::RPCException and derived types.
         */
        std::string getExceptionMessage(const wisc::RPCSvc::RPCException &e);

        /**
         * @brief Sets the type of the current exception in @c response.
         * @internal This function makes use of low-level functions of the Itanium C++ ABI to
         *           retrieve the type name of the current exception.
         */
        void setExceptionType(wisc::RPCMsg *response);

        /**
         * @brief Fetches an error message from @c response.
         *
         * The @c error key must be set and to a string.
         */
        std::string readExceptionMessage(const wisc::RPCMsg &response);

      }
    }
  }
}

#endif // XHAL_COMMON_RPC_EXCEPTIONS_H
