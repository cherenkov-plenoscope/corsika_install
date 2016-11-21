#ifndef __CherenkovInOutPhoton_H_INCLUDED__
#define __CherenkovInOutPhoton_H_INCLUDED__

#include <math.h>

//-------------------- Photon --------------------------------------------------
struct Photon{
   short x;
   short y;
   short cx;
   short cy;
   short arrival_time;
   short wavelength;
   short emission_altitude;
   short mother;
};

const double MAXSHORT = 32768.0;

short compress_position_to_short(const double pos) {
   return (short)((pos/260.0e2)*MAXSHORT);
}

short compress_slope_to_short(const double cx) {
   return (short)(cx*MAXSHORT);
}

short compress_emission_altitude_to_short(double alt) {
   alt = fabs(alt);
   return (short)((alt/100000.0e2)*MAXSHORT);
}

short compress_mother_to_short(const double mass, const double charge) {
   return 1;
}

struct Photon bunch2photon(
   struct PhotonBunch* bunch, 
   struct CherenkovInOut* cerio, 
   struct DetectorSphere* detector
) {
   const double time_offset = cerio->time_offset;
   const double speed_of_light = cerio->speed_of_light_in_air_on_observation_level;

   struct Photon photon;
   photon.x = compress_position_to_short(bunch->x);
   photon.y = compress_position_to_short(bunch->y);
   photon.cx = compress_slope_to_short(bunch->cx);
   photon.cy = compress_slope_to_short(bunch->cy);
   photon.wavelength = (short)(bunch->wavelength*MAXSHORT);
   photon.arrival_time = bunch->arrival_time - 
           detector->z*sqrt(1.+bunch->cx*bunch->cx+bunch->cy*bunch->cy)/speed_of_light - time_offset;

   photon.emission_altitude = compress_emission_altitude_to_short(bunch->emission_altitude);

   return photon;
}

#endif // __CherenkovInOutPhoton_H_INCLUDED__ 