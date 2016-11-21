import numpy as np

photon_dtype = np.dtype([
   ('x', np.int16),
   ('y', np.int16),
   ('cx', np.int16),
   ('cy', np.int16),
   ('arrival_time', np.float32),
   ('wavelength', np.int8),
   ('emission_altitude', np.int16),
   ('mother_charge', np.int8),
])

int16_t x;
int16_t y;
int16_t cx;
int16_t cy;
float arrival_time;
uint8_t wavelength;
uint16_t emission_altitude;
int8_t mother_charge;