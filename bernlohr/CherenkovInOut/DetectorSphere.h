#ifndef __DetectorSphere_H_INCLUDED__
#define __DetectorSphere_H_INCLUDED__

// forward declared dependencies

// included dependencies

//-------------------- DetectorSphere ------------------------------------------
struct DetectorSphere {
   double x;
   double y;
   double z;
   double radius;
};

void DetectorSphere_init(
   struct DetectorSphere* pl, 
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

int DetectorSphere_is_hit_by_photon(
   struct DetectorSphere* pl, 
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

void DetectorSphere_transform_to_detector_frame(
   struct DetectorSphere* pl, 
   struct PhotonBunch* bunch
) {
   bunch->x = bunch->x - pl->x;
   bunch->y = bunch->y - pl->y;
}

#endif // __DetectorSphere_H_INCLUDED__ 