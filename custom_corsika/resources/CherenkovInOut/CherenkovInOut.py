import numpy as np

compressed_photon_dtype = np.dtype([
   ('x', np.int16),
   ('y', np.int16),
   ('cx', np.int16),
   ('cy', np.int16),
   ('arrival_time', np.float32),
   ('wavelength', np.int8),
   ('mother_charge', np.int8),
   ('emission_altitude', np.int16),
])

def read_compressed_photons(path):
    return np.fromfile(path, dtype=compressed_photon_dtype)

def decompress_photons(comp):
    photons = np.zeros(shape=(comp.shape[0] ,8), dtype=np.float32)
    int16max = 32767.0
    int8max = 255.0
    photons[:,0] = (comp['x']/int16max)*260.0e2
    photons[:,1] = (comp['y']/int16max)*260.0e2
    photons[:,2] = comp['cx']/int16max
    photons[:,3] = comp['cy']/int16max
    photons[:,4] = comp['arrival_time']
    photons[:,5] = (comp['wavelength']/int8max)*1e3 + 2e2
    photons[:,6] = comp['mother_charge']
    photons[:,7] = (comp['emission_altitude']/int16max)*100*1e3*1e2
    return photons