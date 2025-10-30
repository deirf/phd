#include "oFwa.h"
#include "oSpark.h"
#include <stdarg.h>
#include "../../common/Array.h"
#include "../../common/Record.h"
#include "../../common/ThreadPool.h"

std::mutex oFWABatchEvaluationMutex;

oFwa::oFwa(Context& _ctx, ProblemFunctionProcessor& _processor) : ctx( _ctx ), processor( _processor )
{
  this->param_N = this->ctx.config.searching_dimension;
  this->max_param = this->ctx.config.max_param;
  this->min_param = this->ctx.config.min_param;
  this->min_fitness = this->ctx.config.oFWA.min_fitness;
  this->min_spark_number = this->ctx.config.oFWA.min_spark_number;
  this->max_spark_number = this->ctx.config.oFWA.max_spark_number;
  this->max_spark_iteration = this->ctx.config.oFWA.max_spark_iteration;
  this->max_firework_iteration = this->ctx.config.oFWA.max_firework_iteration;

  this->best_params = new PREC[ this->ctx.config.searching_dimension ]();
  this->distances = new PREC[ this->max_spark_number * this->max_spark_number ]();
  this->sumDistances = new PREC[ this->max_spark_number ]();
  this->probability = new PREC[ this->max_spark_number ]();
  this->best_functionValue = new PREC[ this->ctx.config.reference_function_N ]();

  this->evaluation_FunctionValue_store = new PREC[ this->max_spark_number * this->ctx.config.reference_function_N ]();
  this->evaluation_FitnessFunction_store = new PREC[ this->max_spark_number * this->ctx.config.reference_function_N ]();

  this->CreateSparks();
}

oFwa::~oFwa()
{
  ctx.threadpools->LogWorkerThreadpool->wait_for_empty();
  this->DestroySparks();

  delete[] this->best_params;
  delete[] this->best_functionValue;
  delete[] this->distances;
  delete[] this->sumDistances;
  delete[] this->probability;
  delete[] this->evaluation_FunctionValue_store;
  delete[] this->evaluation_FitnessFunction_store;
}

oSpark*
oFwa::CreateOneSpark(COUNT_T i)
{
  //for easy GPU usage
  return new oSpark(this->ctx.config, this->param_N, i, this->ctx.pf_index);
}

void
oFwa::CreateSparks()
{
  //for easy GPU usage
  this->spark_params = new PREC[ this->param_N * this->max_spark_number ]();
  this->spark_fitness = new PREC[ this->max_spark_number ]();
  this->direction = new PREC[ this->param_N * this->max_spark_number ]();
  this->shape_fitness = new PREC[ this->max_spark_number ]();

  this->sparks = new oSpark*[ this->max_spark_number ];
  for (COUNT_T i = 0; i < this->max_spark_number; i++)
  {
    this->SetSpark( i, this->CreateOneSpark( i )) ;
  }
}

void
oFwa::DestroySparks()
{
  for (COUNT_T i = 0; i < this->max_spark_number; i++)
  {
    delete this->GetSpark( i );
  }
  delete[] this->sparks;

  delete[] this->spark_fitness;
  delete[] this->spark_params;
  delete[] this->direction;
  delete[] this->shape_fitness;
}

void
oFwa::CalculateSparkNumber()
{
  const COUNT_T random_spark = (COUNT_T)this->GetUniformRandom( 0, (PREC)this->spark_number - 1 );
  const PREC up = this->worst_fitness - this->GetSpark( random_spark )->GetFitness() + EPSILON;

  PREC down = 0.0f;
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    down += this->worst_fitness - this->GetSpark( i )->GetFitness();
  }
  down += EPSILON;

  const COUNT_T __spark_number = (COUNT_T)round( (PREC)this->max_spark_number * up / down );
  const COUNT_T spark_number_with_min = __spark_number < this->min_spark_number ? this->min_spark_number : __spark_number;
  this->spark_number = __spark_number > this->max_spark_number ? this->max_spark_number : spark_number_with_min;
}

void
oFwa::Initialize(oFwa** _Fwas, COUNT_T _fwa_index)
{
  this->oFwas = _Fwas;
  this->fwa_index = _fwa_index;

  switch ( this->ctx.logLevel )
  {
    case LL_NO:
    case LL_BEST:
    default:
      break;

    case LL_BESTS:
    case LL_PARTICLE:
      std::string filename = this->ctx.config.oFWA.log_file + "_" +
                             solver_algsName[ this->ctx.sa_index ] + "_" +
                             problem_funcsName[ this->ctx.pf_index ] + "_" +
                             this->ctx.starttime + "_" +
                             std::to_string( this->ctx.round ) + "_" +
                             std::to_string( this->fwa_index );
      this->logindex = this->ctx.logthread->OpenFile(filename);
      break;
  }

  this->spark_number = this->max_spark_number;
  this->iteration = 0;

  for (COUNT_T i = 0; i < this->max_spark_number; i++)
  {
    PREC* param = this->spark_params + i * this->param_N;
    PREC* fitness = this->spark_fitness + i;
    PREC* Direction = this->direction + i * this->param_N;
    PREC* shapeFitness = this->shape_fitness + i;
    PREC* function_value = this->evaluation_FunctionValue_store + i * this->ctx.config.reference_function_N;

    this->GetSpark( i )->Initialize(param,
                                    fitness,
                                    Direction,
                                    shapeFitness,
                                    function_value,
                                    SF_SIMPLE);
  }

  this->GenerateSparks();

  for (COUNT_T i = 0; i < this->max_spark_number; i++)
  {
    this->GetSpark( i )->SetAmplitude(0);
  }

  this->firework_iteration = 0;
  this->iteration = 0;

  this->BatchEvaluate();
  this->BestEvaluate();
  this->Logging( LL_SPARK + LL_BESTS );

  for (COUNT_T i = 0; i < this->max_spark_number; i++)
  {
    this->GetSpark( i )->InitializeShapeFunction( this->ctx.config.WorkPiece.shapeFunction );
  }
}

void
oFwa::BatchEvaluate()
{
  std::unique_lock<std::mutex> lck(oFWABatchEvaluationMutex);

  processor.BatchEvaluation(
                            this->spark_params, this->param_N,
                            this->spark_fitness,
                            this->spark_number,
                            this->evaluation_FunctionValue_store, this->ctx.config.reference_function_N,
                            this->evaluation_FitnessFunction_store, this->ctx.config.reference_function_N
                           );

  this->calculation += this->spark_number;
}

void
oFwa::BestEvaluate()
{
  this->CalculateMinMaxFitness();

  memcpy(
         this->best_params,
         this->GetSpark( this->best_index )->GetParams(),
         this->param_N * sizeof(PREC)
        );

  memcpy(
         this->best_functionValue,
         this->GetSpark( this->best_index )->GetFunctionValue(),
         this->ctx.config.reference_function_N * sizeof(PREC)
        );

  this->FitnessPostProcessing();
  this->SelectBestFwaFitness();
}

void
oFwa::PrintTheBest()
{
  printf("\r\033[%dCfwa:%ld   iter:%ld/%ld    best:%lf    sf:%ld%*c\r", PRINT_STAT_ITER_POS,
          this->fwa_index,
          this->GetIteration(),
          this->max_spark_iteration,
          this->GetBestFitness(),
          (COUNT_T)this->GetBestShapeFitness(),
          50, ' '
         );
}

bool
oFwa::IsStopCondition()
{
  return ( this->iteration >= this->max_spark_iteration ) ||
         ( simutation_stop_running );
}

void
oFwa::RunSimulation(COUNT_T _firework_iteration)
{
  if (this->spark_number < 1)
    return;

  this->firework_iteration = _firework_iteration;
  this->iteration = 0;

  while ( not this->IsStopCondition() )
  {
    this->iteration++;
    this->Evaluate();

    if ( this->GetIteration() % this->ctx.config.oFWA.status_print_iter == 0 )
    {
      PrintTheBest();
    }
  }

  this->PrintTheBest();
}

void
oFwa::SelectBestFwaFitness()
{
  this->best_firework_fitness = PREC_MAX;
  for (COUNT_T i = 0; i < this->ctx.config.oFWA.max_firework_number; i++)
  {
    const PREC fitness = this->oFwas[i]->GetBestFitness();
    if (this->best_firework_fitness > fitness)
    {
      this->best_firework_fitness = fitness;
    }
  }
}

void
oFwa::GenerateRandomSparkParams(COUNT_T _index)
{
  for(COUNT_T k = 0; k < this->param_N; k++)
  {
    this->GetSpark( _index )->SetParamValue( k, this->GetUniformRandom( this->min_param, this->max_param ) );
  }
}

void
oFwa::GenerateSparks()
{
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    this->GenerateRandomSparkParams( i );
  }
}

void
oFwa::CalculateMinMaxFitness()
{
  for(COUNT_T i = 0; i < this->spark_number; i++)
  {
    const PREC fitness = this->GetSpark( i )->GetFitness();
    if(fitness < this->best_fitness)
    {
      this->best_index = i;
      this->best_fitness = fitness;
      this->best_amplitude = this->GetSpark( i )->GetAmplitude();
    }

    if(fitness > this->worst_fitness)
    {
      this->worst_fitness = fitness;
    }
  }
}

void
oFwa::FitnessPostProcessing()
{
  for(COUNT_T i = 0; i < this->spark_number; i++)
  {
    this->GetSpark( i )->FitnessPostProcessing();
  }
}

void
oFwa::GenerateAmplitude()
{
  PREC sum = 0.0f;
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    sum += this->GetSpark( i )->GetFitness() - this->best_firework_fitness;
  }

  const PREC amplitudo_init = ( this->max_param - this->min_param ) / 2;
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    if ( i == this->best_index ) continue;
    PREC amplitude = (PREC) ( amplitudo_init * (this->GetSpark( i )->GetFitness() - this->best_firework_fitness + EPSILON) / (sum + EPSILON));
    amplitude = finite(amplitude) ? amplitude : this->GetSpark( i )->GetAmplitude();
    assume( amplitude >= 0, "amplitude is negative:'%lf'", amplitude );
    this->GetSpark( i )->SetAmplitude( amplitude );
  }
}

void
oFwa::Evaluate()
{
  this->BatchEvaluate();
  this->BestEvaluate();

  this->Logging( LL_SPARK + LL_BESTS );

  this->CalculateSparkNumber();

  this->CalculateDistances();
  this->CalculateProbability();
  this->GenerateNextGeneration();

  this->GenerateAmplitude();
  this->MoveSparks();
  this->GaussianMutation();
}

void
oFwa::MoveSparks()
{
  for ( COUNT_T j = 0; j < this->spark_number; j++ )
  {
    if ( j == this->best_index ) continue;

    oSpark* spark = this->GetSpark( j );

    for ( COUNT_T k = 0; k < this->param_N; k++ )
    {
      const PREC direction = this->GetRandomDirection();
      const PREC step = this->GetRandomCoinSide() ? spark->GetAmplitude() * direction : 0;
      spark->SetDirectionValue( k, step );
    }

    spark->SetMovingBase(spark->GetParams());
    spark->GetProblemShape()->SetCalculation( OCT_DIRECTED, spark->GetMovingBase(), spark->GetDirection(), spark->GetParams(), this->param_N );
    spark->GetProblemShape()->GenerateParamByBase();
  }
}

void
oFwa::GaussianMutation()
{
  for ( COUNT_T i = 0; i < this->spark_number; i++ )
  {
    if ( i == this->best_index ) continue;

    oSpark* spark = this->GetSpark( i );
    const PREC g = this->GetNormalRandom11(); // mean = 1, variance = 1

    for ( COUNT_T j = 0; j < this->param_N; j++ )
    {
      const PREC step = this->GetRandomCoinSide() ? spark->GetParamValue(j) * ( g - 1 ) : 0;
      spark->SetDirectionValue( j, step );
    }

    spark->SetMovingBase(spark->GetParams());
    spark->GetProblemShape()->SetCalculation( OCT_DIRECTED, spark->GetMovingBase(), spark->GetDirection(), spark->GetParams(), this->param_N );
    spark->GetProblemShape()->GenerateParamByBase();
  }
}

PREC
oFwa::CalculateDistance(const PREC* _A, const PREC* _B)
{
  PREC sum = 0;
  for (COUNT_T i = 0; i < this->param_N; ++i)
  {
    const PREC distance = _A[i] - _B[i];
    sum += distance * distance;
  }

  return std::sqrt(sum);
}

void
oFwa::CalculateDistances()
{
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    for (COUNT_T j = 0; j < this->spark_number; j++)
    {
      if ( i == j )
      {
        this->distances[ j + this->spark_number * i ] = 0;
      }
      else
      {
        this->distances[ j + this->spark_number * i ] =
          this->CalculateDistance( this->GetSpark( i )->GetParams(), this->GetSpark( j )->GetParams() );
      }
    }
  }
}

void
oFwa::CalculateProbability()
{
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    this->sumDistances[ i ] = 0;
    for (COUNT_T j = 0; j < this->spark_number; j++)
    {
      this->sumDistances[ i ] += this->distances[ j + this->spark_number * i ];
    }
  }

  PREC sumSumDistances = 0;
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    sumSumDistances += this->sumDistances[ i ];
  }

  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    this->probability[ i ] = this->sumDistances[ i ] / sumSumDistances;
  }
}

void
oFwa::SparkSelections()
{
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    if ( i == this->best_index ) continue;

    if ( this->GetUniformRandom01() < this->probability[ i ] )
    {
      oSpark* spark = this->GetSpark( i );
      for ( COUNT_T j = 0; j < this->param_N; j++ )
      {
        const PREC direction = this->GetRandomDirection();
        const PREC step = this->GetRandomCoinSide() ? spark->GetAmplitude() * direction : 0;
        spark->SetDirectionValue( j, step );
      }

      oSpark* best_spark = this->GetSpark( this->best_index );
      spark->SetMovingBase(best_spark->GetParams());
      spark->GetProblemShape()->SetCalculation( OCT_DIRECTED, spark->GetMovingBase(), spark->GetDirection(), spark->GetParams(), this->param_N );
      spark->GetProblemShape()->GenerateParamByBase();
    }
  }
}

void
oFwa::GenerateNextGeneration()
{
  this->SparkSelections();
}

COUNT_T
oFwa::GetIteration()
{
  return this->iteration;
}

PREC
oFwa::GetBestAmplitude()
{
  return this->best_amplitude;
}

COUNT_T
oFwa::GetCalculation()
{
  return this->calculation;
}

PREC
oFwa::GetBestFitness()
{
  return this->best_fitness;
}

PREC
oFwa::GetBestShapeFitness()
{
  return this->GetSpark( this->GetBestIndex() )->GetShapeFitnessValue();
}

PREC*
oFwa::GetBestParams()
{
  return this->best_params;
}

COUNT_T
oFwa::GetBestIndex()
{
  return this->best_index;
}

PREC*
oFwa::GetBestFunctionValue()
{
  return this->best_functionValue;
}

void
oFwa::ConvertOneArrayToTArray(TArray(PREC)* _to, PREC* _from, COUNT_T _len)
{
  COUNT_T recordIndex = _to->addRecord( _len );
  _to->copyData(_from, recordIndex, ACD_TOARRAY);
  _to->getRecordPtr(recordIndex)->setCount( _len );
}

TArray(PREC)*
oFwa::ConvertParamsArrayToTArray()
{
  TArray(PREC)* params = new TArray(PREC)();

  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    this->ConvertOneArrayToTArray(params, this->GetSpark( i )->GetParams(), this->ctx.config.searching_dimension);
  }

  return params;
}

TArray(PREC)*
oFwa::ConvertBestParamsArrayToTArray()
{
  TArray(PREC)* params = new TArray(PREC)();
  this->ConvertOneArrayToTArray(params, this->GetSpark( this->best_index )->GetParams(), this->ctx.config.searching_dimension);
  return params;
}

TArray(PREC)*
oFwa::ConvertFunctionValueArrayToTArray()
{
  TArray(PREC)* functionValue = new TArray(PREC)();

  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    this->ConvertOneArrayToTArray(functionValue, this->GetSpark(i)->GetFunctionValue(), this->ctx.config.reference_function_N);
  }

  return functionValue;
}

TArray(PREC)*
oFwa::ConvertBestFunctionValueArrayToTArray()
{
  TArray(PREC)* functionValue = new TArray(PREC)();
  this->ConvertOneArrayToTArray(functionValue, this->GetBestFunctionValue(), this->ctx.config.reference_function_N);
  return functionValue;
}

void
oFwa::ConvertOneOtherArrayToTArray(TArray(PREC)* _others, COUNT_T _index, COUNT_T _type)
{
  COUNT_T recordIndex = _others->addRecord();
  _others->addData( recordIndex, SSP_FITNESS, this->GetSpark( _index )->GetFitness() );
  _others->addData( recordIndex, SSP_A, this->GetSpark(  _index )->GetAmplitude() );
  _others->addData( recordIndex, SSP_FWAINDEX, (PREC)this->fwa_index );
  _others->addData( recordIndex, SSP_SPARKINDEX, (PREC)this->GetSpark( _index )->GetIndex() );
  _others->addData( recordIndex, SSP_FWAITER, (PREC)this->iteration );
  _others->addData( recordIndex, SSP_POPITER, (PREC)this->firework_iteration );
  _others->addData( recordIndex, SSP_TYPE, (PREC)_type );
  _others->addData( recordIndex, SSP_SHAPE_FITNESS, (PREC)this->GetSpark( _index )->GetShapeFitnessValue() );
  _others->getRecordPtr( recordIndex )->setCount( SSP_MAX );
}

TArray(PREC)*
oFwa::ConvertOthersArrayToTArray()
{
  TArray(PREC)* others = new TArray(PREC)();

  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    this->ConvertOneOtherArrayToTArray(others, i, SWT_SPARK);
  }

  return others;
}

TArray(PREC)*
oFwa::ConvertBestOthersArrayToTArray()
{
  TArray(PREC)* others = new TArray(PREC)();
  this->ConvertOneOtherArrayToTArray(others, this->best_index, SWT_BEST);
  return others;
}

void
oFwa::PrepareLogging(TArray(PREC)* _params, TArray(PREC)* _others, TArray(PREC)* _functionValue)
{
  const COUNT_T count = _params->getCount();

  assume(
         ( _others->getCount() == _params->getCount() )
         ,
         "Others and params does not have the same count"
        );

  LogWorker* logworker = new LogWorker(this->ctx.logthread, QT_oFWA, this->logindex,
                                       OFWA_LOGWORKER_PREC_ARRAY_COUNT,
                                       OFWA_LOGWORKER_COUNT_ARRAY_COUNT,
                                       OFWA_LOGWORKER_LARRAY_COUNT,
                                       OFWA_LOGWORKER_INTS_COUNT);
  THREADPOOL(LogWorker::send, logworker,
             this->ctx.threadpools->LogWorkerThreadpool, THREADTYPE_LOG,
             "",
             count,
             _params,
             _others,
             _functionValue
             );

  this->IncLogItemCount( count );
}

void
oFwa::SparkLogging()
{
  TArray(PREC)* params = ConvertParamsArrayToTArray();
  TArray(PREC)* others = ConvertOthersArrayToTArray();
  TArray(PREC)* functionValue = ConvertFunctionValueArrayToTArray();

  this->PrepareLogging(params, others, functionValue);
}

void
oFwa::BestLogging()
{
  TArray(PREC)* params = ConvertBestParamsArrayToTArray();
  TArray(PREC)* others = ConvertBestOthersArrayToTArray();
  TArray(PREC)* functionValue = ConvertBestFunctionValueArrayToTArray();

  this->PrepareLogging(params, others, functionValue);
}

void
oFwa::Logging( uint _logLevel )
{
  switch( this->ctx.logLevel & _logLevel )
  {
    case LL_NO:
    case LL_BEST:
      break;

    case LL_BESTS:
      this->BestLogging();
      break;

    case LL_SPARK:
      this->BestLogging();
      this->SparkLogging();
      break;

    default:
      DOES_NOT_REACH();
  }
}
