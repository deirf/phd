#include "BestSpark.h"
#include "GaussianSpark.h"

void
BestSpark::SetNewBestSpark(Spark* _spark)
{
    this->newBestSpark = _spark;
    this->bestSparkChanged = ( _spark != static_cast<Spark*>( this ) );
}

void
BestSpark::Initialize(Boundaries _boundaries)
{
  Spark::Initialize(_boundaries);
  this->SetType( ST_BEST );
  this->SetBestRandomFitness( PREC_MAX );
  this->InitRandomMovingProfileInArea();
  this->SetITL(5);

  this->SetNewBestSpark( this );
  this->SetNewBestRandomSpark( NULL );
}

void
BestSpark::Reset()
{
  this->SetNewBestSpark( this );
  this->SetNewBestRandomSpark( NULL );
}

void
BestSpark::InitBestEvaluate()
{
  this->BestEvaluate();
  this->SetFirstFitness( this->GetFitness() );
}

void
BestSpark::BestEvaluate()
{
  this->SetMovingProfileOn();
  this->NextMovingProfile();
  this->SetMovingProfileOff();
}


void
BestSpark::NextMovingProfile()
{
  if ( !this->GetBestSparkChanged() )
  {
    return;
  }

  this->GetParamPtr()->copyFrom( this->GetNewBestSpark()->GetParamPtr() );
  this->GetCalculatedFunctionValuePtr()->copyFrom( this->GetNewBestSpark()->GetCalculatedFunctionValuePtr() );
  this->GetFitnessFunctionPtr()->copyFrom( this->GetNewBestSpark()->GetFitnessFunctionPtr() );
  this->SetFitness( this->GetNewBestSpark()->GetFitness() );
  this->SetShapeFitness( this->GetNewBestSpark()->GetShapeFitness() );
  this->SetPartnerID( this->GetNewBestSpark()->GetID() );
  this->SetPartnerIter( this->GetNewBestSpark()->GetIter() );
  this->SetPartnerType( this->GetNewBestSpark()->GetType() );

  this->SetParentPartnerID( (COUNT_T)0 );
  this->SetParentPartnerType( (COUNT_T)ST_UNKNOWN );

  if ( this->GetNewBestSpark()->GetType() ==  ST_GAUSSIAN )
  {
    this->SetParentPartnerID( static_cast<GaussianSpark*>( this->GetNewBestSpark() )->GetSparkID() );
    this->SetParentPartnerType( static_cast<GaussianSpark*>( this->GetNewBestSpark() )->GetSparkType() );
  }
}

void
BestSpark::GenerateNextParam()
{
}

void
BestSpark::CalculateBestRandomValues()
{
  if ( !this->GetNewBestRandomSpark() )
  {
    return;
  }

  this->SetBestRandomFitness( this->GetNewBestRandomSpark()->GetFitness() );
  this->GetCustomLocation( SCLT_BEST_BESTRANDOM_REAL_AMAX )->copyFrom( this->GetNewBestRandomSpark()->GetRealAmaxFunctionPtr() );

  this->GetCustomLocation( SCLT_BEST_AMAX )->copyFrom( this->GetNewBestSpark()->GetCustomLocation( SCLT_BEST_AMAX ) );
}

void
BestSpark::CallSpecialPostCalculation()
{
  this->GetCustomLocation( SCLT_BEST_SPARK_REAL_AMAX )->copyFrom( this->GetNewBestSpark()->GetRealAmaxFunctionPtr() );
  this->CalculateBestRandomValues();
}

void
BestSpark::CallSpecialInitPostCalculation()
{
  this->GetCustomLocation( SCLT_BEST_SPARK_REAL_AMAX )->copyFrom( this->GetNewBestSpark()->GetRealAmaxFunctionPtr() );
  this->CalculateBestRandomValues();
}
