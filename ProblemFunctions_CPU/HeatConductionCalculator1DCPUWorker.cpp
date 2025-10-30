#include <bitset>
#include <time.h>

#include "../common/Types.h"
#include "HeatConductionCalculator1DCPU.h"


inline
COUNT_T
FindGreaterXValuesLIN
                      (
                        const COUNT_T _start,
                        const COUNT_T _end,
                        const PREC _x,
                        const PREC* const restrict _x_values
                      )
{
  for (COUNT_T index = _start; index < _end; index++)
  {
    if ( _x_values[ index ] >= _x )
    {
      return index;
    }
  }
  DOES_NOT_REACH();
  return 0;
}

inline
COUNT_T
BinarySearch
                      (
                        const PREC* const restrict _values,
                        const COUNT_T _l,
                        const COUNT_T _r,
                        const PREC _x
                      )
{

  assume( _r >= _l, "r=%ld, l=%ld", _r, _l );

  const COUNT_T mid = _l + ( _r - _l ) / 2;

  if ( ( _values[ mid ] <= _x ) && ( _values[ mid + 1 ] > _x ) )
  {
    return mid + 1;
  }

  if ( _values[mid] >= _x )
  {
    return BinarySearch( _values, _l, mid - 1, _x); // left
  }

  return BinarySearch( _values, mid + 1, _r, _x);  // right
}

inline
COUNT_T
FindGreaterXValues
                      (
                        const COUNT_T _N,
                        const PREC _x,
                        const PREC* const restrict _x_values
                      )
{
  const uint LIN_LIMIT = 10;

  if ( _N <= LIN_LIMIT + 2 )
  {
    return FindGreaterXValuesLIN( 1, _N, _x, _x_values );
  }

  if ( _x < _x_values [ LIN_LIMIT ] )
  {
    return FindGreaterXValuesLIN( 1, LIN_LIMIT + 1, _x, _x_values );
  }

  if ( _x > _x_values [ _N - LIN_LIMIT ] )
  {
    return FindGreaterXValuesLIN( _N - LIN_LIMIT, _N, _x, _x_values );
  }

  return BinarySearch( _x_values, LIN_LIMIT, _N - LIN_LIMIT, _x);
}

#define LinearInterpolateCalc(x0, x1, y0, y1, x) (y0 + ((y1 - y0) * ((_x - x0) / PREC(x1 - x0))))

inline
PREC
QuickLinearInterpolation
                      (
                        const PREC _x,
                        const PREC _x0,
                        const PREC _x1,
                        const PREC _y0,
                        const PREC _y1
                      )
{
  assume( ( _x >= _x0 ) && ( _x <= _x1 ), "x=%lf, x0=%lf, x1=%lf", _x, _x0, _x1 );

  if ( _x == _x0 )
  {
    return _y0;
  }

  if ( _x == _x1 )
  {
    return _y1;
  }

  const PREC y = LinearInterpolateCalc(_x0, _x1, _y0, _y1, _x);

  assume( ((  y >= _y0 ) && (  y <= _y1 )) || ((  y >= _y1 ) && (  y <= _y0 )), "y=%lf, y0=%lf, y1=%lf", y, _y0, _y1 );

  return y;
}

inline
PREC
LinearInterpolation
                      (
                        const PREC _x,
                        const COUNT_T _N,
                        const PREC* const restrict _x_values,
                        const PREC* const restrict _y_values
                      )
{
  assume( ( _x >= _x_values[ 0 ] ) && ( _x <= _x_values[ _N - 1 ] ), "x=%lf, x_values[0]=%lf, x_values[N-1]=%lf",_x, _x_values[ 0 ], _x_values[ _N - 1 ] );

  if ( _x == _x_values[ 0 ] )
  {
    return _y_values[ 0 ];
  }

  if ( _x == _x_values[ _N - 1 ] )
  {
    return _y_values[ _N - 1 ];
  }

  const COUNT_T index = FindGreaterXValues(_N, _x, _x_values );

  const PREC x0 = _x_values[ index - 1 ];
  const PREC y0 = _y_values[ index - 1 ];
  const PREC x1 = _x_values[ index ];
  const PREC y1 = _y_values[ index ];

  const PREC y = LinearInterpolateCalc(x0, x1, y0, y1, _x);

  assume( (  _x >= x0 ) && ( _x <= x1 ), "index=%ld, x=%lf, x0=%lf, x1=%lf", index, _x, x0, x1 );
  assume( ((  y >= y0 ) && (  y <= y1 )) || ((  y >= y1 ) && (  y <= y0 )), "index=%ld, y=%lf, y0=%lf, y1=%lf", index, y, y0, y1 );

  return y;
}

inline
PREC
CalculateK( const PREC T )
{
  const PREC KPLUS = (PREC)13.7129;
  const PREC KMULT = (PREC)0.017;

  return KPLUS + KMULT * T;
}

inline
PREC
CalculateCp( const PREC T )
{
  const PREC CpPLUS = (PREC)333.73;
  const PREC CpMULT = (PREC)0.2762;

  return CpPLUS + CpMULT * T;
}

inline
PREC
CalculateRho( const PREC T )
{
  const PREC RhoPLUS = (PREC)7925.4;
  const PREC RhoMULT = (PREC)0.4434;

  return RhoPLUS - RhoMULT * T;
}

inline
PREC
CalculateAlpha( const PREC temperature )
{
  const PREC k = CalculateK(temperature);
  const PREC cp = CalculateCp(temperature);
  const PREC rho = CalculateRho(temperature);

  return k / (cp * rho);
}



void
CalculateHeatFunction
                    (
                      const PREC _htc,
                      HeatCalcParams* const restrict _heatCalcParams
                    )
{
  const PREC* temperatures = _heatCalcParams->temperatures;
  const COUNT_T lastTemperatureIndex = _heatCalcParams->lastTemperatureIndex;
  const PREC c1pK_last_m2 = 1 / (2 * CalculateK( temperatures[ lastTemperatureIndex ]));
  const PREC alpha_last = CalculateAlpha( temperatures[ lastTemperatureIndex ] );
  const PREC alpha_zero = CalculateAlpha( temperatures[ 0 ] );
  const PREC prevTemp_zero = temperatures[ 0 ];
  const PREC prevTemp_one = temperatures[ 1 ];
  const PREC prevTemp_last = temperatures[ lastTemperatureIndex ];
  const PREC prevTemp_lastM1 = temperatures[ lastTemperatureIndex - 1 ];
  const PREC decrement_T  = ( prevTemp_last - _heatCalcParams->finalTemperature ) * _htc * _heatCalcParams->decTmod * c1pK_last_m2;

  const PREC temp_zero =
    prevTemp_zero + _heatCalcParams->dtdx2m2 * alpha_zero * ( prevTemp_one - prevTemp_zero );

  const PREC temp_last =
    prevTemp_last + _heatCalcParams->dtdx2m2 * alpha_last * ( prevTemp_lastM1 - prevTemp_last - decrement_T );

  PREC* nextTemperatures = _heatCalcParams->nextTemperatures;
  for (COUNT_T i = 1; i < lastTemperatureIndex; i++)
  {
    const PREC alpha_i = CalculateAlpha( temperatures[ i ]);
    const PREC prevTemp_i = temperatures[ i ];
    const PREC prevTemp_im1 = temperatures[ i - 1 ];
    const PREC prevTemp_ip1 = temperatures[ i + 1 ];
    const PREC c1pc1p2i = _heatCalcParams->c1pc1p2i[ i ];
    const PREC c1mc1p2i = _heatCalcParams->c1mc1p2i[ i ];

    const PREC temp_i =
      prevTemp_i + _heatCalcParams->dtpdx2 * alpha_i * ( prevTemp_ip1 * c1pc1p2i + prevTemp_im1 * c1mc1p2i - prevTemp_i * 2 );

    nextTemperatures[ i ] = temp_i;
  }

  nextTemperatures[ 0 ] = temp_zero;
  nextTemperatures[ lastTemperatureIndex ] = temp_last;
}

void
StoreData
        (
          PREC*  const restrict _heatFunctionValues,
          const COUNT_T _next_iteration_index,
          PREC*  const restrict _next_iteration_temperatures,
          const COUNT_T _temperatureLength,
          const PREC*  const restrict  _temperatures,
          COUNT_T* const _store_index,
          const COUNT_T _HF_index
        )
{
  _heatFunctionValues[ *_store_index ] = _temperatures[ _HF_index ];
  if UNLIKELY(*_store_index == _next_iteration_index * 2)
  {
    memcpy( _next_iteration_temperatures, _temperatures, _temperatureLength * sizeof(PREC));
  }

  (*_store_index)++;
}

void
CalculateHeatParams
                  (
                    HeatCalcParams* const restrict _heatCalcParams,
                    const COUNT_T ref_time_index
                  )
{
    CalculateHeatFunction( _heatCalcParams->interpolated_htc[ ref_time_index ], _heatCalcParams );
    SWAP( _heatCalcParams->temperatures, _heatCalcParams->nextTemperatures );
}

void
CalculateHeatFunctionWorker
                          (
                            HeatCalcParams* const restrict _heatCalcParams
                          )
{
  PREC time = 0;
  COUNT_T ref_time_index = 0;
  COUNT_T store_index = 0;
  PREC reference_time = 0;

  if ( _heatCalcParams->start_iteration_index != COUNT_T_NEG )
  {
    store_index = _heatCalcParams->start_iteration_index;
    memcpy( _heatCalcParams->temperatures, _heatCalcParams->start_iteration_temperatures, _heatCalcParams->temperatureLength * sizeof(PREC) );
    StoreData(
                                     _heatCalcParams->heatFunctionValues,
                                     _heatCalcParams->next_iteration_index,
                                     _heatCalcParams->next_iteration_temperatures,
                                     _heatCalcParams->temperatureLength,
                                     _heatCalcParams->start_iteration_temperatures,
                                     &store_index,
                                     _heatCalcParams->TCIndex
                                    );

    time = _heatCalcParams->referenceFunctionTimes[ _heatCalcParams->start_iteration_index ];
    ref_time_index = _heatCalcParams->start_iteration_index;
  }
  else
  {
    FILL( _heatCalcParams->temperatures, _heatCalcParams->temperatureLength, _heatCalcParams->initialTemperature );
    StoreData(
                                     _heatCalcParams->heatFunctionValues,
                                     _heatCalcParams->next_iteration_index,
                                     _heatCalcParams->next_iteration_temperatures,
                                     _heatCalcParams->temperatureLength,
                                     _heatCalcParams->temperatures,
                                     &store_index,
                                     _heatCalcParams->TCIndex
                                    );

    time = _heatCalcParams->delta_t;
  }

  reference_time = _heatCalcParams->referenceFunctionTimes[ ++ref_time_index ];
  while ( time < _heatCalcParams->endTime )
  {
    CalculateHeatFunction( _heatCalcParams->interpolated_htc[ ref_time_index ], _heatCalcParams );

    if (UNLIKELY( time + _heatCalcParams->delta_t > reference_time ) )
    {
      assume( reference_time > time, "reference_time is NOT bigger than time; reference_time='%lf', time='%lf', delta_t='%lf'", reference_time, time, _heatCalcParams->delta_t);

      StoreData(
                 _heatCalcParams->heatFunctionValues,
                 _heatCalcParams->next_iteration_index,
                 _heatCalcParams->next_iteration_temperatures,
                 _heatCalcParams->temperatureLength,
                 _heatCalcParams->temperatures,
                 &store_index,
                 _heatCalcParams->TCIndex
                );

      time = reference_time;
      reference_time = ++ref_time_index < _heatCalcParams->referenceFunctionN ?
                       _heatCalcParams->referenceFunctionTimes[ ref_time_index ]: time + _heatCalcParams->delta_t;
      assume( reference_time > time, "reference_time is NOT bigger than time; ref_time_index='%ld', reference_time='%lf', time='%lf'", ref_time_index, reference_time, time);
    }

    SWAP( _heatCalcParams->temperatures, _heatCalcParams->nextTemperatures );
    time += _heatCalcParams->delta_t;
  }

  assume(ref_time_index == _heatCalcParams->referenceFunctionN, "ref_time_index != _heatCalcParams->referenceFunctionN, reftimeindex=%ld, _heatCalcParams->referenceFunctionN=%ld", ref_time_index, _heatCalcParams->referenceFunctionN);
}

inline
PREC
CalculateLossFunction
                    (
                      const PREC _diff
                    )
{
  return ABS( _diff );
}

void
CPUCalculateFitness
                (
                  HeatCalcParams* const restrict _heatCalcParams,
                  PREC* const restrict _heatFunctionValues,
                  PREC* const restrict _fitness,
                  PREC* const restrict _fitnessVector
                )
{
  *_fitness = 0;
  for (COUNT_T i = 0; i < _heatCalcParams->referenceFunctionN; i++)
  {
    _fitnessVector[ i ] = _heatFunctionValues[ i ] - _heatCalcParams->referenceFunctionValues[ i ];
    *_fitness += CalculateLossFunction( _fitnessVector[ i ] );
  }
}

void
CalculateTargetFunctionWorker
                            (
                              HeatCalcParams* const restrict _heatCalcParams,
                              const PREC* const restrict _htc,
                              PREC* const restrict _heatFunctionValues
                            )
{
  _heatCalcParams->heatFunctionValues = _heatFunctionValues;

  for ( COUNT_T ref_time_index = 0; ref_time_index < _heatCalcParams->referenceFunctionN; ++ref_time_index)
  {
    const PREC reference_time = _heatCalcParams->referenceFunctionTimes[ ref_time_index ];
    const PREC interpolated_htc_value =
                    LinearInterpolation( reference_time, _heatCalcParams->htcN, _heatCalcParams->HTCTimeFunction, _htc );
    _heatCalcParams->interpolated_htc[ ref_time_index ] = interpolated_htc_value;
  }

  CalculateHeatFunctionWorker( _heatCalcParams );
}

void
CPUCalculateTargetFunction
                      (
                        HeatCalcParams* const restrict _heatCalcParams,
                        PREC* const restrict _htc,
                        PREC* const restrict _heatFunctionValues
                      )
{
  CalculateTargetFunctionWorker( _heatCalcParams, _htc, _heatFunctionValues );
}

void
CPUHeatConductionWorker
                  (
                    HeatCalcParams* const restrict _heatCalcParams,
                    const COUNT_T _N,
                    PREC* const _fitnesses,
                    PREC* const _htcs,
                    PREC* const _heatFunctions,
                    PREC* const _fitnessFunctions
                  )
{
for (COUNT_T i = 0; i < _N; i++)
  {
    PREC* htc = _htcs + i * _heatCalcParams->htcN;
    PREC* heatFunctionValues = _heatFunctions + i * _heatCalcParams->referenceFunctionN;
    PREC* fitnessVector = _fitnessFunctions + i * _heatCalcParams->referenceFunctionN;
    PREC* fitness = _fitnesses + i;

    CalculateTargetFunctionWorker(_heatCalcParams, htc, heatFunctionValues);
    CPUCalculateFitness(_heatCalcParams, heatFunctionValues, fitness, fitnessVector);
  }
}
