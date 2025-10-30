#include "EFwaDM.h"
#include "oSpark.h"


void
EFwaDM::Evaluate()
{
  this->BatchEvaluate();
  this->BestEvaluate();
  this->BestLogging();

  this->SavePopAsOld();
  this->CalculateSparkNumber();
  this->GenerateNextGeneration();

  this->BetterSelection();
  this->DifferentalMutation();
}

void
EFwaDM::SavePopAsOld()
{
  memcpy( this->old_params, this->spark_params, sizeof(PREC) * this->max_spark_number * this->param_N );
  memcpy( this->old_fitness, this->spark_fitness, sizeof(PREC) * this->max_spark_number );
}

void
EFwaDM::DifferentalMutation()
{
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    const COUNT_T random_spark1_id = (COUNT_T)round( this->GetUniformRandom( 0, (PREC)this->spark_number ) );
    const COUNT_T random_spark2_id = (COUNT_T)round( this->GetUniformRandom( 0, (PREC)this->spark_number ) );

    oSpark* random_spark1 = this->GetSpark( random_spark1_id );
    oSpark* random_spark2 = this->GetSpark( random_spark2_id );
    oSpark* spark = this->GetSpark( i );

    for(COUNT_T j = 0; j < this->param_N; j++)
    {
      const PREC step = scaleFactor * ( random_spark1->GetParamValue( j ) - random_spark2->GetParamValue( j ) );
      spark->SetDirectionValue( j, step );
    }

    spark->SetMovingBase(this->GetBestParams());
    spark->GetProblemShape()->SetCalculation( OCT_DIRECTED, spark->GetMovingBase(), spark->GetDirection(), spark->GetParams(), this->param_N );
    spark->GetProblemShape()->GenerateParamByBase();
  }
}

void
EFwaDM::BetterSelection()
{
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    if ( this->old_fitness[ i ] < this->sparks[ i ]->GetFitness() )
    {
      PREC* params = this->sparks[ i ]->GetParams();
      PREC* oldParams = this->GetOldParams(i);
      for (COUNT_T j = 0; j < this->param_N; j++)
      {
        params[ j ] = oldParams[ j ];
      }
    }
  }
}
