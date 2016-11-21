#ifndef __CherenkovInOutPhoton_H_INCLUDED__
#define __CherenkovInOutPhoton_H_INCLUDED__

#include <math.h>

//-------------------- Photon --------------------------------------------------
short round_to_nearest_int(const float number) {
   return number > 0 ? (short)(number + 0.5) : (short)(number - 0.5);
}

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

const float MAXSHORT = 32768.0;

short compress_position_to_short(const float pos) {
   return round_to_nearest_int((pos/260.0e2)*MAXSHORT);
}

short compress_slope_to_short(const float cx) {
   return (short)(cx*MAXSHORT);
}

short compress_emission_altitude_to_short(float alt) {
   alt = fabs(alt);
   return round_to_nearest_int((alt/100000.0e2)*MAXSHORT);
}

short compress_wavelength_to_short(float wavelength) {

}

short compress_mother_to_short(const float mass, const float charge) {
   return 1;
}

void Photon_init_from_bunch(struct Photon* photon, const struct Bunch* bunch) {
   photon->x = compress_position_to_short(bunch->x);
   photon->y = compress_position_to_short(bunch->y);
   photon->cx = compress_slope_to_short(bunch->cx);
   photon->cy = compress_slope_to_short(bunch->cy);
   photon->wavelength = (short)(bunch->wavelength*MAXSHORT);
}


/*void Photon_init_from_bunch(
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
}*/
/*
cbunch = &det->cbunch[det->next_bunch];
//@ The bunch size has a limited range of up to 327 photons.
cbunch->photons = (short)(100.*photons+0.5);
//@ Positions have a limited range of up to 32.7 m from the detector centre
cbunch->x       = (short)Nint(10.*(x - sx*det->z0 - det->x0));
cbunch->y       = (short)Nint(10.*(y - sy*det->z0 - det->y0));
//@ No limits in the direction (accuracy 7 arcsec for vertical showers)
cbunch->cx      = (short)Nint(30000.*cx);
cbunch->cy      = (short)Nint(30000.*cy);
//@ The time has a limited range of +-3.27 microseconds.
cbunch->ctime   = (short)Nint(10.*(ctime - 
        det->z0*sqrt(1.+sx*sx+sy*sy)/airlightspeed - toffset));
cbunch->log_zem = (short)(1000.*log10(zem)+0.5);
if ( lambda == 0. )     // Unspecified wavelength of photons
   cbunch->lambda = (short)0;
else if ( lambda < 0. ) // Photo-electrons instead of photons
   cbunch->lambda = (short)(lambda-0.5);
else                    // Photons of specific wavelength
   cbunch->lambda = (short)(lambda+0.5);
det->next_bunch++;
*/

#endif // __CherenkovInOutPhoton_H_INCLUDED__ 