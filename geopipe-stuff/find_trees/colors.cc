#include "colors.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace find_trees {

// hsv <-> rgb from http://stackoverflow.com/a/6930407/1035246
hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = static_cast<long>(hh);
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

static inline double lab_cvt_helper(double q) {
	if (q > 0.008856) {
		return pow(q, 0.333333);
	} else {
		return 7.787 * q + 0.137931;
	}
}


namespace {

// Based on code from ImageMagick (which we used to generated training LAB_A images).
typedef uint8_t Quantum;
const double QuantumRange = 255.0;
const double QuantumScale = 1 / QuantumRange;
const double CIEEpsilon = 216.0/24389.0;
const double CIEK = 24389.0/27.0;
const double D65X = 0.950456;
const double D65Y = 1.0;
const double D65Z = 1.088754;
typedef double MagickRealType;

static inline double DecodeGamma(const double x)
{
  div_t
    quotient;

  double
    p,
    term[9];

  int
    exponent;

  static const double coefficient[] =
  {
    1.7917488588043277509,
    0.82045614371976854984,
    0.027694100686325412819,
    -0.00094244335181762134018,
    0.000064355540911469709545,
    -5.7224404636060757485e-06,
    5.8767669437311184313e-07,
    -6.6139920053589721168e-08,
    7.9323242696227458163e-09
  };

  static const double powers_of_two[] =
  {
    1.0,
    2.6390158215457883983,
    6.9644045063689921093,
    1.8379173679952558018e+01,
    4.8502930128332728543e+01
  };

  term[0]=1.0;
  term[1]=4.0*frexp(x,&exponent)-3.0;
  term[2]=2.0*term[1]*term[1]-term[0];
  term[3]=2.0*term[1]*term[2]-term[1];
  term[4]=2.0*term[1]*term[3]-term[2];
  term[5]=2.0*term[1]*term[4]-term[3];
  term[6]=2.0*term[1]*term[5]-term[4];
  term[7]=2.0*term[1]*term[6]-term[5];
  term[8]=2.0*term[1]*term[7]-term[6];
  p=coefficient[0]*term[0]+coefficient[1]*term[1]+coefficient[2]*term[2]+
    coefficient[3]*term[3]+coefficient[4]*term[4]+coefficient[5]*term[5]+
    coefficient[6]*term[6]+coefficient[7]*term[7]+coefficient[8]*term[8];
  quotient=div(exponent-1,5);
  if (quotient.rem < 0)
    {
      quotient.quot-=1;
      quotient.rem+=5;
    }
  return(x*ldexp(powers_of_two[quotient.rem]*p,7*quotient.quot));
}

MagickRealType inline DecodePixelGamma(const MagickRealType pixel)
{
  if (pixel <= (0.0404482362771076*QuantumRange))
    return(pixel/12.92f);
  return((MagickRealType) (QuantumRange*DecodeGamma((double) (QuantumScale*
    pixel+0.055)/1.055)));
}

static inline void ConvertRGBToXYZ(const Quantum red,const Quantum green,
  const Quantum blue,double *X,double *Y,double *Z)
{
  double
    b,
    g,
    r;

  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  r=QuantumScale*DecodePixelGamma((MagickRealType) red);
  g=QuantumScale*DecodePixelGamma((MagickRealType) green);
  b=QuantumScale*DecodePixelGamma((MagickRealType) blue);
  *X=0.4124564*r+0.3575761*g+0.1804375*b;
  *Y=0.2126729*r+0.7151522*g+0.0721750*b;
  *Z=0.0193339*r+0.1191920*g+0.9503041*b;
}

static inline void ConvertXYZToLab(const double X,const double Y,const double Z,
  double *L,double *a,double *b)
{
  double
    x,
    y,
    z;

  assert(L != (double *) NULL);
  assert(a != (double *) NULL);
  assert(b != (double *) NULL);
  if ((X/D65X) > CIEEpsilon)
    x=pow(X/D65X,1.0/3.0);
  else
    x=(CIEK*X/D65X+16.0)/116.0;
  if ((Y/D65Y) > CIEEpsilon)
    y=pow(Y/D65Y,1.0/3.0);
  else
    y=(CIEK*Y/D65Y+16.0)/116.0;
  if ((Z/D65Z) > CIEEpsilon)
    z=pow(Z/D65Z,1.0/3.0);
  else
    z=(CIEK*Z/D65Z+16.0)/116.0;
  *L=((116.0*y)-16.0)/100.0;
  *a=(500.0*(x-y))/255.0+0.5;
  *b=(200.0*(y-z))/255.0+0.5;
}

static inline void ConvertRGBToLab(const Quantum red,const Quantum green,
  const Quantum blue,double *L,double *a,double *b)
{
  double
    X,
    Y,
    Z;

  ConvertRGBToXYZ(red,green,blue,&X,&Y,&Z);
  ConvertXYZToLab(X,Y,Z,L,a,b);
}

}  // anonymous namespace

void fill_lab_a_channel(int n_pixels, uint8_t* lab_a_channel, const uint8_t* red, const uint8_t* green, const uint8_t* blue) {
    for (int i = 0; i < n_pixels; i++) {
        double L, a, b;
        ConvertRGBToLab(red[i], green[i], blue[i], &L, &a, &b);
        double a255 = a * 255;
        if (a255 < 0) {
            a255 = 0;
        } else if (a255 > 255) {
            a255 = 255;
        }
        lab_a_channel[i] = a255;
    }
}

}  // namespace find_trees
