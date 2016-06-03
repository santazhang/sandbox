#include <stdio.h>

#include "mgrs.h"

int main() {
    printf("This shall be done!\n");
    double a, f;
    char c;
    Get_MGRS_Parameters(&a, &f, &c);
    printf("a=%lf f=%lf c=%c\n", a, f, c);
    double lon = -999, lat = -999;
    Convert_MGRS_To_Geodetic("16TDM170245", &lon, &lat);
    lon *= 180/3.1415926;
    lat *= 180/3.1415926;
    printf("lon=%lf, lat = %lf\n", lon, lat);

    // Washington Square Park
    lon = -73.9971;
    lat = 40.7312;

    // to radius
    lon /= 180/3.1415926;
    lat /= 180/3.1415926;

    char buf[100];
    Convert_Geodetic_To_MGRS(lat, lon, 3 /* precision */, buf);
    printf("Washington Square Park -> %s\n", buf);

    // North pole
    lon = -73.9971;
    lat = 89.99;

    // to radius
    lon /= 180/3.1415926;
    lat /= 180/3.1415926;

    Convert_Geodetic_To_MGRS(lat, lon, 3 /* precision */, buf);
    printf("Near North Pole -> %s\n", buf);

    return 0;
}
