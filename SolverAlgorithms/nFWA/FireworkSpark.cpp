#include "FireworkSpark.h"
#include "BestSpark.h"
#include "Firework.h"
#include <cmath>


void
FireworkSpark::Initialize(Boundaries _boundaries)
{
  Spark::Initialize(_boundaries);
  this->SetType(ST_FIREWORK);

  if ( this->GetFirework()->GetSparkPopulation()->GetInitFireworkSparkHTCValues() )
  {
    this->SetMovingProfileOn();
    this->GetParamPtr()->copyFrom( this->GetFirework()->GetSparkPopulation()->GetInitFireworkSparkHTCValues(), this->GetParamCount() );
    this->SetMovingProfileOff();
  }
  else
  {
      this->InitRandomMovingProfileInArea();
  }

  this->SetAmax( FIREWORK_INIT_AMAX );
}

PREC
FireworkSpark::GetAmax()
{
  return this->GetAmaxFunction( 0 );
}

void
FireworkSpark::SetAmax(PREC _Amax)
{
  for (COUNT_T i = 0; i < this->GetParamCount(); ++i)
  {
    this->SetAmaxFunction( i, _Amax );
  }
}

void
FireworkSpark::NextMovingProfile()
{
  this->SetMovingProfileOn();

  if ( this->GetAmax() * 10 > this->GetFirework()->GetBestSpark()->GetFitness() )
  {
    this->SetAmax( this->GetAmax() / 10 );

    for (COUNT_T i = 0; i < this->GetParamCount(); ++i)
    {
      this->SetCustomLocationValue( SCLT_FIREWORK_AMAX, i, this->GetAmax() );
    }
  }

  this->SetMovingBase( this->GetFirework()->GetBestSpark()->GetParamPtr() );
  this->GenerateRandomParamByBase();
}

void
FireworkSpark::GenerateNextParam()
{
  this->NextMovingProfile();
}
