#pragma once

namespace sg6 {

extern const double PI;
extern const double TWO_PI;
extern const double HALF_PI;

struct lon_lat_t {
    double lon;  // longitude, -180 west, 0 prime meridian, +180 east
    double lat;  // latitude, +90 north pole, -90 south pole

    lon_lat_t(): lon(0), lat(0) { }

    // NOTE: north pole and south pole has lon=0 (prime meridian)
};

// spherical coordinates (r=1.0, theta, phi)
struct spherical_t {
    double azimuthal;  // -180 west -> 0, prime meridian -> pi, +180 east -> 2pi
    double polar;  // north pole=0, south pole=pi

    spherical_t(): azimuthal(PI), polar(0) { }

    // NOTE: north pole and south pole has azimuthal = pi (prime meridian)
};

struct xyz_t {
    double x;
    double y;
    double z;

    xyz_t(): x(0), y(0), z(0) { }

    // x axis -> prime meridian, y -> 90deg east
};

lon_lat_t normalize(lon_lat_t lon_lat);
spherical_t normalize(spherical_t theta_phi);

spherical_t lon_lat_to_spherical(lon_lat_t);
lon_lat_t spherical_to_lon_lat(spherical_t);

xyz_t spherical_to_xyz(spherical_t);
spherical_t xyz_to_unit_spherical(xyz_t);

// north pole=0, +x=1, +y=2, -x=3, -y=4, south pole=5
int spherical_to_region(spherical_t sph);

}  // namespace sg6
