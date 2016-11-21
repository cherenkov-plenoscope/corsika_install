#ifndef __PhotonBunch_H_INCLUDED__
#define __PhotonBunch_H_INCLUDED__

//-------------------- PhotonBunch ---------------------------------------------
struct PhotonBunch{
   float size;
   float x;
   float y;
   float cx;
   float cy;
   float arrival_time;
   float emission_altitude;
   float wavelength;
   float mother_mass;
   float mother_charge;
};

double PhotonBunch_cz(struct PhotonBunch* bunch) {
   return sqrt(1.0 - bunch->cx*bunch->cx - bunch->cy*bunch->cy);
}

double PhotonBunch_slope_x(struct PhotonBunch* bunch) {
   return bunch->cx/PhotonBunch_cz(bunch);
}

double PhotonBunch_slope_y(struct PhotonBunch* bunch) {
   return bunch->cy/PhotonBunch_cz(bunch);
}

void PhotonBunch_to_string(struct PhotonBunch* bunch, char* out) {
   char size_str[1024];
   sprintf(size_str, "%f", bunch->size);

   char x_str[1024];
   sprintf(x_str, "%f", bunch->x);

   char y_str[1024];
   sprintf(y_str, "%f", bunch->y);

   char cx_str[1024];
   sprintf(cx_str, "%f", bunch->cx);

   char cy_str[1024];
   sprintf(cy_str, "%f", bunch->cy);

   char arrival_time_str[1024];
   sprintf(arrival_time_str, "%f", bunch->arrival_time);

   char emission_altitude_str[1024];
   sprintf(emission_altitude_str, "%f", bunch->emission_altitude);

   char wavelength_str[1024];
   sprintf(wavelength_str, "%f", bunch->wavelength);

   char mother_mass_str[1024];
   sprintf(mother_mass_str, "%f", bunch->mother_mass);

   char mother_charge_str[1024];
   sprintf(mother_charge_str, "%f", bunch->mother_charge);

   strcpy(out, "PhotonBunch(");
   strcat(out, "size "); strcat(out, size_str); strcat(out, ", ");
   strcat(out, "x "); strcat(out, x_str); strcat(out, "cm, ");
   strcat(out, "y "); strcat(out, y_str); strcat(out, "cm, ");
   strcat(out, "cx "); strcat(out, cx_str); strcat(out, ", ");
   strcat(out, "cy "); strcat(out, cy_str); strcat(out, ", ");
   strcat(out, "t "); strcat(out, arrival_time_str); strcat(out, "ns, ");
   strcat(out, "z0 "); strcat(out, emission_altitude_str); strcat(out, "cm, ");
   strcat(out, "lambda "); strcat(out, wavelength_str); strcat(out, "nm, ");
   strcat(out, "mother mass "); strcat(out, mother_mass_str); strcat(out, "GeV, ");
   strcat(out, "mother charge "); strcat(out, mother_charge_str);
   strcat(out, ")");
}

void PhotonBunch_warn_if_size_above_one(struct PhotonBunch* bunch) {
   if(bunch->size > 1.0) {
      char bunch_str[4096];
      PhotonBunch_to_string(bunch, bunch_str);
      fprintf(
         stderr, 
         "Warning: Photon bunch size > 1.0 in %s\n", 
         bunch_str
      );
   }
}

int PhotonBunch_reaches_observation_level(
   struct PhotonBunch* bunch,
   double random_uniform_0to1
) {
   return random_uniform_0to1 <= bunch->size;
}

#endif // __PhotonBunch_H_INCLUDED__ 