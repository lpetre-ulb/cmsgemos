/**
 * @file Node.h
 * Flat representation of firmware registers with certain attributes extracted from XML address table
 *
 * @author Mykhailo Dalchenko
 * @version 1.0
 */

#ifndef XHAL_COMMON_UTILS_NODE_H
#define XHAL_COMMON_UTILS_NODE_H

#include <iostream>
#include <vector>

namespace xhal {
  namespace common {
    namespace utils {
      /**
       * @class Node
       * @brief stores single XML node with its attributes
       *
       * Note that all the class members are public in order to avoid extra ambiguitization of the code with getters and setters
       */
      class Node
      {
        public:
          /**
           * @brief Default constructor. Creates empty Node.
           */
          Node()
          {
            name="";
            description="";
            vhdlname = "";
            address = 0x0;
            real_address = 0x0;
            permission = "";
            mode = "single";
            size = 1;
            mask = 0xFFFFFFFF;
            isModule = false;
            parent = nullptr;
            level = 0;
            warn_min_value = -1;
            error_min_value = -1;
          }

          ~Node() {}

          /**
           * @brief Adds child node
           */
          void addChild(Node child) {children.push_back(child);}

          /**
           * @brief Returns VHDL node name
           */
          std::string getVhdlName() {return vhdlname;}

          /**
           * @brief Not implemented
           */
          void output()
          {
            //!TODO
            //print 'Name:',self.name
            //print 'Description:',self.description
            //print 'Address:','{0:#010x}'.format(self.address)
            //print 'Permission:',self.permission
            //print 'Mode:',self.mode
            //print 'Size:',self.size
            //print 'Mask:','{0:#010x}'.format(self.mask)
            //print 'Module:',self.isModule
            //print 'Parent:',self.parent.name
          }

          /**
           * @brief Returns all hierarchy of chlid nodes
           * @param node parent node
           * @param kids vector of nodes, must be empty when function called and will be updated with node childrem
           */
          void getAllChildren(Node node, std::vector<Node> kids)
          {
            if (node.children.empty()) {
              kids.push_back(node);
            } else {
              for (auto const& child: node.children) {
                getAllChildren(child, kids);
              }
            }
          }

          std::string name;
          std::string description;
          std::string vhdlname;
          uint32_t address;
          uint32_t real_address;
          std::string permission;
          std::string mode;
          uint32_t size;
          uint32_t mask;
          bool isModule;
          Node *parent;
          std::vector<Node> children;
          int level;
          int warn_min_value;
          int error_min_value;
      };
    }
  }
}

#endif // XHAL_COMMON_UTILS_NODE_H
