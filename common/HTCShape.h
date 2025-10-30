#pragma once
#include "ProblemShape.h"



class HTCShape : public ProblemShape
{
private:
  typedef PREC (HTCShape::*MaxAplitudeCalculationFunction)(COUNT_T _index, COUNT_T _max_index, int* _offset_direction);
  typedef PREC (HTCShape::*ParamOffsetCalculationFunction)(COUNT_T _index, COUNT_T _max_index);

  PREC shape_fitness = 0;
  MaxAplitudeCalculationFunction maxAmplitudeFunction = NULL;
  ParamOffsetCalculationFunction paramOffsetFunction = NULL;

  PREC* moving_base = NULL;
  PREC* direction = NULL;
  PREC* param = NULL;

  PREC min_param = PREC_MIN;
  PREC max_param = PREC_MAX;
  COUNT_T param_N = 0;
  bool initialized = FALSE;

  PREC fitness = PREC_MAX;

public:
  UniformRnd<PREC> randomUniform;
  NormalRnd<PREC> randomNormal;

  HTCShape() {}
  ~HTCShape() {}

  void Initialize(
                   COUNT_T _shapeFunction,
                   PREC _min_param,
                   PREC _max_param
                  ) override;


  void SetCalculation(
                       OffsetCalculationType _offsetCalculationType,
                       PREC* _moving_base,
                       PREC* _direction,
                       PREC* _param,
                       COUNT_T _param_N
                     ) override;

  void SetCalculation(
                       OffsetCalculationType _offsetCalculationType,
                       PREC* _moving_base,
                       PREC* _direction,
                       PREC* _param,
                       PREC _fitness,
                       COUNT_T _param_N
                     ) override;

  COUNT_T GetParamCount() { return this->param_N; }
  PREC* GetParam() { return this->param; }
  PREC GetParam( COUNT_T _index ) { return this->param[ _index ]; }
  void SetParam( PREC* _param ) { this->param = _param; }
  void SetParam( COUNT_T _index, PREC _param ) { this->param[ _index ] = _param; }

  void SetFitness( PREC _value ) { this->fitness = _value; }
  PREC GetFitness() { return this->fitness; }


  PREC GetMinParam() { return this->min_param; }
  PREC GetMaxParam() { return this->max_param; }

  void BoundaryCheck(PREC _base, PREC* _amplitude, int* _direction);

  PREC* GetMovingBase() { return this->moving_base; }
  PREC GetMovingBase( COUNT_T _index ) { return this->moving_base[ _index ]; }
  void SetMovingBase( PREC* _moving_base ) { this->moving_base = _moving_base; }

  void SetParamValueOffsetCalculation(COUNT_T _index, COUNT_T _max_index);

  void SetDirection( PREC* _direction ) { this->direction = _direction; }
  PREC* GetDirection() { return this->direction; }
  PREC GetDirection( COUNT_T _index ) { return this->direction[ _index ]; }

  COUNT_T FindMaxDataIndex();
  COUNT_T FindNextBiggerMovingBaseDataIndex(COUNT_T _start_index, int _inc);
  COUNT_T FindNextSmallerMovingBaseDataIndex(COUNT_T _start_index, int _inc);
  void CalculateMaxAmplitudeUpDownPoint(COUNT_T _index,
                                           COUNT_T _max_index,
                                           PREC* restrict _up_point,
                                           PREC* restrict _down_point
                                          );
  void CalculateMaxAmplitudeBorders(COUNT_T _index, COUNT_T _max_index, PREC* restrict _up_direction, PREC* restrict _down_direction);
  PREC CalculateMaxHTCAmplitude(COUNT_T _index, COUNT_T _max_index, int* _offset_direction);
  PREC CalculateMaxSimpleAmplitude(COUNT_T _index, COUNT_T _max_index, int* restrict _offset_direction);
  PREC CalculateOffset(COUNT_T _index, COUNT_T _max_index, PREC _real_amplitude, PREC _boundary_offset_coef, int _offset_direction);

  PREC CalculateParamDirectedRandomOffsetValue(COUNT_T _index, COUNT_T _max_index);
  PREC CalculateParamAmaxRandomOffsetValue(COUNT_T _index, COUNT_T _max_index);
  PREC CalculateParamDirectedOffsetValue(COUNT_T _index, COUNT_T _max_index);

  void GenerateParamByBase() override;
  PREC CalculateShapeFitness() override;

  int GetRandomDirection() { return randomUniform.getRandomDirection(); }
  bool GetRandomCoinSide() { return randomUniform.getRandomCoinSide(); }
  PREC GetUniformRandom01() { return randomUniform.getRand01(); }
  PREC GetUniformRandom(PREC min, PREC max)  { return randomUniform.getRand(min, max); }

  PREC GetNormalRandom()  { return randomNormal.getRand(); };
  PREC GetNormalRandom_0T1B()  { return randomNormal.getRand01_0T1B(); };
  PREC GetNormalRandom_1T0B()  { return randomNormal.getRand01_0B1T(); };
  PREC GetNormalRandom_0B1T2B()  { return randomNormal.getRand02_0B1T2B(); };
};
