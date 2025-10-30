#include "oSpark.h"

void
oSpark::InitializeShapeFunction(COUNT_T _ShapeFunction)
{
  switch ( _ShapeFunction )
  {
    case SF_HTC:
    {
      this->problemShape =  static_cast<ProblemShape*>( new HTCShape() );
      break;
    }
    case SF_SIMPLE:
    {
      this->problemShape =  static_cast<ProblemShape*>( new SimpleShape() );
      break;
    }

    default:
    {
      DOES_NOT_REACH();
    }
  }

  const PREC min = this->GetMinParam();
  const PREC max = this->GetMaxParam();
  this->GetProblemShape()->Initialize( _ShapeFunction, min, max );
}

void
oSpark::Initialize(PREC* _param,
                   PREC* _fitness,
                   PREC* _direction,
                   PREC* _shape_fitness,
                   PREC* _function_value,
                   COUNT_T _ShapeFunction)
{
  this->param = _param;
  this->fitness = _fitness;
  *this->fitness = PREC_MAX;
  this->amplitude = -1;
  this->direction =_direction;
  this->shape_fitness = _shape_fitness;
  this->functionValue = _function_value;
  this->SetShapeFitnessValue(INT_MAX);
  this->InitializeShapeFunction( _ShapeFunction );
}

void
oSpark::FitnessPostProcessing()
{
  this->GetProblemShape()->SetCalculation( OCT_SHAPE, NULL, NULL, this->GetParams(), this->GetParamCount() );
  this->SetShapeFitnessValue( this->GetProblemShape()->CalculateShapeFitness() );
 }
