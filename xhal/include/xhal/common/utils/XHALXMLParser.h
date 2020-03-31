/**
 * @file XHALXMLParser.h
 * XML parser for XHAL library. Parses the XML address table and store results in unordered map of (regName, Node)
 *
 * @author Mykhailo Dalchenko
 * @version 1.0
 */



#ifndef XHAL_COMMON_UTILS_XHALXMLPARSER_H
#define XHAL_COMMON_UTILS_XHALXMLPARSER_H

#include <string>
#include <iostream>
#include <unordered_map>
#ifdef __ARM_ARCH_7A__ //do not include boost::optional
#include <experimental/optional>
#else
#include <boost/optional.hpp>
#endif

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>

#define TRANSCODE(STR) xercesc::XMLString::transcode(STR)

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

#include "xhal/common/utils/XHALXMLNode.h"
#include "xhal/common/utils/Exception.h"

namespace xhal {
  namespace common {
    namespace utils {
      /**
       * @class XHALXMLParser
       * @brief provide parsing interface and search through flattened node tree
       */
      class XHALXMLParser
      {
        public:
          /**
           * @brief Default constructor
           * @param xmlFile address table file name
           */
          XHALXMLParser(const std::string& xmlFile);

          ~XHALXMLParser();

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

          /**
           * @brief parses XML file and creates flattened nodes tree
           */
          void parseXML();

          /**
           * @brief returns node object by its name or nothing if name is not found
           */
#ifdef __ARM_ARCH_7A__
          std::experimental::optional<xhal::common::utils::Node> getNode(const char* nodeName);
#else
          boost::optional<xhal::common::utils::Node> getNode(const char* nodeName);
#endif
          /**
           * @brief not implemented
           */
#ifdef __ARM_ARCH_7A__
          std::experimental::optional<xhal::common::utils::Node> getNodeFromAddress(const uint32_t nodeAddress);
#else
          boost::optional<xhal::common::utils::Node> getNodeFromAddress(const uint32_t nodeAddress);
#endif
          /**
           * @brief return all nodes
           */
          std::unordered_map<std::string,xhal::common::utils::Node> getAllNodes();

        private:
          std::string m_xmlFile;
          log4cplus::Logger m_logger;
          std::unordered_map<std::string, int> m_vars;
          std::unordered_map<std::string,xhal::common::utils::Node>* m_nodes;
          xercesc::DOMNode* m_root;
          xercesc::DOMNode* m_node;
          xercesc::DOMNodeList* children;
          static int index;

          /**
           * @brief fills custom node object
           */
          void makeTree(xercesc::DOMNode * node, std::string basename, uint32_t baseAddress, std::unordered_map<std::string,xhal::common::utils::Node> * nodes, xhal::common::utils::Node * parentNode, std::unordered_map<std::string, int> vars, bool isGenerated);

          /**
           * @brief returns node attribute value by its name if found
           */
#ifdef __ARM_ARCH_7A__
          std::experimental::optional<std::string> getAttVal(xercesc::DOMNode * t_node, const char * attname);
#else
          boost::optional<std::string> getAttVal(xercesc::DOMNode * t_node, const char * attname);
#endif
          /**
           * @brief converts string representation of hex, binary or decimal number to an integer
           */
          unsigned int parseInt(std::string & s);

          /**
           * @brief returns cleaned from automatic generation symbols node name
           */
          std::string substituteVars(std::string & s, std::unordered_map<std::string, int> dict);

          /**
           * @brief returns a copy of original string with all occurences of first substring replaced with second substring
           */
          std::string replaceAll( std::string const& original, std::string const& from, std::string const& to );
      };
    }
  }
}

#endif  // XHAL_COMMON_UTILS_XHALXMLPARSER_H
