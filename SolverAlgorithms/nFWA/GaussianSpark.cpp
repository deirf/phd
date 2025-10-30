#include "GaussianSpark.h"
#include "Firework.h"


void
GaussianSpark::Initialize(Boundaries _boundaries)
{
  Spark::Initialize(_boundaries);
  this->SetType(ST_GAUSSIAN);

  this->SetDirection( new TRecord(PREC)( this->GetParamCount() ) );
  this->InitRandomMovingProfileInArea();
}

void
GaussianSpark::DeInitialize()
{
  if (! this->GetInitialized() )
  {
    return;
  }

  Spark::DeInitialize();
  this->GetDirection()->free();
}

Spark*
GaussianSpark::SelectOneSpark()
{
  this->bestSparkWasPreviousSelected = not this->bestSparkWasPreviousSelected;

  if ( this->bestSparkWasPreviousSelected )
  {
    PREC selected_individual_spark = this->GetUniformRandom01();
    if ( selected_individual_spark > 0.9 )
    {
      COUNT_T fw_index = (COUNT_T)( 0.5 + this->GetUniformRandom( 0, (PREC)this->GetFireworks()->getCount() ) );
      return (Spark*)( this->GetFirework( fw_index )->GetBestSpark() );
    }
    if ( selected_individual_spark > 0.8 )
    {
      return (Spark*)( this->GetFirework()->GetBestSpark() );
    }
    // normal selection
  }

  return this->SelectOneSparkFromAll();
}

void
GaussianSpark::NextMovingProfile(Spark* _spark)
{
  this->SetMovingProfileOn();

  TRecord(PREC)* gaussianSparkPos = this->GetParamPtr();
  TRecord(PREC)* selectedSparkPos = _spark->GetParamPtr();
  this->CalculateDirection( gaussianSparkPos, selectedSparkPos );

  this->SetMovingBase( this->GetParamPtr() );
  this->GenerateDirectedParamByBase();
}

void
GaussianSpark::GenerateNextParam()
{
  Spark* spark = this->SelectOneSpark();
  this->NextMovingProfile(spark);
  this->SetSparkID(spark->GetID());
  this->SetSparkType(spark->GetType());
}
