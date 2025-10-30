#pragma once
#include <assert.h>
#include <iostream>
#include <stdio.h>

#define COUNT_T uint64_t

#define PREC float
#define PRECL(literal_value) literal_value##F

#define LIKELY(x)   (__builtin_expect((x),1))
#define UNLIKELY(x) (__builtin_expect((x),0))

#define SWAP(a, b) { PREC *temp = a; a = b; b = temp; }
#define FILL(p, len, val) (std::fill_n(p, len, val))

#ifndef __OPTIMIZE__
  #define assume(_condition, ...)                                                     \
    do {                                                                              \
      if ( UNLIKELY( !( _condition ) ) )                                              \
      {                                                                               \
        printf("\n\n  ");                                                             \
        printf( __VA_ARGS__ );                                                        \
        printf("\n  Assume checked is placed at (%s:%d in function:'%s')\n",          \
                        __FILE__, __LINE__, __FUNCTION__);                            \
        assert(false);                                                                \
      }                                                                               \
    } while (0)
#endif

#ifdef __OPTIMIZE__
  #define assume(_condition, ...) {}
#endif

typedef struct
{
  PREC initial_temperature;
  PREC final_temperature;
  PREC end_time;
  PREC R;
  PREC dx;
  PREC dx2;
  PREC dx4;
  PREC dtpdx2;
  PREC dtdx2m2;
  PREC delta_t;
  int TC;
  int N;
  int htc_N;
  int cooling_curve_N;

  PREC* htc_values;
  PREC* htc_times;

  PREC* cooling_curve_values;
  PREC* cooling_curve_times;

  PREC* c1pc1p2i;
  PREC* c1mc1p2i;
} Config;

typedef struct
{
  PREC initial_temperature;
  PREC final_temperature;
  PREC end_time;
  PREC R;
  int N;
  int TC;

  int cooling_curve_N;
  int htc_N;
  int TCIndex;
  int lastTemperatureIndex;
} GPUConfig;
