
#ifndef __sflight_fdm_FileOutput_H__
#define __sflight_fdm_FileOutput_H__

#include "sflight/fdm/modules/Module.hpp"

#include <fstream>

namespace sflight {
namespace xml { class Node; }
namespace fdm {
class FDMGlobals;

//------------------------------------------------------------------------------
// Class: FileOutput
//------------------------------------------------------------------------------
class FileOutput : public Module
{
public:
   FileOutput(FDMGlobals* globals, const double frameRate);
   ~FileOutput();

   // module interface
   virtual void initialize(xml::Node* node) override;
   virtual void update(const double timestep) override;

   void update();
   void close();

   //FDMGlobals *globals;
   std::ofstream fout;
   int rate {};
   double lastTime {};
   int frameCounter {};
};

}
}

#endif
