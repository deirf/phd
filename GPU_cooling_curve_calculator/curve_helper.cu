#include "device_launch_parameters.h"
#include "gpu.h"

#define LinearInterpolateCalc(x0, x1, y0, y1, x) (y0 + ((y1 - y0) * ((x - x0) / PREC(x1 - x0))))

__device__ PREC
LinearInterpolationWithFixDx
                    (
                      const PREC* _x_values,
                      const PREC* _y_values,
                      const int _N,
                      const PREC _x
                    )
{
  assume( ( _x >= _x_values[ 0 ] ) && ( _x <= _x_values[ _N - 1 ] ), "_x:%lf is out ouf bounds", _x );

  const PREC c1pdx = 1 / (_x_values[1] - _x_values[0]);
  const PREC x_parcel = c1pdx * (_x - _x_values[0]);

  const int index = __float2int_rz(x_parcel);

  const PREC yp = _y_values[ index ];
  const PREC yp1 = _y_values[ index + 1];

  const PREC x_weight = x_parcel - truncf(x_parcel);
  const PREC y = (1.f - x_weight) * yp + x_weight * yp1;

  assume( ((  y == yp ) && (  y == yp1 ))|| ((  y >= yp ) && (  y <= yp1 )) || ((  y >= yp1 ) && (  y <= yp )), "y:%lf is out ouf bounds; yp:%lf; yp1:%lf", y, yp, yp1 );
  return ((yp == yp1) ? yp : y);
}


__device__ PREC
CalculateK(PREC T)
{
  return PRECL(13.7129) + PRECL(0.017) * T;
}

__device__ PREC
CalculateCp(PREC T)
{
  return PRECL(333.73) + PRECL(0.2762) * T;
}

__device__ PREC
CalculateRho(PREC T)
{
  return PRECL(7925.4) - PRECL(0.4434) * T;
}

__device__ PREC
CalculateAlpha(PREC _temperature)
{
  PREC k = CalculateK(_temperature);
  PREC cp = CalculateCp(_temperature);
  PREC rho = CalculateRho(_temperature);

  return k  / (cp * rho);
}

PREC
H_CalculateK(PREC T)
{
  return PRECL(13.7129) + PRECL(0.017) * T;
}

PREC
H_CalculateCp(PREC T)
{
  return PRECL(333.73) + PRECL(0.2762) * T;
}

PREC
H_CalculateRho(PREC T)
{
  return PRECL(7925.4) - PRECL(0.4434) * T;
}

PREC
H_CalculateAlpha(PREC _temperature)
{
  PREC k = H_CalculateK(_temperature);
  PREC cp = H_CalculateCp(_temperature);
  PREC rho = H_CalculateRho(_temperature);

  return k / (cp * rho);
}