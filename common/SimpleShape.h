#pragma once
#include "ProblemShape.h"



class SimpleShape : public ProblemShape
{
private:
  PREC* moving_base = NULL;
  PREC* direction = NULL;
  PREC* param = NULL;

  PREC min_param = PREC_MIN;
  PREC max_param = PREC_MAX;
  COUNT_T param_N = 0;
  bool initialized = FALSE;

public:
  SimpleShape() {}
  ~SimpleShape() {}

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
                       PREC _fitness,
                       COUNT_T _param_N
                     ) override;

  void SetCalculation(
                       OffsetCalculationType _offsetCalculationType,
                       PREC* _moving_base,
                       PREC* _direction,
                       PREC* _param,
                       COUNT_T _param_N
                     ) override;

  COUNT_T GetParamCount() { return this->param_N; }
  PREC* GetParam() { return this->param; }
  PREC GetParam( COUNT_T _index ) { return this->param[ _index ]; }
  void SetParam( PREC* _param ) { this->param = _param; }
  void SetParam( COUNT_T _index, PREC _param ) { this->param[ _index ] = _param; }

  PREC GetMinParam() { return this->min_param; }
  PREC GetMaxParam() { return this->max_param; }

  PREC* GetMovingBase() { return this->moving_base; }
  PREC GetMovingBase( COUNT_T _index ) { return this->moving_base[ _index ]; }
  void SetMovingBase( PREC* _moving_base ) { this->moving_base = _moving_base; }

  void SetDirection( PREC* _direction ) { this->direction = _direction; }
  PREC* GetDirection() { return this->direction; }
  PREC GetDirection( COUNT_T _index ) { return this->direction[ _index ]; }

  void GenerateParamByBase() override;
  PREC CalculateShapeFitness() override { return 0; }

  PREC SimpleBoundaryCheck(PREC _new_pos);
  void SimpleSetParamValueOffsetCalculation(COUNT_T _index);
};