#include "EFwa.h"
#include "oSpark.h"


void
EFwa::GenerateAmplitude()
{
  PREC sum = 0.0f;
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    sum += this->sparks[i]->GetFitness() - this->best_firework_fitness;
  }

  assume( ( representable_as_type<COUNT_T, PREC>(this->FunctionEvaluations) ), "PREC is not representable as COUNT_T" );
  const PREC amplitudo_init = ( (PREC)this->max_param - (PREC)this->min_param ) / 2.0f;
  const PREC Amin = amplitudo_init -
                    (PREC)((amplitudo_init - this->amplitudo_final) / (PREC)this->FunctionEvaluations *
                    sqrt((PREC)(2 * this->FunctionEvaluations - this->iteration) * (PREC)this->iteration));

  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    PREC amplitude = (PREC) ( amplitudo_init * (this->sparks[i]->GetFitness() - this->best_firework_fitness + EPSILON) / (sum + EPSILON));
    amplitude = !finite(amplitude) ? this->sparks[i]->GetAmplitude() : amplitude;
    amplitude = amplitude < Amin ? Amin : amplitude;

    this->sparks[i]->SetAmplitude(amplitude);
  }
}

void
EFwa::GaussianMutation()
{
  for (COUNT_T i = 1; i < this->spark_number; i++)
  {
    oSpark* spark = this->GetSpark( i );
    const PREC e = this->GetNormalRandom11();

    for(COUNT_T j = 0; j < this->param_N; j++)
    {
      const PREC best_distance = this->GetBestParams()[j] - spark->GetParamValue(j);
      const PREC step = this->GetRandomCoinSide() ? spark->GetAmplitude() * best_distance * e - spark->GetParamValue(j) : 0;
      spark->SetDirectionValue( j, step );
    }

    spark->SetMovingBase( spark->GetParams() );
    spark->GetProblemShape()->SetCalculation( OCT_DIRECTED, spark->GetMovingBase(), spark->GetDirection(), spark->GetParams(), this->param_N );
    spark->GetProblemShape()->GenerateParamByBase();
  }
}

void
EFwa::SparkSelections()
{
  for (COUNT_T i = 1; i < this->spark_number; i++)
  {
    if ( this->GetRandomCoinSide() )
    {
      this->GenerateRandomSparkParams(i);
    }
  }
}
