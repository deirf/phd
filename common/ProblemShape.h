#pragma once
#include "Types.h"
#include "Configuration.h"
#include "Random.h"



class ProblemShape
{
public:
  typedef PREC (ProblemShape::*MaxAplitudeCalculationFunction)(COUNT_T _index, COUNT_T _max_index, int* _offset_direction);

  ProblemShape() {}
  ~ProblemShape() {}

  virtual void Initialize(
                           COUNT_T _shapeFunction,
                           PREC _min_param,
                           PREC _max_param
                         ) = 0;

  virtual void SetCalculation(
                               OffsetCalculationType _offsetCalculationType,
                               PREC* _moving_base,
                               PREC* _direction,
                               PREC* _param,
                               COUNT_T _param_N
                             ) = 0;

  virtual void SetCalculation(
                       OffsetCalculationType _offsetCalculationType,
                       PREC* _moving_base,
                       PREC* _direction,
                       PREC* _param,
                       PREC _fitness,
                       COUNT_T _param_N
                     ) = 0;

  void SetCalculation(
                       OffsetCalculationType _offsetCalculationType,
                       TRecord(PREC)* _moving_base,
                       TRecord(PREC)* _direction,
                       TRecord(PREC)* _param,
                       PREC _fitness,
                       COUNT_T _param_N
                     )
  {
    this->SetCalculation(
                          _offsetCalculationType,
                          _moving_base->getRecordDataPtr(),
                          _direction->getRecordDataPtr(),
                          _param->getRecordDataPtr(),
                          _fitness,
                          _param_N
                        );
  }

  void SetCalculation(
                       OffsetCalculationType _offsetCalculationType,
                       TRecord(PREC)* _moving_base,
                       TRecord(PREC)* _direction,
                       TRecord(PREC)* _param,
                       COUNT_T _param_N
                     )
  {
    if (_offsetCalculationType == OCT_SHAPE )
    {
      this->SetCalculation(
                            _offsetCalculationType,
                            NULL,
                            NULL,
                            _param->getRecordDataPtr(),
                            _param_N
                          );
      return;
    }

    this->SetCalculation(
                          _offsetCalculationType,
                          _moving_base->getRecordDataPtr(),
                          _direction->getRecordDataPtr(),
                          _param->getRecordDataPtr(),
                          _param_N
                        );
  }

  virtual void GenerateParamByBase() = 0;
  virtual PREC CalculateShapeFitness() = 0;
};
