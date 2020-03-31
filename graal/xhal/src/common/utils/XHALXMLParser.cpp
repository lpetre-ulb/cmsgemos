#include "xhal/common/utils/XHALXMLParser.h"

#include <log4cplus/version.h>

int xhal::common::utils::XHALXMLParser::index = 0;

xhal::common::utils::XHALXMLParser::XHALXMLParser(const std::string& xmlFile)
{
  m_xmlFile = xmlFile;
  log4cplus::SharedAppenderPtr myAppender(new log4cplus::ConsoleAppender());
#if LOG4CPLUS_VERSION < LOG4CPLUS_MAKE_VERSION(2, 0, 0)
  myAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::TTCCLayout()));
#else
  myAppender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::TTCCLayout()));
#endif
  auto t_logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("XHALXMLParser_" + std::to_string(index)));
  ++index;
  m_logger = t_logger;
  m_logger.addAppender(myAppender);
  m_logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
  m_nodes = new std::unordered_map<std::string,Node>();
}

xhal::common::utils::XHALXMLParser::~XHALXMLParser()
{
  if (m_nodes)
    delete m_nodes;
  m_logger.shutdown();
}

void xhal::common::utils::XHALXMLParser::setLogLevel(int loglevel)
{
  switch (loglevel) {
  case 0:
    m_logger.setLogLevel(log4cplus::ERROR_LOG_LEVEL);
    break;
  case 1:
    m_logger.setLogLevel(log4cplus::WARN_LOG_LEVEL);
    break;
  case 2:
    m_logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
    break;
  case 3:
    m_logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
    break;
  case 4:
    m_logger.setLogLevel(log4cplus::TRACE_LOG_LEVEL);
    break;
  }
}

void xhal::common::utils::XHALXMLParser::parseXML()
{
  //
  /// Initialize XML4C system
  try {
    xercesc::XMLPlatformUtils::Initialize();
    XHAL_INFO("Successfully initialized XML4C system");
  } catch (const xercesc::XMLException& toCatch) {
    XHAL_ERROR("Error during Xerces-c Initialization." << std::endl
               << "  Exception message:"
               << xercesc::XMLString::transcode(toCatch.getMessage()));
    throw xhal::common::utils::XHALXMLParserException("XHALParser: initialization failed");
    return;
  }

  //  Create our parser, then attach an error handler to the parser.
  //  The parser will call back to methods of the ErrorHandler if it
  //  discovers errors during the course of parsing the XML document.

  XHAL_TRACE("Call XMLCh");
  XMLCh tempStr[100];
  XHAL_TRACE("XMLCh created");
  xercesc::XMLString::transcode("LS", tempStr, 99);
  XHAL_TRACE("XMLString transcode called");
  xercesc::DOMImplementation *impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
  XHAL_TRACE("DOM implementation created");
  xercesc::DOMLSParser       *parser = ((xercesc::DOMImplementationLS*)impl)->createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
  XHAL_TRACE("DOMLSParser created");
  xercesc::DOMConfiguration  *config = parser->getDomConfig();
  XHAL_TRACE("DOM configuration created");
  xercesc::DOMDocument *doc;
  XHAL_DEBUG("Xerces parser created ");

  config->setParameter(xercesc::XMLUni::fgDOMNamespaces, true);
  config->setParameter(xercesc::XMLUni::fgXercesSchema, false);
  config->setParameter(xercesc::XMLUni::fgXercesHandleMultipleImports, true);
  config->setParameter(xercesc::XMLUni::fgXercesSchemaFullChecking, false);

  if (config->canSetParameter(xercesc::XMLUni::fgXercesDoXInclude, true)) {
    config->setParameter(xercesc::XMLUni::fgXercesDoXInclude, true);
    XHAL_DEBUG("Xerces parser set to do XInclude ");
  }

  XHAL_DEBUG("Xerces parser tuned up ");

  //  Parse the XML file, catching any XML exceptions that might propogate
  //  out of it.
  //
  bool errorsOccured = false;
  try {
    parser->resetDocumentPool();
    doc = parser->parseURI(m_xmlFile.c_str());

  } catch (const xercesc::XMLException& e) {
    XHAL_ERROR("An error occured during parsing" << std::endl
               << "   Message: "
               << xercesc::XMLString::transcode(e.getMessage()));
    errorsOccured = true;
    // fileError = "An error occured during parsing of selected file. Please select another configuration file.";
  } catch (const xercesc::DOMException& e) {
    XHAL_ERROR("An error occured during parsing" << std::endl
               << "   Message: "
               << xercesc::XMLString::transcode(e.msg));
    errorsOccured = true;
    // fileError = "An error occured during parsing of selected file. Please select another configuration file.";
  } catch (...) {
    XHAL_ERROR("An error occured during parsing");
    errorsOccured = true;
    // fileError = "An error occured during parsing of selected file. Please select another configuration file.";
  }

  if (!errorsOccured) {
    XHAL_DEBUG("DOM tree created succesfully");
    if (doc->getDocumentElement()!=NULL) {
      m_root = doc->getDocumentElement();
    }
    XHAL_DEBUG("Root node (getDocumentElement) obtained");
    makeTree(m_root,"",0x0,m_nodes,NULL,m_vars,false);
    XHAL_DEBUG("Number of nodes: " << m_nodes->size());
  } else {
    throw xhal::common::utils::XHALXMLParserException("XHALParser: an error occured during parsing");
  }
  XHAL_DEBUG("Parsing done!");
  if (parser) parser->release();
  xercesc::XMLPlatformUtils::Terminate();
}

void xhal::common::utils::XHALXMLParser::makeTree(xercesc::DOMNode * node, std::string baseName, uint32_t baseAddress, std::unordered_map<std::string, Node> * nodes, Node * parentNode, std::unordered_map<std::string, int> vars, bool isGenerated)
{
  XHAL_DEBUG("Call makeTree");
  unsigned int generateSize;
  unsigned int generateAddressStep;
  std::string generateIdxVar;
  XHAL_DEBUG("Declare some local variables");
  if (isGenerated == false) {
    XHAL_DEBUG("Node is not _generated_");
    if (auto tmp = getAttVal(node, "generate")) {
      if (*tmp == "true") {
        XHAL_DEBUG("Generate nodes");
        tmp = getAttVal(node, "generate_size");
        generateSize = parseInt(*tmp);
        tmp = getAttVal(node, "generate_address_step");
        generateAddressStep = parseInt(*tmp);
        auto generateIdxVar = getAttVal(node, "generate_idx_var");
        for (unsigned int i = 0; i < generateSize; i++) {// in range(0, generateSize):
          vars[*generateIdxVar] = i;
          makeTree(node, baseName, baseAddress + generateAddressStep * i, nodes, parentNode, vars, true);
        }
        return;
      }
    }
  } else {
    XHAL_DEBUG("Node is _generated_");
  }
  Node newNode = Node();
  XHAL_DEBUG("Create Node() object");
  std::string name;
  uint32_t address;
  name = baseName;
  address = baseAddress;
  if (baseName != "") name += ".";
  if (auto tmp = getAttVal(node, "id")) {
    name.append(*tmp);
  } else {
    XHAL_ERROR("getAttVal returned NONE, node has no id attribute");
  }
  //name += *getAttVal(node, "id");
  name = substituteVars(name, vars);
  XHAL_DEBUG("Node name: " << name);
  newNode.name = name;
  if (auto tmp = getAttVal(node, "description")) {
    newNode.description = *tmp;
  }
  if (auto tmp = getAttVal(node, "address")) {
    address = baseAddress + parseInt(*tmp);
    newNode.address = address;
    newNode.real_address = (address<<2)+0x64000000;
  } else {
    XHAL_TRACE("getAttVal returned NONE");
  }
  if (auto tmp = getAttVal(node, "permission")) {
    newNode.permission = *tmp;
  }
  if (auto tmp = getAttVal(node, "mode")) {
    newNode.mode = *tmp;
  }
  if (auto tmp = getAttVal(node, "size")) {
    newNode.size = parseInt(*tmp);
  }
  if (auto tmp = getAttVal(node, "mask")) {
    newNode.mask = parseInt(*tmp);
  }
  newNode.isModule = (getAttVal(node, "fw_is_module")) && (*getAttVal(node, "fw_is_module") == "true");
  if (auto tmp = getAttVal(node, "sw_monitor_warn_min_threshold")) {
    newNode.warn_min_value = parseInt(*tmp);
  }
  if (auto tmp = getAttVal(node, "sw_monitor_error_min_threshold")) {
    newNode.warn_min_value = parseInt(*tmp);
  }

  nodes->insert(std::make_pair(newNode.name,newNode));
  XHAL_TRACE("Node map size after insert " << nodes->size());
  if (parentNode) {
    parentNode->addChild(newNode);
    newNode.parent = parentNode;
    newNode.level = parentNode->level+1;
  }
  xercesc::DOMNodeList *children_ = node->getChildNodes();
  const XMLSize_t nodeCount = children_->getLength();
  XHAL_DEBUG("Node children length: " << nodeCount);

  for (XMLSize_t ix = 0 ; ix < nodeCount ; ++ix) {
    if (children_->item(ix)->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      makeTree(children_->item(ix),name,address,nodes,&newNode,vars,false);
    } else {
      continue;
    }
  }
}

#ifdef __ARM_ARCH_7A__
std::experimental::optional<std::string> xhal::common::utils::XHALXMLParser::getAttVal(xercesc::DOMNode * t_node_, const char * attname)
#else
  boost::optional<std::string> xhal::common::utils::XHALXMLParser::getAttVal(xercesc::DOMNode * t_node_, const char * attname)
#endif
{
  XHAL_TRACE("Call getAttVal for attribute " << attname);
  XMLCh * tmp = xercesc::XMLString::transcode(attname);
  xercesc::DOMElement* t_node = static_cast<xercesc::DOMElement*>(t_node_);
  XHAL_TRACE("tmp: " << tmp);
  XHAL_TRACE("successfull call of getAttribute: " << t_node->getAttribute(tmp));
  char * tmp2 = xercesc::XMLString::transcode(t_node->getAttribute(tmp));
  XHAL_TRACE("tmp2: " << tmp2);
  if (tmp2[0]!='\0') {
    std::string value = tmp2;
    xercesc::XMLString::release(&tmp);
    xercesc::XMLString::release(&tmp2);
    XHAL_TRACE("result " << value);
    return value;
  } else {
    XHAL_TRACE("Attribute not found");
    xercesc::XMLString::release(&tmp);
    xercesc::XMLString::release(&tmp2);
    return {};
  }
}

unsigned int xhal::common::utils::XHALXMLParser::parseInt(std::string & s)
{
  XHAL_TRACE("Call parseInt for argument " << s);
  std::stringstream converter(s);
  uint32_t value;
  if (s.find("0x") != std::string::npos) {
    converter >> std::hex >> value;
    XHAL_TRACE("result " << value);
    return value;
  } else if (s.find("0b") != std::string::npos) {
    XHAL_TRACE("result " << std::stoi(s,nullptr,2));
    return std::stoi(s,nullptr,2);
  } else {
    converter >> std::dec >> value;
    XHAL_TRACE("result " << value);
    return value;
  }
}

std::string xhal::common::utils::XHALXMLParser::substituteVars(std::string & s, std::unordered_map<std::string, int> dict)
{
  if (s == "")
    return s;

  std::string ret;
  ret = s;
  for (auto& p: dict) {
    ret = replaceAll(ret,"${" + p.first + "}",std::to_string(p.second));
  }
  return ret;
}

std::string xhal::common::utils::XHALXMLParser::replaceAll( std::string const& original, std::string const& from, std::string const& to )
{
  std::string results;
  std::string::const_iterator end = original.end();
  std::string::const_iterator current = original.begin();
  std::string::const_iterator next = std::search( current, end, from.begin(), from.end() );
  while ( next != end ) {
    results.append( current, next );
    results.append( to );
    current = next + from.size();
    next = std::search( current, end, from.begin(), from.end() );
  }
  results.append( current, next );
  return results;
}

#ifdef __ARM_ARCH_7A__
std::experimental::optional<xhal::common::utils::Node> xhal::common::utils::XHALXMLParser::getNode(const char* nodeName)
#else
  boost::optional<xhal::common::utils::Node> xhal::common::utils::XHALXMLParser::getNode(const char* nodeName)
#endif
{
  XHAL_DEBUG("Call getNode for argument " << nodeName);
  //Node * res = NULL;
  XHAL_TRACE("Searching unordered map");
  auto search = m_nodes->find(nodeName);
  if (search!=m_nodes->end()) {
    return search->second;
  } else {
    return {};
  }
}

// Not used a.t.m. Do we need it? FIXME
#ifdef __ARM_ARCH_7A__
std::experimental::optional<xhal::common::utils::Node> xhal::common::utils::XHALXMLParser::getNodeFromAddress(const uint32_t nodeAddress)
#else
  boost::optional<xhal::common::utils::Node> xhal::common::utils::XHALXMLParser::getNodeFromAddress(const uint32_t nodeAddress)
#endif
{
  //Node * res = NULL;
  //for (auto & n: *m_nodes) {
  //  if (nodeAddress == n.real_address) {
  //    res = &n;
  //    break;
  //  }
  //}
  //if (res) {
  //  return *res;
  //} else {
  //  return {};
  //}

}

std::unordered_map<std::string,xhal::common::utils::Node> xhal::common::utils::XHALXMLParser::getAllNodes()
{
  return *m_nodes;
}
