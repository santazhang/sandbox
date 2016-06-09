#include "sg6.h"

#include "gtest/gtest.h"

#include <stdio.h>

using namespace sg6;


#define VERIFY_LON_LAT_NORMALIZED(ll) \
    do { \
        EXPECT_GE(ll.lon, -180.0); \
        EXPECT_LT(ll.lon, 180.0); \
        EXPECT_GE(ll.lat, -90.0); \
        EXPECT_LE(ll.lat, 90.0); \
        if (ll.lat == 90.0 || ll.lat == -90.0) { \
            EXPECT_EQ(0.0, ll.lon); \
        } \
    } while (0); \


TEST(lon_lat, normlize) {
    lon_lat_t ll;
    printf("Default init of lon_lat_t: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lat = 91;
    printf("Before normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    ll = normalize(ll);
    printf("After normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lat = 90;
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lat = -90;
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lat = -91;
    ll = normalize(ll);
    VERIFY_LON_LAT_NORMALIZED(ll);
    
    ll.lon = 10;
    ll.lat = 90;
    ll = normalize(ll);
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lon = 180;
    ll.lat = 89;
    printf("Before normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    ll = normalize(ll);
    printf("After normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lon = -180.1;
    ll.lat = 89;
    printf("Before normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    ll = normalize(ll);
    printf("After normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lon = 93993;
    ll.lat = 923923;
    printf("Before normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    ll = normalize(ll);
    printf("After normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lon = 93993;
    ll.lat = -923923;
    printf("Before normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    ll = normalize(ll);
    printf("After normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    VERIFY_LON_LAT_NORMALIZED(ll);
    
    ll.lon = -93993;
    ll.lat = 923923;
    printf("Before normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    ll = normalize(ll);
    printf("After normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    VERIFY_LON_LAT_NORMALIZED(ll);

    ll.lon = -93993;
    ll.lat = -923923;
    printf("Before normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    ll = normalize(ll);
    printf("After normalize: lon=%lf, lat=%lf\n", ll.lon, ll.lat);
    VERIFY_LON_LAT_NORMALIZED(ll);
}

#define VERIFY_SHERICAL_NORMALIZED(sph) \
    do { \
        EXPECT_GE(sph.azimuthal, 0); \
        EXPECT_LT(sph.azimuthal, TWO_PI); \
        EXPECT_GE(sph.polar, 0); \
        EXPECT_LE(sph.polar, PI); \
        if (sph.polar == 0 || sph.polar == PI) { \
            EXPECT_EQ(PI, sph.azimuthal); \
        } \
    } while (0); \

TEST(spherical, normalize) {
    spherical_t sph;
    printf("Default init of spherical_t: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    VERIFY_SHERICAL_NORMALIZED(sph);

    sph.azimuthal = TWO_PI;
    printf("Before normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    sph = normalize(sph);
    printf("After normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    VERIFY_SHERICAL_NORMALIZED(sph);

    sph.azimuthal = 1000;
    printf("Before normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    sph = normalize(sph);
    printf("After normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    VERIFY_SHERICAL_NORMALIZED(sph);

    sph.polar = -930;
    sph.azimuthal = 1000;
    printf("Before normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    sph = normalize(sph);
    printf("After normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    VERIFY_SHERICAL_NORMALIZED(sph);

    sph.polar = -930;
    sph.azimuthal = -1000;
    printf("Before normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    sph = normalize(sph);
    printf("After normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    VERIFY_SHERICAL_NORMALIZED(sph);
    
    sph.polar = 930;
    sph.azimuthal = 1000;
    printf("Before normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    sph = normalize(sph);
    printf("After normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    VERIFY_SHERICAL_NORMALIZED(sph);
    
    sph.polar = 930;
    sph.azimuthal = -1000;
    printf("Before normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    sph = normalize(sph);
    printf("After normalize: azimuthal=%lf, polar=%lf\n", sph.azimuthal, sph.polar);
    VERIFY_SHERICAL_NORMALIZED(sph);
}

TEST(conversion, coordinates) {
    lon_lat_t ll;
    ll.lon = 30;
    ll.lat = 40;
    spherical_t sph = lon_lat_to_spherical(ll);
    printf("LON_LAT(%lf, %lf) -> SPH(%lf, %lf)\n", ll.lon, ll.lat, sph.azimuthal, sph.polar);
    
    ll = normalize(ll);
    printf("normlized: LON_LAT(%lf, %lf)\n", ll.lon, ll.lat);

    ll = spherical_to_lon_lat(sph);
    printf("SPH(%lf, %lf) -> LON_LAT(%lf, %lf)\n", sph.azimuthal, sph.polar, ll.lon, ll.lat);

    xyz_t pt;
    pt.x = 1;
    pt.y = 1;
    pt.z = 1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);
    
    pt.x = -1;
    pt.y = 1;
    pt.z = 1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);
    
    pt.x = 1;
    pt.y = -1;
    pt.z = 1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);
    
    pt.x = -1;
    pt.y = -1;
    pt.z = 1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);
    
    pt.x = 1;
    pt.y = 1;
    pt.z = -1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);
    
    pt.x = -1;
    pt.y = 1;
    pt.z = -1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);
    
    pt.x = 1;
    pt.y = -1;
    pt.z = -1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);
    
    pt.x = -1;
    pt.y = -1;
    pt.z = -1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);

    pt.x = 1;
    pt.y = 0;
    pt.z = 0;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);

    pt.x = 0;
    pt.y = 1;
    pt.z = 0;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);

    pt.x = 0;
    pt.y = -1;
    pt.z = 0;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);

    pt.x = -1;
    pt.y = 0;
    pt.z = 0;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);

    pt.x = 0;
    pt.y = 0;
    pt.z = 1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);

    pt.x = 0;
    pt.y = 0;
    pt.z = -1;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);

    pt.x = 0;
    pt.y = 0;
    pt.z = 0;
    sph = xyz_to_unit_spherical(pt);
    printf("XYZ(%lf, %lf, %lf) -> unit SPH(%lf, %lf)\n", pt.x, pt.y, pt.z, sph.azimuthal, sph.polar);
    pt = spherical_to_xyz(sph);
    printf("SPH(%lf, %lf) -> XYZ(%lf, %lf, %lf)\n", sph.azimuthal, sph.polar, pt.x, pt.y, pt.z);
}

TEST(region, basic) {
    // new york
    lon_lat_t ll;
    ll.lon = -75;
    ll.lat = 40;

    spherical_t sph = lon_lat_to_spherical(ll);
    printf("New York is in region: %d\n", spherical_to_region(sph));
}

int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
