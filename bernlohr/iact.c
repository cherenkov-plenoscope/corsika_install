/*
   Copyright (C) 1997, 1998, ..., 2012, 2013, 2016  Konrad Bernloehr

   This file is part of the IACT/atmo package for CORSIKA.

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

#define IACT_ATMO_VERSION "1.49 (2016-01-27)"
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
/* FORTRAN called functions (beware changes of parameter types !!) */
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

/* =============================================================== */
/* The ACP */
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

int SEED = -1;
FILE *acp_photon_block;
char acp_out_path[1024] = "";
struct Plenoscope {
   double x;
   double y;
   double z;
   double radius;
};
struct Plenoscope plenoscope;

/**
 * Define the output file for photon bunches hitting the telescopes.
 * @param  name    Output file name.
*/
void telfil_ (char *name) {
   strcpy(acp_out_path, name);
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

   plenoscope.x = 0.0;
   plenoscope.y = 0.0;
   plenoscope.z = 0.0;
   plenoscope.radius = 55.0e2;

   FILE *acp_run_header;

   char runh_path[1024];
   strcpy(runh_path, acp_out_path);
   strcat(runh_path, ".runh");
   acp_run_header = fopen(runh_path, "wb");
   fwrite(&runh, sizeof(cors_real_t), 273, acp_run_header);
   fclose(acp_run_header);
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
   return;
}


/**
 *  Start of new event. Save event parameters.
 *
 *  @param  evth    CORSIKA event header block
 *  @param  prmpar  CORSIKA primary particle block
 *  @return (none)
*/
void televt_ (cors_real_t evth[273], cors_real_dbl_t prmpar[PRMPAR_SIZE]) {
   FILE *acp_evt_header;
   char evt_num[1024];
   sprintf(evt_num, "%d", (int)evth[2-1]);
   char evth_path[1024];
   strcpy(evth_path, acp_out_path);
   strcat(evth_path, ".evth.");
   strcat(evth_path, evt_num);
   acp_evt_header = fopen(evth_path, "wb");
   fwrite(&evth, sizeof(cors_real_t), 273, acp_evt_header);
   fclose(acp_evt_header);

   char photon_block_path[1024];
   strcpy(photon_block_path, acp_out_path);
   strcat(photon_block_path, ".photons.");
   strcat(photon_block_path, evt_num);   
   acp_photon_block = fopen(photon_block_path, "wb");
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

   double dist = sqrt(bunch.x*bunch.x + bunch.y*bunch.y);
   if(dist <= plenoscope.radius)
      fwrite(&bunch, sizeof(bunch), 1, acp_photon_block);
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
   fclose(acp_photon_block);
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