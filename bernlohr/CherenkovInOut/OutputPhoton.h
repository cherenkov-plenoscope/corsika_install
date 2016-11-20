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

struct OutputPhoton bunch2photon(
   struct PhotonBunch* bunch, 
   struct CherenkovInOut* cerio, 
   struct DetectorSphere* detector
) {
   struct OutputPhoton photon;
   photon.x = (short)((bunch->x/260.0e2)*MAXSHORT);
   photon.y = (short)((bunch->y/260.0e2)*MAXSHORT);
   photon.cx = (short)(bunch->cx*MAXSHORT);
   photon.cy = (short)(bunch->cy*MAXSHORT);
   photon.wavelength = (short)(bunch->wavelength*MAXSHORT);
   photon.arrival_time = bunch->arrival_time - 
           detector->z*sqrt(1.+bunch->cx*bunch->cx+bunch->cy*bunch->cy)/cerio->speed_of_light_in_air_on_observation_level - cerio->time_offset;


   return photon;
}

#endif // __OutputPhoton_H_INCLUDED__ 