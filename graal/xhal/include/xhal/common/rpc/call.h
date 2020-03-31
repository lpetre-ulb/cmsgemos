/**
 * @file
 * @brief This file contains all the functions needed to call
 * remotely a RPC method
 *
 * @author Laurent Pétré <lpetre@ulb.ac.be>
 * @author Louis Moureaux <lmoureau@ulb.ac.be>
 */

#ifndef XHAL_COMMON_RPC_CALL_H
#define XHAL_COMMON_RPC_CALL_H

#include "xhal/common/rpc/common.h"
#include "xhal/common/rpc/compat.h"
#include "xhal/common/rpc/exceptions.h"
#include "xhal/common/rpc/helper.h"

#include "xhal/extern/wiscrpcsvc.h"

#include <typeinfo>

namespace xhal {
  namespace common {
    namespace rpc {

      /**
       * @brief Remotely call a RPC method
       *
       * The remote method called is defined by the template parameter
       * @c Method. The arguments to give to the function are those from
       * the @c Method::operator() signature and their types @b must be deducible.
       */
      template<typename Method,
               typename... Args,
               typename std::enable_if<std::is_base_of<xhal::common::rpc::Method, Method>::value, int>::type = 0
              >
      helper::functor_return_t<Method> call(wisc::RPCSvc &connection, Args&&... args);

      /**
       * @brief Thrown by @ref call when an exception is thrown on the remote host.
       */
      class RemoteException : public std::runtime_error
      {
        std::string m_type;

        /**
         * @brief @ref call is the only function that can throw this exception.
         */
        template<typename Method,
                 typename... Args,
                 typename std::enable_if<std::is_base_of<xhal::common::rpc::Method, Method>::value, int>::type
                >
        friend helper::functor_return_t<Method> call(wisc::RPCSvc &connection,
                                                     Args&&... args);

        /**
         * @brief Constructor.
         * @param response An RPC response to extract error information from.
         */
        explicit RemoteException(const wisc::RPCMsg &response) :
          std::runtime_error(helper::readExceptionMessage(response)),
          m_type(response.get_key_exists(std::string(abiVersion) + ".type") ?
                 response.get_string(std::string(abiVersion) + ".type") : "")
        {}

      public:
        /**
         * @brief Returns @c true if the type of the exception is available.
         */
        bool hasType() const { return !m_type.empty(); }

        /**
         * @brief Returns the exception type name if available, an empty string otherwise.
         */
        std::string type() const { return m_type; }
      };

      /**
       * @brief Thrown by @c call when there is a problem calling the remote method.
       *
       * This can be either because the Wisconsin messaging layer throws an exception or because
       * the method can't be found.
       */
      class MessageException : public std::runtime_error
      {
        /**
         * @brief @ref call is the only function that can throw this exception.
         */
        template<typename Method,
                 typename... Args,
                 typename std::enable_if<std::is_base_of<xhal::common::rpc::Method, Method>::value, int>::type
                >
        friend helper::functor_return_t<Method> call(wisc::RPCSvc &connection,
                                                     Args&&... args);

        /**
         * @brief Constructor.
         */
        explicit MessageException(const std::string &message) :
          std::runtime_error(message)
        {}

        public:
          // Use what()
      };

      /* Implementation */
      template<typename Method,
               typename... Args,
               typename std::enable_if<std::is_base_of<xhal::common::rpc::Method, Method>::value, int>::type
              >
      helper::functor_return_t<Method> call(wisc::RPCSvc &connection, Args&&... args)
      {
        try {
          // The wisc::RPCMsg method name is taken from the typeid
          // This is implementation dependent but g++ and clang++
          // follow the same convention
          wisc::RPCMsg request(std::string(abiVersion) + "." + typeid(Method).name());
          MessageSerializer query{&request};

          // Type conversion from args to serializable types
          // must be performed in the same statement in order to
          // make use of lifetime extension
          query << helper::get_forward_as_tuple<Method>()(args...);

          // Remote call
          const wisc::RPCMsg response = connection.call_method(request);

          // Check for errors
          if (response.get_key_exists("rpcerror")) {
            throw MessageException(response.get_string("rpcerror"));
          } else if (response.get_key_exists(std::string(abiVersion) + ".error")) {
            throw RemoteException(response);
          }

          // The RPC method can return a void so the void_holder is required
          compat::void_holder<helper::functor_return_t<Method>> return_v;

          MessageDeserializer reply{&response};
          reply >> return_v;

          return return_v.get();

        } catch (const wisc::RPCMsg::BadKeyException &e) {
          throw MessageException(helper::getExceptionMessage(e));
        } catch (const wisc::RPCMsg::TypeException &e) {
          throw MessageException(helper::getExceptionMessage(e));
        } catch (const wisc::RPCMsg::BufferTooSmallException &e) {
          throw MessageException(helper::getExceptionMessage(e));
        } catch (const wisc::RPCMsg::CorruptMessageException &e) {
          throw MessageException(helper::getExceptionMessage(e));
        } catch (const wisc::RPCSvc::RPCException &e) {
          throw MessageException(helper::getExceptionMessage(e));
        }
      }
    }
  }
}

#endif // XHAL_COMMON_RPC_CALL_H
