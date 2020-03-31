/**
 * @file
 * @brief This file contains all the functions needed to register
 * a remotely callable RPC method
 *
 * @author Laurent Pétré <lpetre@ulb.ac.be>
 * @author Louis Moureaux <lmoureau@ulb.ac.be>
 */

#ifndef XHAL_COMMON_RPC_REGISTER_H
#define XHAL_COMMON_RPC_REGISTER_H

#include "xhal/common/rpc/common.h"
#include "xhal/common/rpc/compat.h"
#include "xhal/common/rpc/exceptions.h"
#include "xhal/common/rpc/helper.h"

#include "xhal/extern/ModuleManager.h" // Only present in the CTP7 modules

#include <typeinfo>

namespace xhal {
  namespace common {
    namespace rpc {
      namespace helper {

        /**
         * @brief Handles an exception, setting the error key on the response.
         *
         * In case a second exception occurs when setting the error key, @c std::terminate is called.
         */
        template<typename Exception>
        void handleException(const Exception &e, wisc::RPCMsg *response) noexcept {
          // Log exception here?
          response->set_string(std::string(abiVersion) + ".error", getExceptionMessage(e));
          setExceptionType(response);
        }

        /**
         * @brief Handles an unknown exception, setting the error key on the response.
         *
         * In case an exception occurs when setting the error key, @c std::terminate is called.
         */
        void handleException(wisc::RPCMsg *response) noexcept {
          // Log exception here?
          response->set_string(std::string(abiVersion) + ".error", "unknown exception type");
          setExceptionType(response);
        }

      } // namespace helper

      /**
       * @brief Locally invoke a RPC method
       *
       * This function is the wrapper called for every remote function call. It
       * deserializes the arguments from the @c wisc::RPCMsg, calls the local functor
       * and then serializes the return value to the @c wisc::RPCMsg.
       */
      template<typename Method,
               typename std::enable_if<std::is_base_of<xhal::common::rpc::Method, Method>::value, int>::type = 0
              >
      void invoke(const wisc::RPCMsg *request, wisc::RPCMsg *response) noexcept {
        try {
          // Remove the cv-qualifiers and references since we need
          // a copy of the object
          helper::functor_decay_args_t<Method> args;

          MessageDeserializer query(request);
          query >> args;

          // Call the Method functor with the arguments received from
          // the RPC message
          auto result = compat::tuple_apply<helper::functor_return_t<Method>>(Method{}, args);

          // Serialize the reply
          MessageSerializer reply(response);
          reply << result;
        } catch (const std::exception &e) {
          helper::handleException(e, response);
        } catch (const wisc::RPCMsg::BadKeyException &e) {
          helper::handleException(e, response);
        } catch (const wisc::RPCMsg::TypeException &e) {
          helper::handleException(e, response);
        } catch (const wisc::RPCMsg::BufferTooSmallException &e) {
          helper::handleException(e, response);
        } catch (const wisc::RPCMsg::CorruptMessageException &e) {
          helper::handleException(e, response);
        } catch (...) {
          helper::handleException(response);
        }
      }

      /**
       * @brief Register a RPC method into the @c ModuleManager
       *
       * This helper function register a RPC method with the right parameters
       * so it can be remotely called.
       */
      template<typename Method>
      void registerMethod(ModuleManager *modmgr) {
        // The method name is taken from the typeid
        // This is implementation dependent but g++ and clang++
        // follow the same convention
        return modmgr->register_method(abiVersion,
                                       typeid(Method).name(),
                                       xhal::common::rpc::invoke<Method>);
      }
    }
  }
}

#endif // XHAL_COMMON_RPC_REGISTER_H
