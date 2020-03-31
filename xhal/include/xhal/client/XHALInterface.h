/**
 * @file XHALInterface.h
 * Hardware interface for XHAL
 *
 * @author Mykhailo Dalchenko
 * @version 1.0
 */

#ifndef XHAL_CLIENT_XHALINTERFACE_H
#define XHAL_CLIENT_XHALINTERFACE_H

#include <cstring>
#include <string>
#include "xhal/extern/wiscrpcsvc.h"
#include "xhal/common/utils/Exception.h"

#include "log4cplus/logger.h"
#include "log4cplus/loglevel.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"

#define XHAL_TRACE(MSG) LOG4CPLUS_TRACE(m_logger, MSG)
#define XHAL_DEBUG(MSG) LOG4CPLUS_DEBUG(m_logger, MSG)
#define XHAL_INFO(MSG)  LOG4CPLUS_INFO(m_logger, MSG)
#define XHAL_WARN(MSG)  LOG4CPLUS_WARN(m_logger, MSG)
#define XHAL_ERROR(MSG) LOG4CPLUS_ERROR(m_logger, MSG)
#define XHAL_FATAL(MSG) LOG4CPLUS_FATAL(m_logger, MSG)

#define STANDARD_CATCH                                                                                                           \
  catch (wisc::RPCSvc::NotConnectedException &e) {                                                                               \
    XHAL_ERROR("Caught NotConnectedException: " << e.message.c_str());                                                           \
    throw xhal::common::utils::XHALRPCNotConnectedException("RPC NotConnectedException: " + e.message);                          \
  } catch (wisc::RPCSvc::RPCErrorException &e) {                                                                                 \
    XHAL_ERROR("Caught RPCErrorException: " << e.message.c_str());                                                               \
    throw xhal::common::utils::XHALRPCException("RPC ErrorException: " + e.message);                                             \
  } catch (wisc::RPCSvc::RPCException &e) {                                                                                      \
    XHAL_ERROR("Caught exception: " << e.message.c_str());                                                                       \
    throw xhal::common::utils::XHALRPCException("RPC exception: " + e.message);                                                  \
  } catch (wisc::RPCMsg::BadKeyException &e) {                                                                                   \
    XHAL_ERROR("Caught exception: " << e.key.c_str());                                                                           \
    throw xhal::common::utils::XHALRPCException("RPC BadKeyException (most probably remote register not accessible): " + e.key); \
  }

#define ASSERT(x)                                                 \
  do {                                                            \
    if (!(x)) {                                                   \
      printf("Assertion Failed on line %u: %s\n", __LINE__, #x);  \
      throw xhal::common::utils::XHALException("ASSERT failure"); \
    }                                                             \
  } while (0)

namespace xhal {
  namespace client {
    /**
     * @class XHALInterface
     * @brief Provides interface to call remote procedures at Zynq CPU
     */
    class XHALInterface
    {
      public:
        /**
         * @brief Default constructor
         * @param board_domain_name domain name of CTP7
         */
        XHALInterface(const std::string& board_domain_name);

        /**
         * @brief Constructor, taking also the external logger
         * @param board_domain_name domain name of CTP7
         */
        XHALInterface(const std::string& board_domain_name, log4cplus::Logger& logger);

        virtual ~XHALInterface();

        /**
         * @brief Initialize interface and establish RPC service connection with CTP7
         */
        void connect();

        /**
         * @brief Reconnect to RPC service and reload required modules
         */
        virtual void reconnect();

        /**
         * @brief Initialize interface and establish RPC service connection with CTP7
         */
        void disconnect();

        /**
         * @brief load remote module
         */
        void loadModule(const std::string& module_name, const std::string& module_version);

        /**
         * @brief sets amount of logging/debugging information to display
         * @param loglevel:
         * 0 - ERROR
         * 1 - WARN
         * 2 - INFO
         * 3 - DEBUG
         * 4 - TRACE
         */
        void setLogLevel(int loglevel);

      protected:
        std::string m_board_domain_name;
        log4cplus::Logger m_logger;
        wisc::RPCSvc rpc;
        wisc::RPCMsg req, rsp;
        bool isConnected;
        static int index;
    };
  }
}

#endif  // XHAL_CLIENT_XHALINTERFACE_H
