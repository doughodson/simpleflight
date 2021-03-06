
#include "sflight/xml/parser_utils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstdlib>

namespace sflight {
namespace xml {

// parse a given filename
Node* parse(const std::string& filename)
{
   std::ifstream fin(filename, std::ifstream::in);

   if (fin) {
      std::cout << "Opened configuration file : " << filename << std::endl;
   } else {
      std::cerr << "Could not open file, exiting..." << filename << std::endl;
      std::exit(1);
   }

   Node* node{parse(fin)};
   fin.close();
   return node;
}

// parse input stream
Node* parse(std::istream& ifs)
{
   Node* rootNode{};
   Node* node{};

   while (true) {
      std::string chunk{readChunk(ifs)};
//      std::cout << "Chuck read : " << chunk << std::endl
//                << "----------------------------------------------------------"
//                << std::endl;

      if (chunk == "") break;

      // declaration
      if (startsWith(chunk, "<?")) {
         while (!endsWith(chunk, "?")) {
            chunk += readChunk(ifs);
         }
         continue;
      }

      // comment
      if (startsWith(chunk, "<!--")) {
         while (!endsWith(chunk, "--")) {
            chunk += readChunk(ifs);
         }
         continue;
      }

      // instruction
      if (startsWith(chunk, "<!")) {
         continue;
      }

      // cdata node
      if (startsWith(chunk, "<![CDATA[")) {
         while (!endsWith(chunk, "]]")) {
            chunk += readChunk(ifs);
         }
         continue;
      }

      // if this is a text node or close tag, set the element text
      std::size_t i{chunk.find("</")};
      if (i != std::string::npos && node != 0) {
         node->setText(chunk.substr(0, i));
         chunk = chunk.substr(i + 1);

         if ((chunk.substr(1) == node->getTagName()) && node->getParent() != 0) {
            node = node->getParent();
         }

         continue;
      }

      // regular element node (nodes that interest us)
      if (startsWith(chunk, "<")) {

         chunk = chunk.substr(1);       // remove "<" from beginning of chunk
         if (!rootNode) {               // start building a Node tree
            rootNode = new Node("");
            node = rootNode;
         } else {
            Node* tmpNode{node->addChild("")};
            node = tmpNode;
         }

         std::size_t splitPt = chunk.find(" ");
         if (splitPt != std::string::npos) {
            std::string tag{chunk.substr(0, splitPt)};
            node->setTagName(tag);
            putAttributes(chunk.substr(splitPt), node);
         } else {
            node->setTagName(chunk);
         }

         if (chunk.rfind("/") == chunk.length() - 1 && node->getParent() != 0) {
            node = node->getParent();
         }
      }
   }

   return rootNode;
}

std::string readChunk(std::istream& ifs)
{
   std::string buf;
   try {
      int ch{ifs.get()};
      while (ch != '>') {
         if (ch == -1) {
            if (buf.length() > 0) {
               throw 0;
            }
            return "";
         }
         if (!isWhitespace(static_cast<char>(ch)) || buf.length() > 0) {
            buf += static_cast<char>(ch);
         }
         ch = ifs.get();
      }
   } catch (int) {
      return "";
   }

   return buf;
}

// test to see if char is a whitespace
bool isWhitespace(const char x)
{
   if (x == '\t' || x == '\n' || x == '\r' || x == ' ') {
      return true;
   }
   return false;
}

std::string putAttributes(const std::string& x, Node* const node)
{
   std::string str(x);

   try {
      while (str.length() > 0) {
         while (isWhitespace(str[0])) {
            str = str.substr(1, str.length() - 1);
         }
         const std::size_t nameEnd{str.find("=\"", 0)};
         if (nameEnd == std::string::npos) {
            return str;
         }

         const std::size_t attrEnd = str.find("\"", nameEnd + 3);
         if (attrEnd == std::string::npos) {
            throw 1;
         }

         const std::string name{str.substr(0, nameEnd)};
         const std::string attr{str.substr(nameEnd + 2, attrEnd - nameEnd - 2)};

         // subChars( name );
         // subChars( attr );
//         if (treatAsChildren) {
            Node* tmp{node->addChild(name)};
            tmp->setText(attr);
//         } else {
//            node->putAttribute(name, attr);
//         }
         str = str.substr(attrEnd + 1);
      }
   } catch (int) {
      std::cerr << "error" << std::endl;
   }
   return str;
}

bool startsWith(const std::string& str, const std::string& search)
{
   return (str.find(search, 0) == 0);
}

bool endsWith(const std::string& str, const std::string& search)
{
   const std::size_t searchLimit{str.length() - search.length()};
   return (str.rfind(search, searchLimit) == searchLimit);
}
}

}
