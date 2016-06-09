#include "sg6.h"

#include <stdio.h>
#include <math.h>

namespace sg6 {

const double PI = 3.141592653589793238462643383279502884197169399375105820974;
const double TWO_PI = 6.283185307179586476925286766559005768394338798750211641949;
const double HALF_PI = 1.570796326794896619231321691639751442098584699687552910487;

lon_lat_t normalize(lon_lat_t lon_lat) {
    lon_lat_t norm = lon_lat;

    if (norm.lat > 90.0) {
        int d = static_cast<int>(norm.lat / 180.0);
        norm.lat -= d * 180.0;
        while (norm.lat > 90.0) {
            norm.lat -= 180.0;
        }
    } else if (norm.lat < -90.0) {
        int d = static_cast<int>(-norm.lat / 180.0);
        norm.lat += d * 180.0;
        while (norm.lat < -90.0) {
            norm.lat += 180.0;
        }
    } else if (norm.lat == 90.0 || norm.lat == -90.0) {
        // NOTE: north pole and south pole has lon=0
        norm.lon = 0.0;
    }

    // -180 <= lon < 180
    if (norm.lon >= 180.0) {
        int d = static_cast<int>(norm.lat / 360.0);
        norm.lon -= d * 360.0;
        while (norm.lon >= 180.0) {
            norm.lon -= 360.0;
        }
    } else if (norm.lon < -180) {
        int d = static_cast<int>(-norm.lat / 360.0);
        norm.lon += d * 360.0;
        while (norm.lon < -180.0) {
            norm.lon += 360.0;
        }
    }
    
    return norm;
}

spherical_t normalize(spherical_t sph) {
    spherical_t norm = sph;

    if (norm.polar > PI) {
        int d = static_cast<int>(norm.polar / PI);
        norm.polar -= d * PI;
        while (norm.polar > PI) {
            norm.polar -= PI;
        }
    } else if (norm.polar < 0) {
        int d = static_cast<int>(-norm.polar / PI);
        norm.polar += d * PI;
        while (norm.polar < 0) {
            norm.polar += PI;
        }
    } else if (norm.polar == 0 || norm.polar == PI) {
        // NOTE: north pole and south pole has azimuthal = pi (prime meridian)
        norm.azimuthal = PI;
    }

    // 0 <= azimuthal < 2pi
    if (norm.azimuthal >= TWO_PI) {
        int d = static_cast<int>(norm.azimuthal / TWO_PI);
        norm.azimuthal -= d * TWO_PI;
        while (norm.azimuthal >= TWO_PI) {
            norm.azimuthal -= TWO_PI;
        }
    } else if (norm.azimuthal < 0) {
        int d = static_cast<int>(-norm.azimuthal / TWO_PI);
        norm.azimuthal += d * TWO_PI;
        while (norm.azimuthal < 0) {
            norm.azimuthal += TWO_PI;
        }
    }

    return norm;
}

spherical_t lon_lat_to_spherical(lon_lat_t ll) {
    ll = normalize(ll);
    spherical_t sph;
    sph.polar = (90.0 - ll.lat) * PI / 180.0;
    sph.azimuthal = (ll.lon + 180.0) * TWO_PI / 360.0;
    return normalize(sph);
}

lon_lat_t spherical_to_lon_lat(spherical_t sph) {
    sph = normalize(sph);
    lon_lat_t ll;
    ll.lat = 90.0 - sph.polar * 180.0 / PI;
    ll.lon = sph.azimuthal * 360.0 / TWO_PI - 180.0;
    return normalize(ll);
}

xyz_t spherical_to_xyz(spherical_t sph) {
    xyz_t pt;
    pt.z = cos(sph.polar);
    // -PI because x axis is prime meridian
    pt.x = sin(sph.polar) * cos(sph.azimuthal - PI);
    pt.y = sin(sph.polar) * sin(sph.azimuthal - PI);
    return pt;
}

spherical_t xyz_to_unit_spherical(xyz_t pt) {
    spherical_t sph;
    if (pt.x == 0 && pt.y == 0) {
        sph.azimuthal = PI;  // prime meridian
        if (pt.z >= 0) {
            // NORTH POLE
            sph.polar = 0;
        } else {
            // SOUTH POLE
            sph.polar = PI;
        }
    } else {
        // NOTE: x axis -> prime meridian, y -> 90deg east
        // acos returns [0, pi]
        sph.polar = acos(pt.z / sqrt(pt.x * pt.x + pt.y * pt.y + pt.z * pt.z));
        if (pt.x == 0) {
            if (pt.y < 0) {
                sph.azimuthal = HALF_PI;
            } else if (pt.y > 0) {
                sph.azimuthal = 3 * HALF_PI;
            }
        } else {
            // x != 0 && y != 0
            // atan returns [-pi/2, pi/2]
            if (pt.x > 0) {
                sph.azimuthal = PI + atan(pt.y / pt.x);
            } else if (pt.y < 0) {
                // x < 0
                sph.azimuthal = atan(pt.y / pt.x);
            } else {
                // x < 0
                sph.azimuthal = TWO_PI + atan(pt.y / pt.x);
            }
        }
    }

    return normalize(sph);
}

int spherical_to_region(spherical_t sph) {
    sph = normalize(sph);
    int region = 0;
    if (sph.polar <= PI) {
        // exclude south
        region = 0;  // default: north pole
        int candidate_region = region;
        if (sph.azimuthal >= 0.75 * PI && sph.azimuthal < 1.25 * PI) {
            candidate_region = 1;  // +x axis
        } else if (sph.azimuthal >= 1.25 * PI && sph.azimuthal < 1.75 * PI) {
            candidate_region = 2;  // +y axis
        } else if (sph.azimuthal >= 0.25 * PI && sph.azimuthal < 0.75 * PI) {
            candidate_region = 4;  // -y axis
        } else {
            candidate_region = 3;  // -x axis
        }

        if (sph.polar < 0.25 * PI) {
            // north pole
            return region;
        } else if (sph.polar > HALF_PI - 0.6) {
            // arctan(1/sqrt(2)) = 0.615479709
            return candidate_region;
        } else {
            // check if line intersects with z = 1 with in -1<=x<=1 && -1<=y<=1, if so, north pole
            double x = 1 * tan(sph.polar) * cos(sph.azimuthal - PI);
            double y = 1 * tan(sph.polar) * sin(sph.azimuthal - PI);
            if (-1 <= x && x <= 1 && -1 <= y && y <= 1) {
                // north pole region
                return region;
            } else {
                return candidate_region;
            }
        }

    } else {
        // exclude north
        region = 5;  // default: south pole
        int candidate_region = region;
        if (sph.azimuthal >= 0.75 * PI && sph.azimuthal < 1.25 * PI) {
            candidate_region = 1;  // +x axis
        } else if (sph.azimuthal >= 1.25 * PI && sph.azimuthal < 1.75 * PI) {
            candidate_region = 2;  // +y axis
        } else if (sph.azimuthal >= 0.25 * PI && sph.azimuthal < 0.75 * PI) {
            candidate_region = 4;  // -y axis
        } else {
            candidate_region = 3;  // -x axis
        }

        if (sph.polar > 0.75 * PI) {
            // south pole
            return region;
        } else if (sph.polar < HALF_PI + 0.6) {
            // arctan(1/sqrt(2)) = 0.615479709
            return candidate_region;
        } else {
            // check if line intersects with z = -1 with in -1<=x<=1 && -1<=y<=1, if so, south pole
            double x = -1 * tan(sph.polar) * cos(sph.azimuthal - PI);
            double y = -1 * tan(sph.polar) * sin(sph.azimuthal - PI);
            if (-1 <= x && x <= 1 && -1 <= y && y <= 1) {
                // south pole region
                return region;
            } else {
                return candidate_region;
            }
        }
    }
    return region;
}

}  // namespace sg6
