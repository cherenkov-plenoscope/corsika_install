import numpy as np

photon_dtype = np.dtype([
   ('x', np.int16),
   ('y', np.int16),
   ('cx', np.int16),
   ('cy', np.int16),
   ('arrival_time', np.float32),
   ('wavelength', np.int8),
   ('mother_charge', np.int8),
   ('emission_altitude', np.int16),
])