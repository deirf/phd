#include "QuantumSpark.h"
#include "BestSpark.h"
#include "FireworkSpark.h"
#include "../../common/Record.h"
#include "../../common/Derivate.h"
#include "../../common/Types.h"


void
QuantumSpark::Initialize(Boundaries _boundaries)
{
  Spark::Initialize(_boundaries);
  this->SetType(ST_QUANTUM);
  this->SetAlpha(1.0f);

  this->SetPlannedPositionPtr( new TRecord(PREC)( this->GetParamCount() ));
  this->GetPlannedPositionPtr()->setCount( this->GetParamCount() );
  this->SetDirection( new TRecord(PREC)( this->GetParamCount() ) );
  this->SetBestParamsAVGPtr( new TRecord(PREC)( this->GetParamCount() ));
  this->GetBestParamsAVGPtr()->setCount( this->GetParamCount() );

  this->InitRandomMovingProfileInArea();
}

void
QuantumSpark::DeInitialize()
{
  if (! this->GetInitialized() )
  {
    return;
  }

  Spark::DeInitialize();
  this->GetDirection()->free();
  this->GetBestParamsAVGPtr()->free();
}

void
QuantumSpark::NextMovingProfile()
{
  this->SetMovingProfileOn();

  for (COUNT_T index = 0; index < this->GetParamCount(); index++)
  {
    this->GetPlannedPositionPtr()->setData(index, this->CalculateNewPosition( index ));
  }

  TRecord(PREC)* sparkPos = this->GetParamPtr();
  this->CalculateDirection( sparkPos, this->GetPlannedPositionPtr() );

  this->SetMovingBase( this->GetParamPtr() );
  this->GenerateDirectedParamByBase();
}

void
QuantumSpark::GenerateNextParam()
{
  this->NextMovingProfile();
}

PREC
QuantumSpark::CalculateNewPosition(COUNT_T _index)
{
  const PREC globalBest = this->GetBestSpark()->GetParam( _index );
  const PREC localBest = this->GetBestParam( _index );
  const PREC p = this->GetParam( _index );
  const PREC C = this->GetBestParamsAVG( _index );
  const PREC fi = this->GetUniformRandom01();
  const PREC mixedBest = fi * localBest + ( 1 - fi ) * globalBest;
  const PREC rand = this->GetUniformRandom01();
  const PREC direction = (PREC)this->GetRandomDirection();
  const PREC amplitude = (PREC)(std::abs( p - C ) * log( 1 / rand ));
  const PREC alpha = this->UpdateAlpha();

  return mixedBest + direction * alpha * amplitude;
}

void
QuantumSpark::CalculateBestParamAVG()
{
  memset( (void*)this->GetBestParamsAVGPtr()->getRecordDataPtr(), 0, sizeof(PREC) * this->GetParamCount() );

  for ( COUNT_T s_index = 0; s_index < this->GetNumberOfQuantumSparks(); s_index++ )
  {
    for (COUNT_T p_index = 0; p_index < this->GetParamCount(); p_index++)
    {
      this->SetBestParamsAVG(p_index, this->GetBestParamsAVG(p_index) + this->GetSparkStore()->GetQuantumSpark(s_index)->GetBestParam(p_index) );
    }
  }

  for (COUNT_T p_index = 0; p_index < this->GetParamCount(); p_index++)
  {
    this->SetBestParamsAVG(p_index, this->GetBestParamsAVG(p_index) + this->GetSparkStore()->GetFireworkSpark()->GetParam(p_index) );
  }


  for (COUNT_T p_index = 0; p_index < this->GetParamCount(); p_index++)
  {
    this->SetBestParamsAVG(p_index, this->GetBestParamsAVG(p_index) / ( (PREC)this->GetNumberOfQuantumSparks() + 1 ) );
  }
}

void
QuantumSpark::UpdateLocalBest()
{
  this->SetBestFitness( this->GetFitness() );
  this->GetCustomLocation( SCLT_QUANTUM_BESTPARAM )->copyFrom( this->GetParamPtr() );

  for (COUNT_T index = 0; index < this->GetParamCount(); index++)
  {
    this->SetBestParam( index, this->GetParam( index ) );
  }
}

void
QuantumSpark::CallSpecialPostCalculation()
{
  if ( this->GetBestFitness() > this->GetFitness() )
  {
    this->UpdateLocalBest();
  }
  this->CalculateBestParamAVG();
}

void
QuantumSpark::CallSpecialInitPostCalculation()
{
  this->UpdateLocalBest();
  this->CalculateBestParamAVG();
}

PREC
QuantumSpark::UpdateAlpha()
{
  if ( this->IsBest() )
  {
    return this->GetAlpha();
  }

  const PREC diff = this->GetFitness() - this->GetBestSpark()->GetFitness();
  const PREC border = this->GetBestSpark()->GetFitness() * 0.1f;

  if ( diff > border )
  {
      this->SetAlpha(  this->GetAlpha() * 0.8f );
  }
  else
  {
      this->SetAlpha(  this->GetAlpha() * 1.2f );
  }

  return this->GetAlpha();
}
