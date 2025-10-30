#include <cmath>
#include <algorithm>
#include "Particle.h"


void
Particle::MoveParticle()
{
  this->SetMovingBase(this->GetParam());

  this->GetProblemShape()->SetCalculation( OCT_DIRECTED, this->GetMovingBase(), this->GetVelocity(), this->GetParam(), this->GetParamCount() );
  this->GetProblemShape()->GenerateParamByBase();
}

void
Particle::InitializeShapeFunction(COUNT_T _ShapeFunction)
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
Particle::Initialize(COUNT_T _ShapeFunction)
{
  const PREC min = this->GetMinParam();
  const PREC max = this->GetMaxParam();

  for (COUNT_T i = 0; i < this->GetParamCount(); i++)
  {
    this->SetParamValue(i, this->GetUniformRandom(min, max));
    this->SetBestParamValue(i, this->GetParamValue(i));
    this->SetVelocityValue(i, 0);
  }

  this->SetBestFitnessValue(PREC_MAX);
  this->SetShapeFitnessValue(PREC_MAX);

  this->InitializeShapeFunction( _ShapeFunction );
}

void
Particle::FitnessPostProcessing()
{
  this->GetProblemShape()->SetCalculation( OCT_SHAPE, NULL, NULL, this->GetParam(), this->GetParamCount() );
  this->SetShapeFitnessValue( this->GetProblemShape()->CalculateShapeFitness() );
  if (this->GetFitnessValue() < this->GetBestFitnessValue())
  {
    this->SetBestFitnessValue(this->GetFitnessValue());

    for (COUNT_T i = 0; i < this->GetParamCount(); i++)
    {
      this->SetBestParamValue(i, this->GetParamValue(i));
    }
  }
}
