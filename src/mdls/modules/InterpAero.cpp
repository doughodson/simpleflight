
#include "sflight/mdls/modules/InterpAero.hpp"

#include "sflight/mdls/modules/Atmosphere.hpp"

#include "sflight/xml/Node.hpp"
#include "sflight/xml/node_utils.hpp"

#include "sflight/mdls/Player.hpp"
#include "sflight/mdls/UnitConvert.hpp"
#include "sflight/mdls/Vector3.hpp"
#include "sflight/mdls/WindAxis.hpp"
#include "sflight/mdls/constants.hpp"

#include <cmath>
#include <iostream>

namespace sflight {
namespace mdls {

InterpAero::InterpAero(Player* player, const double frameRate)
    : Module(player, frameRate)
{
}

void InterpAero::update(const double timestep)
{
   if (player == nullptr)
      return;

   double qbar = 0.5 * player->vInf * player->vInf * player->rho * wingArea;

   double cl = a1 + a2 * player->alpha;
   double cd = b1 + b2 * cl * cl;
   double cy = -2.0 * player->beta;

   if (usingMachEffects) {
      const double beta_mach = getBetaMach(player->mach);
      cl /= beta_mach;
      cd /= beta_mach;
   }

   WindAxis::windToBody(player->aeroForce, player->alpha, player->beta, cl * qbar,
                        cd * qbar, cy * qbar);
}

void InterpAero::initialize(xml::Node* node)
{
   double thrustRatio{};
   double speedSound{};
   double beta_mach{};
   double pitch{};
   double airspeed{};
   double vs{};
   double mach{};
   double thrust{};

   xml::Node* tmp = node->getChild("Design");

   designAlt = UnitConvert::toMeters(getDouble(tmp, "DesignAltitude", 0.0));

   designWeight = UnitConvert::toNewtons(getDouble(tmp, "DesignWeight", 0.0));

   wingSpan = UnitConvert::toMeters(getDouble(tmp, "WingSpan", 6.0));
   wingArea = UnitConvert::toSqMeters(getDouble(tmp, "WingArea", 6.0));

   wingEffects = math::PI * wingSpan * wingSpan / wingArea;
   // wingEffects = 1.0;

   thrustAngle = UnitConvert::toRads(getDouble(tmp, "ThrustAngle", 0.0));

   thrustRatio = getDouble(tmp, "ThrustToWeight", 0.0) * designWeight;
   speedSound = Atmosphere::getSpeedSound(Atmosphere::getTemp(designAlt));

   // cruise condition
   pitch = UnitConvert::toRads(getDouble(tmp, "CruiseCondition/Pitch", 0));
   airspeed = UnitConvert::toMPS(getDouble(tmp, "CruiseCondition/Airspeed", 0.0));
   vs = UnitConvert::FPMtoMPS(getDouble(tmp, "CruiseCondition/VS", 0.0));
   if (airspeed < 1E-6) {
      airspeed = getDouble(tmp, "CruiseCondition/Mach", 0) * speedSound;
   }

   thrust = UnitConvert::toNewtons(getDouble(tmp, "CruiseCondition/Thrust", 0));
   if (thrust < 1E-6)
      thrust = getDouble(tmp, "CruiseCondition/Throttle", 0) * thrustRatio;

   createCoefs(pitch, thrust, vs, airspeed, cruiseAlpha, cruiseCL, cruiseCD);

   if (usingMachEffects) {
      mach = airspeed / speedSound;
      beta_mach = getBetaMach(mach);
      cruiseCL *= beta_mach;
      cruiseCD *= beta_mach;
   }

   // climb condition
   pitch = UnitConvert::toRads(getDouble(tmp, "ClimbCondition/Pitch", 0));
   airspeed = UnitConvert::toMPS(getDouble(tmp, "ClimbCondition/Airspeed", 0.0));
   vs = UnitConvert::FPMtoMPS(getDouble(tmp, "ClimbCondition/VS", 0.0));
   if (airspeed < 1E-6) {
      airspeed = getDouble(tmp, "ClimbCondition/Mach", 0) * speedSound;
   }
   mach = airspeed / speedSound;

   thrust = UnitConvert::toNewtons(getDouble(tmp, "ClimbCondition/Thrust", 0));
   if (thrust < 1E-6)
      thrust = getDouble(tmp, "ClimbCondition/Throttle", 0) * thrustRatio;

   createCoefs(pitch, thrust, vs, airspeed, climbAlpha, climbCL, climbCD);

   if (usingMachEffects) {
      mach = airspeed / speedSound;
      beta_mach = getBetaMach(mach);
      climbCL *= beta_mach;
      climbCD *= beta_mach;
   }

   a2 = (cruiseCL - climbCL) / (cruiseAlpha - climbAlpha);
   a1 = climbCL - a2 * climbAlpha;

   b2 = (cruiseCD - climbCD) / (cruiseCL * cruiseCL - climbCL * climbCL);
   b1 = climbCD - b2 * climbCL * climbCL;
}

void InterpAero::createCoefs(double theta, double thrust, double vz, double u, double& alpha,
                             double& cl, double& cd)
{
   vz = -vz;

   // in this version, thrust is always horizontal
   double thrustAngle = 0;

   double w = (vz + u * std::sin(theta)) * std::cos(theta);

   double vInf = std::sqrt(w * w + u * u);

   alpha = std::atan2(w, u);

   double q = 0.5 * Atmosphere::getRho(designAlt) * vInf * vInf;

   double zforce = (-thrust * std::sin(thrustAngle) + designWeight * std::cos(theta)) / q;
   double xforce = (thrust * std::cos(thrustAngle) - designWeight * std::sin(theta)) / q;

   Vector3 aero = Vector3();
   WindAxis::bodyToWind(aero, alpha, 0., xforce, 0., zforce);

   cl = -aero.get1() / wingArea;
   cd = -aero.get2() / wingArea;
}

double InterpAero::getBetaMach(double mach)
{
   // if (mach > 1.02 ) return 1 + sqrt( mach * mach - 1.0);
   if (mach > 1.05)
      mach = 1.0 / mach;
   if (mach > 0.95)
      mach = 0.95;
   return std::sqrt(1.0 - mach * mach);
}
}
}