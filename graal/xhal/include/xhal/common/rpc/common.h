/**
 * @file
 * @brief Contains the classes required for defining remotely callable RPC methods
 *
 * @author Laurent Pétré <lpetre@ulb.ac.be>
 */

#ifndef XHAL_COMMON_RPC_COMMON_H
#define XHAL_COMMON_RPC_COMMON_H

#include "xhal/common/rpc/compat.h"
#include "xhal/common/rpc/helper.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "xhal/extern/wiscRPCMsg.h"

namespace xhal {
  namespace common {
    namespace rpc {

      /**
       * @brief Defines the templated RPC ABI version
       */
      static constexpr const char* abiVersion = "v1";

      /**
       * @brief Class whose all remotely callable RPC method must inherit from
       *
       * The required inheritance is used as a compile time check so a developer
       * cannot remotely call a local function by mistake.
       */
      struct Method
      {
        /**
         * @brief The operator call must be define <b>once and only once</b> per
         * RPC method.
         *
         * This templated operator declaration is only shown as an example and
         * emphasizes the need of defining it in child classes.
         *
         * @warnng The call operator @b must be defined as @c const.
         */
        template<typename R, typename... Args>
        R operator()(Args...) const;
      };

      /**
       * @brief Base of the @c MessageSerializer and @c MessageDeserializer classes
       *
       * @c MessageBase provides the key index tracking functionnality which
       * is mandatory for serialization.
       */
      class MessageBase {

        /**
         * @brief Index to the next free/unread key
         */
        uint32_t _keyIdx = 0;

      protected:

        /**
         * @brief Returns the next free/unread key
         */
        inline uint32_t dispenseKey() { return _keyIdx++; }

      };

      /**
       * @brief This class serializes parameters into a @c wisc::RPCMsg
       */
      class MessageSerializer : public MessageBase
      {

      protected:

        wisc::RPCMsg *m_wiscMsg;

        /**
         * @brief Adds @c t to the message via a user provided serializer
         */
        template<typename T,
                 typename std::enable_if<
                   helper::is_serialize_present<MessageSerializer, T>::value, int>::type = 0>
        inline void saveImpl(const T &t) {
          // This const_cast is safe when the API is used as intented
          // More precisely when the object t is modified only with the operator&
          serialize(*this, const_cast<T &>(t));
        }

        /**
         * @brief Adds @c t to the message via a split user provided serializer
         */
        template<typename T,
                 typename std::enable_if<
                   helper::is_save_present<MessageSerializer, T>::value, int>::type = 0>
        inline void saveImpl(const T &t) {
          save(*this, t);
        }

        /**
         * @brief Supresses implicit type conversions and warns the user about the error
         *
         * The function serves two purposes:
         *
         * 1. It delegates the serialization to a well-known function.
         * 2. It aims at enforcing maximum type compatibility with the UW RPC API by
         *    remembering the developer that she/he can transmit defined types over the
         *    network.
         */
        template<typename T,
                 typename std::enable_if<
                   !helper::is_serialize_present<MessageSerializer, T>::value &&
                   !helper::is_save_present     <MessageSerializer, T>::value, int>::type = 0>
        inline void saveImpl(const T&) {
          static_assert(!std::is_same<T,T>::value,
                        "Type serialization not natively supported. Writing custom serializer is required.");
        }

        /**
         * @brief Adds a @c std::uint32_t to the message
         */
        inline void saveImpl(const std::uint32_t value) {
          m_wiscMsg->set_word(std::to_string(dispenseKey()), value);
        }

        /**
         * @brief Adds a @c std::vector<std::uint32_t> to the message
         */
        inline void saveImpl(const std::vector<std::uint32_t> &value) {
          m_wiscMsg->set_word_array(std::to_string(dispenseKey()), value);
        }

        /**
         * @brief Adds a @c std::string to the message
         */
        inline void saveImpl(const std::string &value) {
          m_wiscMsg->set_string(std::to_string(dispenseKey()), value);
        }

        /**
         * @brief Adds a @c std::vector<std::string> to the message
         */
        inline void saveImpl(const std::vector<std::string> &value) {
          m_wiscMsg->set_string_array(std::to_string(dispenseKey()), value);
        }

        /**
         * @brief Adds a @c std::array<T> to the message where @c T is an integral type (except @c bool)
         */
        template<typename T,
                 std::size_t N,
                 typename std::enable_if<std::is_integral<T>::value && !helper::is_bool<T>::value, int>::type = 0
                >
        inline void saveImpl(const std::array<T, N> &value) {
          m_wiscMsg->set_binarydata(std::to_string(dispenseKey()), value.data(), N*sizeof(T));
        }

        /**
         * @brief Adds a @c std::map<std::uint32_t, T> to the message where @c T is a serializable type
         */
        template<typename T>
        inline void saveImpl(const std::map<std::uint32_t, T> &value) {
          // The first RPC key stores the std::map keys
          // This is required to know the std::map size at deserialization
          const auto keysKey = dispenseKey();

          std::vector<std::uint32_t> keys{};
          keys.reserve(value.size());

          for (const auto & elem : value) {
            keys.push_back(elem.first);
            this->saveImpl(elem.second);
          }

          m_wiscMsg->set_word_array(std::to_string(keysKey), keys);
        }

        /**
         * @brief Adds a @c std::map<std::string, T> to the message where @c T is a serializable type
         */
        template<typename T>
        inline void saveImpl(const std::map<std::string, T> &value) {
          // The first RPC key stores the std::map keys
          // This is required to know the std::map size at deserialization
          const auto keysKey = dispenseKey();

          std::vector<std::string> keys{};
          keys.reserve(value.size());

          for (const auto & elem : value) {
            keys.push_back(elem.first);
            this->saveImpl(elem.second);
          }

          m_wiscMsg->set_string_array(std::to_string(keysKey), keys);
        }

        /**
         * @brief Adds the content of a @c void_holder to the message
         *
         * It should be used when setting the result from a function call.
         */
        template<typename T>
        inline void saveImpl(const compat::void_holder<T> &holder) {
          this->saveImpl(holder.get());
        }

        /**
         * @brief Specialization for the @c void special case
         */
        inline void saveImpl(compat::void_holder<void>) {}

        /**
         * @brief Serializes the arguments from a @c std::tuple
         *
         * @c std::tuple content is add from left to right to the message
         * via a recursive template. It should be to serialize function arguments.
         */
        template<std::size_t I = 0,
                 typename... Args,
                 typename std::enable_if<I < sizeof...(Args), int>::type = 0
                >
        inline void saveImpl(const std::tuple<Args...> &args) {
          this->saveImpl(std::get<I>(args));
          this->saveImpl<I+1>(args);
        }

        /**
         * @brief Terminal call
         */
        template<std::size_t I = 0,
                 typename... Args,
                 typename std::enable_if<I == sizeof...(Args), int>::type = 0
                >
        inline void saveImpl(const std::tuple<Args...> &) {}

      public:

        /**
         * @brief Constructor
         *
         * Data are serialized into the @c wiscMsg message.
         */
        explicit MessageSerializer(wisc::RPCMsg *wiscMsg) noexcept : m_wiscMsg{wiscMsg} {}

        /**
         * @brief Allows to serialize data into the message with a natural interface
         */
        template<typename T>
        inline MessageSerializer & operator<<(const T &t) {
          this->saveImpl(t);
          return *this;
        }

        /**
         * @brief Behaves as @c operator<<
         *
         * Is used for providing a unifed interface between @c MessageSerializer and
         * @c MessageDeserializer so custom types serialization can be defined in a single
         * function.
         */
        template<typename T>
        inline MessageSerializer & operator&(const T &t) {
          this->saveImpl(t);
          return *this;
        }

      };

      /**
       * @brief This class deserializes parameters from a @c wisc::RPCMsg
       *
       * While it cannot be made @c const because deserializing requires to keep
       * track of the state, this class guarentees that the original @c wisc::RPCMsg
       * object will remain untouched.
       */
      class MessageDeserializer : public MessageBase {

      protected:

        const wisc::RPCMsg *m_wiscMsg;

        /**
         * @brief Retrives @c t from the message via a user provided deserializer
         */
        template<typename T,
                 typename std::enable_if<
                   helper::is_serialize_present<MessageDeserializer, T>::value, int>::type = 0>
        inline void loadImpl(T &t) {
          serialize(*this, t);
        }

        /**
         * @brief Retrives @c t from the message via a split user provided deserializer
         */
        template<typename T,
                 typename std::enable_if<
                   helper::is_load_present<MessageDeserializer, T>::value, int>::type = 0>
        inline void loadImpl(T &t) {
          load(*this, t);
        }

        /**
         * @brief Supresses implicit type conversions and warns the user about the error
         *
         * Every type not defined hereunder is taken care of by this templated function.
         *
         * 1. It delegates the deserialization to a well-known function.
         * 2. It aims at enforcing maximum type compatibility with the UW RPC API by
         *    reminding the developer that she/he can transmit defined types over the
         *    network.
         */
        template<typename T,
                 typename std::enable_if<
                   !helper::is_serialize_present<MessageDeserializer, T>::value &&
                   !helper::is_load_present<MessageDeserializer, T>::value, int>::type = 0>
        inline void loadImpl(const T&) {
          static_assert(!std::is_same<T,T>::value, "Type not natively supported. Writing custom serializer is required.");
        }

        /**
         * @brief Retrieves a @c std::uint32_t from the message
         */
        inline void loadImpl(uint32_t &value) {
          value = m_wiscMsg->get_word(std::to_string(dispenseKey()));
        }

        /**
         * @brief Retrieves a @c std::vector<std::uint32_t> from the message
         */
        inline void loadImpl(std::vector<std::uint32_t> &value) {
          value = m_wiscMsg->get_word_array(std::to_string(dispenseKey()));
        }

        /**
         * @brief Retrieves a @c std::string from the message
         */
        inline void loadImpl(std::string &value) {
          value = m_wiscMsg->get_string(std::to_string(dispenseKey()));
        }

        /**
         * @brief Retrieves a @c std::vector<std::string> from the message
         */
        inline void loadImpl(std::vector<std::string> &value) {
          value = m_wiscMsg->get_string_array(std::to_string(dispenseKey()));
        }

        /**
         * @brief Retrieves a @c std::array<T> from the message where @c T is an integral type (except @c bool)
         */
        template<typename T,
                 std::size_t N,
                 typename std::enable_if<std::is_integral<T>::value && !helper::is_bool<T>::value, int>::type = 0
                >
        inline void loadImpl(std::array<T, N> &value) {
          m_wiscMsg->get_binarydata(std::to_string(dispenseKey()), value.data(), N*sizeof(T));
        }

        /**
         * @brief Retrieves a @c std::map<std::uint32_t, T> from the message where @c T is a serializable type
         */
        template<typename T>
        inline void loadImpl(std::map<std::uint32_t, T> &value) {
          const auto keys = m_wiscMsg->get_word_array(std::to_string(dispenseKey()));

          for (const auto & key: keys) {
            T val;
            this->loadImpl(val);
            value.emplace(key, std::move(val));
          }
        }

        /**
         * @brief Retrieves a @c std::map<std::string, T> from the message where @c T is a serializable type
         */
        template<typename T>
        inline void loadImpl(std::map<std::string, T> &value) {
          const auto keys = m_wiscMsg->get_string_array(std::to_string(dispenseKey()));

          for (const auto & key: keys) {
            T val;
            this->loadImpl(val);
            value.emplace(key, std::move(val));
          }
        }

        /**
         * @brief Retrieves a @c T parameter from the message and stores it inside
         * a @c void_holder.
         *
         * It should be used when setting the result from a function.
         */
        template<typename T>
        inline void loadImpl(compat::void_holder<T> &value) {
          this->loadImpl(value.get());
        }

        /**
         * @brief Specialization for the @c void special case
         */
        inline void loadImpl(compat::void_holder<void>) {}

        /**
         * @brief Fills in a @c std::tuple with data from the message
         *
         * @c std::tuple content is filled from left to right from the message
         * via a recursive template. It should be use to deserialize function
         * arguments.
         */
        template<std::size_t I = 0,
                 typename... Args,
                 typename std::enable_if<I < sizeof...(Args), int>::type = 0
                >
        inline void loadImpl(std::tuple<Args...> &args) {
          this->loadImpl(std::get<I>(args));
          this->loadImpl<I+1>(args);
        }

        /**
         * @brief Terminal call
         */
        template<std::size_t I = 0,
                 typename... Args,
                 typename std::enable_if<I == sizeof...(Args), int>::type = 0
                >
        inline void loadImpl(std::tuple<Args...> &) { }

      public:

        /**
         * @brief Constructor
         *
         * Data are retrieved from the provided @c wiscMsg message.
         */
        explicit MessageDeserializer(const wisc::RPCMsg *wiscMsg) noexcept : m_wiscMsg{wiscMsg} {}

        /**
         * @brief Allows to deserialiaze data from the message with a natural interface
         */
        template<typename T>
        inline MessageDeserializer & operator>>(T &t) {
          this->loadImpl(t);
          return *this;
        }

        /**
         * @brief Behaves as @c operator<<
         *
         * Is used for providing a unifed interface between @c MessageSerializer and
         * @c MessageDeserializer so custom types serialization can be defined in a single
         * function.
         */
        template<typename T>
        inline MessageDeserializer & operator&(T &t) {
          this->loadImpl(t);
          return *this;
        }

      };

      /**
       * @brief Provides a default (de)serialiazer in case the intrusive method is used
       */
      template<typename Message, typename T>
      inline auto serialize(Message &msg, T &t) -> decltype(t.serialize(msg)) {
        t.serialize(msg);
      }

      /**
       * @brief Provides a default split serialiazer in case the intrusive method is used
       */
      template<typename Message, typename T>
      inline auto save(Message &msg, const T &t) -> decltype(t.save(msg)) {
        t.save(msg);
      }

      /**
       * @brief Provides a default split deserialiazer in case the intrusive method is used
       */
      template<typename Message, typename T>
      inline auto load(Message &msg, T &t) -> decltype(t.load(msg)) {
        t.load(msg);
      }

      /**
       * @brief Serializer for @c std::array<T, N> where @c is a serializable type
       *
       * This a simple example of custom type serialization.
       *
       * The library provides two custom type serialization methods:
       *
       * 1. The intrusive method
       * 2. The non-intrusive method
       *
       * Let's take an example :
       *
       * @code{.cpp}
       * struct Point
       * {
       *     std::uint32_t x, y;
       *
       *     // The intrusive version is implemented as a new member function
       *     // which takes a message as parameter (i.e. the serializer or deserializer)
       *     template<class Message> inline void serialize(Message & msg) {
       *         msg & x & y;
       *     }
       *
       *     // The (de)serializers can also be implemented as two split functions
       *     // which also take a message as parameter (i.e. the serializer or deserializer)
       *     template<class Message> inline void save(Message & msg) {
       *         msg << x << y;
       *     }
       *     template<class Message> inline void load(Message & msg) {
       *         msg >> x >> y;
       *     }
       * };
       *
       * // The non-intrusive version allows to serialize objects defined in a library
       * // Simply define the serialize function in the xhal::common::rpc namespace or the namespace
       * // where the type is defined with two parameters (1) A message (i.e. the serializer
       * // or deserializer) and (2) the custom type
       * namespace xhal { namspace rpc {
       *     template<typename Message> inline void serialize(Message &msg, Point &point) {
       *         msq & point.x & point.y;
       *     }
       * } }
       * @endcode
       *
       * @warning In order to work as intended the @c serialize functions @b MUST modify
       * the object only with the @c operator&
       */
      template<typename Message, typename T, std::size_t N>
      inline void serialize(Message &msg, std::array<T, N> &value) {
        // The std::array size is known at compile time (and part of
        // the signature), so we don't need to serialize it
        for (auto & elem: value) {
          msg & elem;
        }
      }

      /**
       * @brief (De)serializer for @c std::vector<T> where @c T is a (de)serializable type
       */
      template<typename Message, typename T>
      inline void serialize(Message &msg, std::vector<T> &value) {
        // 1. Store or retrieve the vector length
        std::uint32_t length = value.size(); // no-op during deserialization
        msg & length;
        value.resize(length); // no-op during serialization

        // 2. Store or retrieve the vector elements
        for (std::uint32_t i = 0; i < length; ++i) {
          msg & value[i];
        }
      }

      /**
       * @brief (De)serializer for @c bool
       */
      template<typename Message>
      inline void save(Message &msg, const bool &value) {
        msg << static_cast<std::uint32_t>(value);
      }
      template<typename Message>
      inline void load(Message &msg, bool &value) {
        std::uint32_t tmp;
        msg >> tmp;
        value = tmp;
      }

      /**
       * @brief (De)serializer for @c std::uint8_t
       */
      template<typename Message>
      inline void save(Message &msg, const std::uint8_t &value) {
        msg << static_cast<std::uint32_t>(value);
      }
      template<typename Message>
      inline void load(Message &msg, std::uint8_t &value) {
        std::uint32_t tmp;
        msg >> tmp;
        value = tmp;
      }

      /**
       * @brief (De)serializer for @c std::uint16_t
       */
      template<typename Message>
      inline void save(Message &msg, const std::uint16_t &value) {
        msg << static_cast<std::uint32_t>(value);
      }
      template<typename Message>
      inline void load(Message &msg, std::uint16_t &value) {
        std::uint32_t tmp;
        msg >> tmp;
        value = tmp;
      }

      /**
       * @brief (De)serializer for the @float type
       *
       * Works only if @sizeof(float) is 4 and the representation 
       * is the same on the client and the server.
       */
      template<typename Message>
      inline void save(Message &msg, const float &value) {
        // This serializer works only for IEEE 754 floating point numbers
        static_assert(std::numeric_limits<float>::is_iec559,
                      "Floating point representation not supported by the serializer.");

        std::uint32_t tmp;
        std::memcpy(&tmp, &value, sizeof(std::uint32_t));
        msg << tmp;
      }
      template<typename Message>
      inline void load(Message &msg, float &value) {
        // This deserializer works only for IEEE 754 floating point numbers
        static_assert(std::numeric_limits<float>::is_iec559,
                      "Floating point representation not supported by the deserializer.");

        std::uint32_t tmp;
        msg >> tmp;
        std::memcpy(&value, &tmp, sizeof(float));
      }

    }
  }
}

#endif // XHAL_COMMON_RPC_COMMON_H
