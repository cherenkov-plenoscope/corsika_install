import pytest
import os
import tempfile
import corsika_primary_wrapper as cpw
import corsika_wrapper as cw
import numpy as np


@pytest.fixture()
def corsika_primary_path(pytestconfig):
    return pytestconfig.getoption("corsika_primary_path")


@pytest.fixture()
def non_temporary_path(pytestconfig):
    return pytestconfig.getoption("non_temporary_path")


def test_different_starting_depths(corsika_primary_path, non_temporary_path):
    assert os.path.exists(corsika_primary_path)

    NUM_DEPTHS = 10
    NUM_EVENTS_PER_DEPTH = 100

    depths = np.linspace(0.0, 950.0, NUM_DEPTHS)
    steering_dict = {
        "run": {
            "run_id": 1,
            "event_id_of_first_event": 1,
            "observation_level_asl_m": 0.0,
            "earth_magnetic_field_x_muT": 12.5,
            "earth_magnetic_field_z_muT": -25.9,
            "atmosphere_id": 10,
        },
        "primaries": [],
    }

    seed = 0
    for depth in depths:
        for rep in range(NUM_EVENTS_PER_DEPTH):
            prm = {
                "particle_id": 1,
                "energy_GeV": 1,
                "zenith_rad": 0.0,
                "azimuth_rad": 0.0,
                "depth_g_per_cm2": depth,
                "random_seed": cpw.simple_seed(seed),
            }
            steering_dict["primaries"].append(prm)
            seed += 1

    num_events = len(steering_dict["primaries"])
    num_bunches = []
    num_photons = []
    std_r = []
    tmp_prefix = "test_different_starting_depths_"
    with tempfile.TemporaryDirectory(prefix=tmp_prefix) as tmp_dir:
        if non_temporary_path != "":
            tmp_dir = os.path.join(non_temporary_path, tmp_prefix)
            os.makedirs(tmp_dir, exist_ok=True)

        run_path = os.path.join(tmp_dir, "different_starting_depths.tar")
        if not os.path.exists(run_path):
            cpw.corsika_primary(
                corsika_path=corsika_primary_path,
                steering_dict=steering_dict,
                output_path=run_path,
            )
        run = cpw.Tario(run_path)

        for depth in depths:
            _num_bunches = []
            _num_photons = []
            _std_r = []
            for rep in range(NUM_EVENTS_PER_DEPTH):
                event = next(run)
                evth, bunches = event
                if bunches.shape[0] > 0:
                    _num_bunches.append(bunches.shape[0])
                    _num_photons.append(np.sum(bunches[:, cpw.IBSIZE]))
                    _std_r.append(
                        np.hypot(
                            np.std(bunches[:, cpw.IX]),
                            np.std(bunches[:, cpw.IY]),
                        )
                    )
            num_bunches.append(np.mean(_num_bunches))
            num_photons.append(np.mean(_num_photons))
            std_r.append(np.mean(_std_r))

    print("num   depth   num.ph.   std.x.y.")
    for ii in range(depths.shape[0]):
        print(
            "{: 3d} {: 3.1f} {: 6.1f} {: 6.1f}".format(
                ii, depths[ii], num_photons[ii], 1e-2 * std_r[ii]
            )
        )

    # max photons is somewhere in the middle:
    # Too high -> photons are absorbed before reaching ground.
    # Too low -> shower reaches obs. level before photons can be emitted.
    depth_with_max_ph = np.argmax(num_photons)
    assert depth_with_max_ph != 0
    assert depth_with_max_ph != NUM_DEPTHS - 1

    # The spread of the light-pool should be smaller for starting points deeper
    # in the atmosphere.
    assert np.all(np.gradient(std_r) < 0)
