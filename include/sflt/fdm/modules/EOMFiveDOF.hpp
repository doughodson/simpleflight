
#ifndef __sflt_fdm_EOMFiveDOF_H__
#define __sflt_fdm_EOMFiveDOF_H__

#include "sflt/fdm/modules/FDMModule.hpp"

#include "sflt/fdm/Quaternion.hpp"
#include "sflt/fdm/Vector3.hpp"

namespace sflt {
namespace xml { class Node; }
namespace fdm {
class FDMGlobals;

//------------------------------------------------------------------------------
// Class: EOMFiveDOF
// Description: Implements psuedo five DOF dynamics for fixed wing aircraft
//              (imposes a no-slip condition)
//------------------------------------------------------------------------------
class EOMFiveDOF : public FDMModule
{
public:
   EOMFiveDOF(FDMGlobals* globals, double frameRate);
   virtual ~EOMFiveDOF() = default;

   // module interface
   virtual void initialize(xml::Node* node) override;
   virtual void update(const double timestep) override;

   void computeEOM(double timestep);

private:
   Quaternion quat;
   Quaternion qdot;
   Vector3 forces;
   Vector3 uvw;
   Vector3 pqr;
   Vector3 xyz;
   Vector3 gravAccel;
   double gravConst {};
   bool autoRudder {};
};

}
}

#endif