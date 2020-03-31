#include "xhal/common/rpc/exceptions.h"

#include "xhal/common/rpc/common.h" // abiVersion

#include "cxxabi.h" // C++ Itanium ABI

std::string xhal::common::rpc::helper::getExceptionMessage(const std::exception &e)
{
  return e.what();
}

std::string xhal::common::rpc::helper::getExceptionMessage(const wisc::RPCMsg::BadKeyException &e)
{
  return "bad RPC key: " + e.key;
}

std::string xhal::common::rpc::helper::getExceptionMessage(const wisc::RPCMsg::TypeException &e)
{
  return "RPC type error";
}

std::string xhal::common::rpc::helper::getExceptionMessage(const wisc::RPCMsg::BufferTooSmallException &e)
{
  return "buffer too small";
}

std::string xhal::common::rpc::helper::getExceptionMessage(const wisc::RPCMsg::CorruptMessageException &e)
{
  return "corrupt RPC message: " + e.reason;
}

std::string xhal::common::rpc::helper::getExceptionMessage(const wisc::RPCSvc::RPCException &e)
{
  return e.message;
}

void xhal::common::rpc::helper::setExceptionType(wisc::RPCMsg *response)
{
  // Fetch the type of the current exception
  const std::type_info *exceptionType = abi::__cxa_current_exception_type();
  if (exceptionType != nullptr) {
    // Try to demangle it
    char *demangled = abi::__cxa_demangle(exceptionType->name(),
                                          nullptr, nullptr, nullptr);
    if (demangled != nullptr) {
      response->set_string(std::string(abiVersion) + ".type", demangled);
      std::free(demangled);
    } else {
      // Could not demangle, use raw name
      response->set_string(std::string(abiVersion) + ".type", exceptionType->name());
    }
  }
}

std::string xhal::common::rpc::helper::readExceptionMessage(const wisc::RPCMsg &response)
{
  std::string msg = "remote error: ";
  if (response.get_key_exists(std::string(abiVersion) + ".type")) {
    msg += response.get_string(std::string(abiVersion) + ".type") + ": ";
  }
  msg += response.get_string(std::string(abiVersion) + ".error");
  return msg;
}
