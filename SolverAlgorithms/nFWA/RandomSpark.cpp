#include "RandomSpark.h"


void
RandomSpark::Initialize(Boundaries _boundaries)
{
  Spark::Initialize(_boundaries);
  this->SetType(ST_RANDOM);
  this->InitRandomMovingProfileInArea();
}

void
RandomSpark::CalculateAmaxFunction(TRecord(PREC)* _lastBestRandomRealAmax,
                                   // TRecord(PREC)* _lastBestRandomAmax,
                                   PREC _lastBestRandomFitness
                                  )
{
  assume( ( _lastBestRandomFitness > 0 ), "Wrong value; LastBestRandomFitness='%lf'", _lastBestRandomFitness);
  assume( _lastBestRandomRealAmax->getCount() == this->GetParamCount(), "Record count error; _lastBestRandomRealAmax='%ld', paramsCount='%ld'", _lastBestRandomRealAmax->getCount(), this->GetParamCount());


  if ( this->IsBest() ) // the best, which is a random spark
  {
    PREC amaxCoef = 1.2f;
    if ( this->IsPlato() )
    {
      amaxCoef = 0.8f;
    }

    for (COUNT_T i = 0; i < this->GetParamCount(); ++i)
    {
      const PREC amax =
        ( this->GetRealAmaxFunctionValue(i) /* \/ _lastBestRandomAmax->getData(i) */ )
        *
        ( this->GetFirework()->GetBestSpark()->GetFitness() / this->GetFitness() )

        // ( _lastBestRandomRealAmax->getData(i) /* \/ _lastBestRandomAmax->getData(i) */ )
        // *
        // ( this->GetFirework()->GetBestSpark()->GetFitness() / _lastBestRandomFitness )
      ;

      this->SetAmaxFunction( i, /* _lastBestRandomAmax->getData(i) * */ amax * amaxCoef );
    }
  }
  else
  {
    for (COUNT_T i = 0; i < this->GetParamCount(); ++i)
    {
      const PREC amaxCoef = 2.0f;
      const PREC amax = std::abs( this->GetParam(i) - this->GetFirework()->GetBestSpark()->GetParam(i) );
      this->SetAmaxFunction( i, amax * amaxCoef );
    }
  }
}

void
RandomSpark::NextMovingProfile()
{
  this->CalculateAmaxFunction(
                              this->GetFirework()->GetBestSpark()->GetCustomLocation( SCLT_BEST_BESTRANDOM_REAL_AMAX ),
                              // this->GetFirework()->GetBestSpark()->GetBestRandomAmax(),
                              this->GetFirework()->GetBestSpark()->GetBestRandomFitness()
                             );

  this->DefaultRandomMovingProfileAroundFw();
}

void
RandomSpark::GenerateNextParam()
{
  this->NextMovingProfile();
}
