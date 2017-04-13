
#ifndef __sflight_fdm_FileOutput_H__
#define __sflight_fdm_FileOutput_H__

#include "sflight/fdm/modules/Module.hpp"

#include <fstream>

namespace sflight {
namespace xml { class Node; }
namespace fdm {
class Player;

//------------------------------------------------------------------------------
// Class: FileOutput
//------------------------------------------------------------------------------
class FileOutput : public Module
{
public:
   FileOutput(Player*, const double frameRate);
   ~FileOutput();

   // module interface
   virtual void initialize(xml::Node*) override;
   virtual void update(const double timestep) override;

   void update();
   void close();

   std::ofstream fout;
   int rate {};
   double lastTime {};
   int frameCounter {};
};

}
}

#endif
