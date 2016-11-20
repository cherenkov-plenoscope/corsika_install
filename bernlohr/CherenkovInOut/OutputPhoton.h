#ifndef __OutputPhoton_H_INCLUDED__
#define __OutputPhoton_H_INCLUDED__

//-------------------- OutputPhoton --------------------------------------------
struct OutputPhoton{
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

struct OutputPhoton bunch2photon(
   struct PhotonBunch* bunch, 
   struct CherenkovInOut* cerio, 
   struct DetectorSphere* detector
) {
   struct OutputPhoton photon;
   photon.x = compress_position_to_short(bunch->x);
   photon.y = compress_position_to_short(bunch->y);
   photon.cx = compress_slope_to_short(bunch->cx);
   photon.cy = compress_slope_to_short(bunch->cy);
   photon.wavelength = (short)(bunch->wavelength*MAXSHORT);
   photon.arrival_time = bunch->arrival_time - 
           detector->z*sqrt(1.+bunch->cx*bunch->cx+bunch->cy*bunch->cy)/cerio->speed_of_light_in_air_on_observation_level - cerio->time_offset;


   return photon;
}

#endif // __OutputPhoton_H_INCLUDED__ 