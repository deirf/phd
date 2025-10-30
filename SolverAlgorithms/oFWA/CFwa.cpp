#include "CFwa.h"
#include "oSpark.h"


void
CFwa::MoveSparks()
{
  for(COUNT_T i = 1; i < this->spark_number; i++)
  {
    oSpark* spark = this->GetSpark( i );

    const PREC direction = this->GetRandomDirection();
    const PREC h = spark->GetAmplitude() * direction;

    for(COUNT_T j = 0; j < this->param_N; j++)
    {
      const PREC step = this->GetRandomCoinSide() ? h : 0;
      spark->SetDirectionValue( j, step );
    }

    spark->SetMovingBase(spark->GetParams());
    spark->GetProblemShape()->SetCalculation( OCT_DIRECTED, spark->GetMovingBase(), spark->GetDirection(), spark->GetParams(), this->param_N );
    spark->GetProblemShape()->GenerateParamByBase();
  }
}
