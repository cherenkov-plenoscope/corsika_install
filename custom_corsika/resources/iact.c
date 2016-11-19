/*
   Sebastian Achim Mueller, ETH Zurich 2016

   This file originated from the iact.c file which is part of the IACT/atmo
   package for CORSIKA by Konrad Bernloehr.

   The IACT/atmo package is free software; you can redistribute it 
   and/or modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This package is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this package. If not, see <http://www.gnu.org/licenses/>.
 */

#define PRMPAR_SIZE 17

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>


typedef float cors_real_t;
typedef double cors_real_now_t;
typedef double cors_real_dbl_t;
typedef double cors_dbl_t;

/* =============================================================== */
/* FORTRAN called functions                                        */
/* The additional character string lengths for name in telfil_ and */
/* line in tellni_ are not used because compiler-dependent.        */
void telfil_(char *name);
void telsmp_(char *name);
void telshw_(void);
void telinf_(
   int *itel, 
   double *x, 
   double *y, 
   double *z, 
   double *r, 
   int *exists);
void tellni_(char *line, int *llength); 
void telrnh_(cors_real_t runh[273]);
void telrne_(cors_real_t rune[273]);
void telasu_(
   int *n, 
   cors_real_dbl_t *dx, 
   cors_real_dbl_t *dy);
void telset_(
   cors_real_now_t *x, 
   cors_real_now_t *y, 
   cors_real_now_t *z, 
   cors_real_now_t *r);
void televt_(
   cors_real_t evth[273], 
   cors_real_dbl_t prmpar[PRMPAR_SIZE]);
int telout_(
   cors_real_now_t *bsize, 
   cors_real_now_t *wt, 
   cors_real_now_t *px, 
   cors_real_now_t *py, 
   cors_real_now_t *pu, 
   cors_real_now_t *pv, 
   cors_real_now_t *ctime, 
   cors_real_now_t *zem, 
   cors_real_now_t *lambda, 
   double *temis, 
   double *penergy, 
   double *amass, 
   double *charge);
void telprt_ (cors_real_t* datab, int *maxbuf);
void tellng_ (
   int *type, 
   double *data, 
   int *ndim, 
   int *np, 
   int *nthick, 
   double *thickstep);
void telend_(cors_real_t evte[273]);

void extprm_ (
   cors_real_dbl_t *type, 
   cors_real_dbl_t *eprim,
   double *thetap, 
   double *phip);


/* =============================================================== */
/* CORSIKA function called from this module: */
extern double heigh_ (double *thickness);
extern double refidx_ (double *height);

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

//-------------------- CerIo ---------------------------------------------------
const double VACUUM_SPEED_OF_LIGHT = 29.9792458;

struct CerIo {
   char output_path [1024];
   char runh_path [1024];
   char evth_template_path [1024];
   char photon_block_template_path [1024];
   char readme_path [1024];
   double observation_level;
   double speed_of_light_in_air_on_observation_level;
   double time_offset;
   FILE *current_photons;
};

void CerIo_init(struct CerIo* sane, char* output_path) {
   sane->current_photons = NULL;
   strcpy(sane->output_path, output_path);

   strcpy(sane->runh_path, sane->output_path);
   strcat(sane->runh_path, ".runh");

   strcpy(sane->evth_template_path, sane->output_path);
   strcat(sane->evth_template_path, ".evth.");

   strcpy(sane->photon_block_template_path, sane->output_path);
   strcat(sane->photon_block_template_path, ".bunches.");

   strcpy(sane->readme_path, sane->output_path);
   strcat(sane->readme_path, ".README.md");

   sane->observation_level = -1.0;
   sane->speed_of_light_in_air_on_observation_level = VACUUM_SPEED_OF_LIGHT;
   sane->time_offset = 0.0;
}

void CerIo_assert_file_is_open(struct CerIo* sane, FILE* file, char* name) {
   if(file == NULL) {
      char message [2048];
      strcpy(message, "CerIo: unable to open specific file: '");
      strcat(message, name);
      strcat(message, "'\n");
      fputs(message, stderr);
      exit(1);      
   }
}

void CerIo_write_readme(struct CerIo* sane) {
   char readme[] = 
   "Cherenkov I/O\n"
   "-------------\n"
   "\n"
   ".runh\n"
   "-----\n"
   "     float32 array [273 x 1], CORSIKA run header.\n"
   "\n"
   ".evth.XXX\n"
   "---------\n"
   "     float32 array [273 x 1], CORSIKA event header of event XXX.\n"
   "\n"
   ".bunches.XXX\n"
   "------------\n"
   "     float32 array [number_of_bunches x 10], Photon bunches of event XXX.\n"
   "\n"
   "Photon bunch\n"
   "------------\n"
   "     float32  size  bunch size (number of )\n"
   "     float32  x     incident position on observation plane\n"
   "     float32  y     incident position on observation plane\n"
   "     float32  cx    incident angel relative to surface normal of observation plane\n"
   "     float32  cy    incident angel relative to surface normal of observation plane\n"
   "     float32  arrival time on observation plane, relative to primary's first interaction\n"
   "     float32  emission altitude above sea level (not above observation plane)\n"
   "     float32  wavelength in nano meter\n"
   "     float32  mother particle mass in GeV\n"
   "     float32  mother particle electric charge\n"
   "\n"
   "     Total bunch size is 10 x 4 bytes = 40 bytes\n"
   "\n"
   "To blame\n"
   "--------\n"
   "     Sebastian Achim Mueller, ETH Zurich 2016.\n";
   FILE* out;
   out = fopen(sane->readme_path, "w");
   CerIo_assert_file_is_open(sane, out, sane->readme_path);
   fputs(readme, out);
   fclose(out);    
}

void CerIo_evth_path(struct CerIo* sane, int event_number, char* evth_path) {
   char evt_num[1024];
   sprintf(evt_num, "%d", event_number);
   strcpy(evth_path, sane->evth_template_path);
   strcat(evth_path, evt_num);
}

void CerIo_photons_path(struct CerIo* sane, int event_number, char* photons_path) {
   char evt_num[1024];
   sprintf(evt_num, "%d", event_number);
   strcpy(photons_path, sane->photon_block_template_path);
   strcat(photons_path, evt_num);
}

void CerIo_assign_observation_level(struct CerIo* sane, cors_real_t runh[273]) {
   sane->observation_level = runh[4 + (int)runh[4]];
   double oht = sane->observation_level;
   sane->speed_of_light_in_air_on_observation_level = 
      VACUUM_SPEED_OF_LIGHT/refidx_(&oht);
}

void CerIo_write_runh(struct CerIo* sane, cors_real_t runh[273]) {
   CerIo_assign_observation_level(sane, runh);
   CerIo_write_readme(sane);
   FILE* out;
   out = fopen(sane->runh_path, "wb");
   fwrite(&runh, sizeof(cors_real_t), 273, out);
   fclose(out);  
}

void CerIo_assign_time_offset(struct CerIo* sane, cors_real_t evth[273]) {
   double z_first_interaction = evth[6];
   double zenith_angle = evth[10];
   double t = 0.0;
   if(z_first_interaction < 0.0)
      sane->time_offset = (heigh_(&t)-sane->observation_level) / cos(zenith_angle) / VACUUM_SPEED_OF_LIGHT;
   else/* Time is counted since first interaction. */
      sane->time_offset = (z_first_interaction - sane->observation_level) / cos(zenith_angle) / VACUUM_SPEED_OF_LIGHT;  

   fprintf(stderr, "time_offset: %E\n",sane->time_offset);
}

void CerIo_write_evth(struct CerIo* sane, cors_real_t evth[273], int event_number) {
   CerIo_assign_time_offset(sane, evth);
   char evth_path[1024];
   CerIo_evth_path(sane, event_number, evth_path);
   FILE* out;
   out = fopen(evth_path, "wb");
   CerIo_assert_file_is_open(sane, out, evth_path);
   fwrite(&evth, sizeof(cors_real_t), 273, out);
   fclose(out);  
}

void CerIo_open_photon_block(struct CerIo* sane, int event_number) {
   char photons_path[1024];
   CerIo_photons_path(sane, event_number, photons_path);
   sane->current_photons = fopen(photons_path, "wb"); 
   CerIo_assert_file_is_open(sane, sane->current_photons, photons_path);
}

void CerIo_append_photon_bunch(struct CerIo* sane, struct PhotonBunch* bunch) {
   fwrite(bunch, sizeof((*bunch)), 1, sane->current_photons);
}

void CerIo_close_photon_block(struct CerIo* sane) {
   fclose(sane->current_photons); 
}

//-------------------- Detector ------------------------------------------------
struct Detector {
   double x;
   double y;
   double z;
   double radius;
};

void Detector_init(
   struct Detector* pl, 
   double x, 
   double y, 
   double z, 
   double r
) {
   pl->x = x;
   pl->y = y;
   pl->z = z;
   pl->radius = r;
}

int Detector_is_hit_by_photon(
   struct Detector* pl, 
   struct PhotonBunch* bunch
) {
   // The ray equation of the photon bunch:
   double sx, sy, sz, dx, dy, dz;
   // Ray support vector:
   sx = bunch->x;
   sy = bunch->y;
   sz = 0.0;
   // Ray direction vector:
   dx = bunch->cx;
   dy = bunch->cy;
   dz = sqrt(1.0 - dx*dx - dy*dy);

   // The detector center:
   double px, py, pz;
   px = pl->x;
   py = pl->y;
   pz = pl->z;

   // Calculate ray parameter alpha (vec{x}:= vec{s} + alpha * vec{d}) which 
   // marks the closest point on the photon bunch ray to the detector center. 
   double d = dx*px + dy*py + dz*pz;
   double alpha = d - sx*dx - sy*dy - sz*dz;

   // Closest point on photon bunch to detector center.
   double clx, cly, clz;
   clx = sx + alpha*dx;
   cly = sy + alpha*dy;
   clz = sz + alpha*dz;

   // Connection vector between detector center and closest point on photon 
   // bunch ray.
   double conx, cony, conz;
   conx = px - clx;
   cony = py - cly;
   conz = pz - clz;

   // The closest distance from the detector center to the photon bunch ray.
   double distance_to_detector_center = 
      sqrt(conx*conx + cony*cony + conz*conz);

   return pl->radius >= distance_to_detector_center;
}

void Detector_transform_to_detector_frame(
   struct Detector* pl, 
   struct PhotonBunch* bunch
) {
   bunch->x = bunch->x - pl->x;
   bunch->y = bunch->y - pl->y;
}

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
   struct CerIo* cerio, 
   struct Detector* detector
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

//-------------------- init ----------------------------------------------------
int SEED = -1;
char output_path[1024] = "";

struct Detector detector;
struct CerIo cer_out;

//-------------------- CORSIKA bridge ------------------------------------------
/**
 * Define the output file for photon bunches hitting the telescopes.
 * @param  name    Output file name.
*/
void telfil_ (char *name) {
   strcpy(output_path, name);
   return;
}


/**
 *  Set the file name with parameters for importance sampling.
 */
void telsmp_ (char *name) {
   return;
}


/**
 *  Show what telescopes have actually been set up.
 *  This function is called by CORSIKA after the input file is read.
*/
void telshw_ () {
   return;
}

/**
 * Return information about configured telescopes back to CORSIKA
 *
 * @param  itel     number of telescope in question
 * @param  x, y, z  telescope position [cm]
 * @param  r	    radius of fiducial volume [cm]
 * @param  exists   telescope exists
*/
void telinf_ (
   int *itel, 
   double *x, 
   double *y, 
   double *z, 
   double *r, 
   int *exists
) {
   fprintf(stderr,"The telinf_ was called.\n");
   exit(1);
}


/**
 *  Save aparameters from CORSIKA run header.
 *
 *  @param  runh CORSIKA run header block
 *  @return (none)
*/
void telrnh_ (cors_real_t runh[273]) {
   SEED = (int)runh[(11+3*1)-1];
   CerIo_init(&cer_out, output_path);
   CerIo_write_runh(&cer_out, runh);
}


/**
 *  Keep a record of CORSIKA input lines.
 *
 *  @param  line     input line (not terminated)
 *  @param  llength  maximum length of input lines (132 usually)
*/
void tellni_ (char *line, int *llength) {
   return;
}


/**
 *   Write run end block to the output file.
 *
 *  @param  rune  CORSIKA run end block
*/
void telrne_ (cors_real_t rune[273]) {
   return;
}


/**
 *  Setup how many times each shower is used.
 *
 *  @param n   The number of telescope systems
 *  @param dx  Core range radius (if dy==0) or core x range
 *  @param dy  Core y range (non-zero for ractangular, 0 for circular)
 *  @return (none)
*/
void telasu_ (int *n, cors_real_dbl_t *dx, cors_real_dbl_t *dy) {
   return;
}


/**
 *  Add another telescope to the system (array) of telescopes.
 *
 *  This function is called for each TELESCOPE keyword in the
 *  CORSIKA input file.
 *
 *  @param  x  X position [cm]
 *  @param  y  Y position [cm]
 *  @param  z  Z position [cm]
 *  @param  r  radius [cm] within which the telescope is fully contained
 *  @return (none)
*/
void telset_ (
   cors_real_now_t *x, 
   cors_real_now_t *y, 
   cors_real_now_t *z, 
   cors_real_now_t *r
) {
   Detector_init(&detector, (*x), (*y), (*z), (*r));
}


/**
 *  Start of new event. Save event parameters.
 *
 *  @param  evth    CORSIKA event header block
 *  @param  prmpar  CORSIKA primary particle block
 *  @return (none)
*/
void televt_ (cors_real_t evth[273], cors_real_dbl_t prmpar[PRMPAR_SIZE]) {
   int event_number = (int)evth[2-1];
   CerIo_write_evth(&cer_out, evth, event_number);
   CerIo_open_photon_block(&cer_out, event_number);
}


/**
 *  Check if a photon bunch hits one or more simulated detector volumes.
 *
 *  @param  bsize   Number of photons (can be fraction of one)
 *  @param  wt	   Weight (if thinning option is active)
 *  @param  px	   x position in detection level plane
 *  @param  py	   y position in detection level plane
 *  @param  pu	   x direction cosine
 *  @param  pv	   y direction cosine
 *  @param  ctime   arrival time in plane after first interaction
 *  @param  zem     height of emission above sea level
 *  @param  lambda  0. (if wavelength undetermined) or wavelength [nm].
 *                  If lambda < 0, photons are already converted to
 *                  photo-electrons (p.e.), i.e. we have p.e. bunches.
 *  @param  temis   Time of photon emission 
 *  @param  penergy Energy of emitting particle.
 *  @param  amass   Mass of emitting particle.
 *  @param  charge  Charge of emitting particle.
 *
 *  @return  0 (no output to old-style CORSIKA file needed)
 *           2 (detector hit but no eventio interface available or
 *             output should go to CORSIKA file anyway)
*/
int telout_ (
   cors_real_now_t *bsize, 
   cors_real_now_t *wt, 
   cors_real_now_t *px, 
   cors_real_now_t *py, 
   cors_real_now_t *pu, 
   cors_real_now_t *pv, 
   cors_real_now_t *ctime, 
   cors_real_now_t *zem , 
   cors_real_now_t *lambda, 
   double *temis, 
   double *penergy, 
   double *amass, 
   double *charge
) {
   struct PhotonBunch bunch;

   bunch.size = *bsize;
   bunch.x = *px;
   bunch.y = *py;
   bunch.cx = *pu;
   bunch.cy = *pv;
   bunch.arrival_time = *ctime;
   bunch.emission_altitude = *zem; 
   bunch.wavelength = fabs(*lambda);
   bunch.mother_mass = *amass;
   bunch.mother_charge = *charge; 

   if(Detector_is_hit_by_photon(&detector, &bunch)) {
      Detector_transform_to_detector_frame(&detector, &bunch);
      CerIo_append_photon_bunch(&cer_out, &bunch);
   }
   return 0;
}


/**
 *  @short Store CORSIKA particle information into IACT output file.
 *
 *  @param datab  A particle data buffer with up to 39 particles.
 *  @param maxbuf The buffer size, which is 39*7 without thinning
 *                option and 39*8 with thinning.
 */
void telprt_ (cors_real_t *datab, int *maxbuf) {
   return;
}


/**
 *  Write CORSIKA 'longitudinal' (vertical) distributions.
 *
 *  @param  type    see above
 *  @param  data    set of (usually 9) distributions
 *  @param  ndim    maximum number of entries per distribution
 *  @param  np      number of distributions (usually 9)
 *  @param  nthick  number of entries actually filled per distribution
 *                  (is 1 if called without LONGI being enabled).
 *  @param  thickstep  step size in g/cm**2
 *  @return  (none)
*/
void tellng_ (
   int *type, 
   double *data, 
   int *ndim, 
   int *np, 
   int *nthick, 
   double *thickstep
) {
   return;
}


/**
 *  End of event. Write out all recorded photon bunches.
*/
void telend_ (cors_real_t evte[273])
{
   CerIo_close_photon_block(&cer_out);
   return;
}


/**
 *  Placeholder function for external shower-by-shower setting
 *         of primary type, energy, and direction.
 */
void extprm_ (
   cors_real_dbl_t *type, 
   cors_real_dbl_t *eprim,
   double *thetap, 
   double *phip
) {
   return;
}