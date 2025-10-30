#include <cstring>
#include "SparkPopulation.h"
#include "Spark.h"

#include "../../common/LogQueueItem.h"
#include "../../common/ThreadPool.h"
#include "../../common/Types.h"
#include "Firework.h"
#include "SparkStore.h"
#include "Stats.h"

#include "FireworkSpark.h"
#include "RandomSpark.h"
#include "GaussianSpark.h"
#include "BestSpark.h"
#include "QuantumSpark.h"

std::mutex nFWABatchEvaluationMutex;

SparkPopulation::~SparkPopulation()
{
  delete this->sparkstore;
  delete[] this->evaluation_data_store;
  delete[] this->evaluation_fitness_store;
  delete[] this->evaluation_calculatedFunctionValue_store;
  delete[] this->evaluation_fitnessFunction_store;
  free(this->bestShapeParams);
}

void
SparkPopulation::Initialize(Fireworks* _fireworks, const Boundaries& _boundaries, COUNT_T _fw_index, COUNT_T _fw_pop_index)
{
  this->fireworks = _fireworks;
  this->fw_index = _fw_index;
  this->fw_pop_index = _fw_pop_index;
  this->boundaries = _boundaries;

  this->SetMaxNumberOfBests(MAX_BEST_SPARK_NUMBER);
  this->SetMaxNumberOfFireworks(MAX_FIREWORK_SPARK_NUMBER);
  this->SetMaxNumberOfRandomSparks( this->boundaries.random_spark_count );
  this->SetMaxNumberOfGaussianSparks( this->boundaries.gaussian_spark_count );
  this->SetMaxNumberOfQuantumSparks( this->boundaries.quantum_spark_count );

  // need to fix
  this->evaluation_data_store = new PREC[ this->GetMaxNumberOfAllSparks() * this->ctx.config.searching_dimension ]();
  this->evaluation_fitness_store = new PREC[ this->GetMaxNumberOfAllSparks() ]();
  this->evaluation_calculatedFunctionValue_store = new PREC[ this->GetMaxNumberOfAllSparks() * this->ctx.config.reference_function_N ]();
  this->evaluation_fitnessFunction_store = new PREC[ this->GetMaxNumberOfAllSparks() * this->ctx.config.reference_function_N ]();

  switch ( this->ctx.logLevel )
  {
    case LL_NO:
    case LL_BEST:
    default:
      break;

    case LL_BESTS:
    case LL_PARTICLE:
      std::string filename = this->ctx.config.nFWA.Log_file + "_" +
                             solver_algsName[ this->ctx.sa_index ] + "_" +
                             problem_funcsName[ this->ctx.pf_index ] + "_" +
                             this->ctx.starttime + "_" +
                             std::to_string( this->ctx.round ) + "_" +
                             std::to_string( this->fw_index );
      this->logindex = this->ctx.logthread->OpenFile(filename);
      break;
  }

  this->SetSparkStore( new SparkStore(
                                      this->fireworks,
                                      this->fw_index,
                                      this->fw_pop_index,
                                      this->ctx.config.searching_dimension,
                                      this->ctx.config.reference_function_N,
                                      this->ctx.config.reference_function_N)
                                     );
  this->bestShapeParams = new TRecord(PREC)( this->ctx.config.searching_dimension );
}

void
SparkPopulation::FitnessEvaluate()
{
  std::unique_lock<std::mutex> lck(nFWABatchEvaluationMutex);

  this->GetSparkStore()->GetParams()->copyAllData(this->evaluation_data_store, ACD_FROMARRAY);

  this->processor.BatchEvaluation(this->evaluation_data_store, this->GetSparkStore()->GetParamCount(),
                                  this->evaluation_fitness_store,
                                  this->GetNumberOfAllSparks(),
                                  this->evaluation_calculatedFunctionValue_store, this->GetSparkStore()->GetOutputFunctionSize(),
                                  this->evaluation_fitnessFunction_store, this->GetSparkStore()->GetFitnessFunctionSize()
                                 );

  this->IncCalculation( this->GetNumberOfAllSparks() );
  for (COUNT_T i = 0; i < this->GetNumberOfAllSparks(); ++i)
  {
    Spark* spark = this->GetSparkStore()->GetSpark(i);
    spark->SetShapeFitness(spark->CalculateShapeFitness());
    spark->SetFitness(this->evaluation_fitness_store[i]);
  }

  this->GetSparkStore()->GetCalculatedFunctionValue()->copyAllData(this->evaluation_calculatedFunctionValue_store, ACD_TOARRAY);
  this->GetSparkStore()->GetFitnessFunction()->copyAllData(this->evaluation_fitnessFunction_store, ACD_TOARRAY);
}

template <typename SparkVector> COUNT_T
SparkPopulation::FindBestSpark(SparkVector* _sparks)
{
  FoundBestSpark bestSpark;

  bestSpark.fitness = PREC_MAX;
  bestSpark.index = 0;

  if ( _sparks )
  {
    for(COUNT_T i = 0; i < _sparks->getCount(); i++)
    {
      if (_sparks->getData(i))
      {
        PREC fitness = _sparks->getData(i)->GetFitness();

        if(fitness < bestSpark.fitness)
          {
            bestSpark.fitness = fitness;
            bestSpark.index = i;
          }
      }
    }
  }

  if ( bestSpark.fitness == PREC_MAX )
  {
    bestSpark.index = _sparks->getCount();
  }

  return bestSpark.index;
}

void
SparkPopulation::CalculateBestIndex( COUNT_T* _bestRandomIndex, COUNT_T* _bestGaussianIndex, COUNT_T* _bestQuantumIndex )
{
  *_bestRandomIndex = this->FindBestSpark<TRandomSpark>(this->GetSparkStore()->GetRandomSparks());
  *_bestGaussianIndex = this->FindBestSpark<TGaussianSpark>(this->GetSparkStore()->GetGaussianSparks());
  *_bestQuantumIndex = this->FindBestSpark<TQuantumSpark>(this->GetSparkStore()->GetQuantumSparks());
}

void
SparkPopulation::SelectNewBestSpark(COUNT_T _bestRandomIndex, COUNT_T _bestGaussianIndex, COUNT_T _bestQuantumIndex)
{
  Spark* bestspark = this->GetSparkStore()->GetBestSpark();

  if ( bestspark->GetFitness() > this->GetSparkStore()->GetFireworkSpark()->GetFitness() )
  {
    bestspark = this->GetSparkStore()->GetFireworkSpark();
  }

  if ( ( this->GetNumberOfRandomSparks() > 0 ) && ( _bestRandomIndex <  this->GetSparkStore()->GetRandomSparks()->getCount() ) )
  {
    if ( bestspark->GetFitness() > this->GetSparkStore()->GetRandomSpark( _bestRandomIndex )->GetFitness() )
    {
      bestspark = this->GetSparkStore()->GetRandomSpark( _bestRandomIndex );
    }
  }

  if ( ( this->GetNumberOfGaussianSparks() > 0 ) && ( _bestGaussianIndex <  this->GetSparkStore()->GetGaussianSparks()->getCount() ) )
  {
    if ( bestspark->GetFitness() > this->GetSparkStore()->GetGaussianSpark( _bestGaussianIndex )->GetFitness() )
    {
      bestspark = this->GetSparkStore()->GetGaussianSpark( _bestGaussianIndex );
    }
  }

  if ( ( this->GetNumberOfQuantumSparks() > 0 ) && ( _bestQuantumIndex <  this->GetSparkStore()->GetQuantumSparks()->getCount() ) )
  {
    if ( bestspark->GetFitness() > this->GetSparkStore()->GetQuantumSpark( _bestQuantumIndex )->GetFitness() )
    {
      bestspark = this->GetSparkStore()->GetQuantumSpark( _bestQuantumIndex );
    }
  }
  this->GetSparkStore()->GetBestSpark()->SetNewBestSpark( bestspark );

  if ( ( this->GetNumberOfRandomSparks() > 0 )  && ( _bestRandomIndex <  this->GetSparkStore()->GetRandomSparks()->getCount() ) )
  {
    Spark* newBestRandomSpark = this->GetSparkStore()->GetRandomSpark( _bestRandomIndex );
    if (
        ( ! this->GetSparkStore()->GetBestSpark()->GetNewBestRandomSpark() ) ||
        ( this->GetSparkStore()->GetBestSpark()->GetNewBestRandomSpark()->GetFitness() < newBestRandomSpark->GetFitness() )
       )
    {
      this->GetSparkStore()->GetBestSpark()->SetNewBestRandomSpark( newBestRandomSpark );
    }
  }
}

void
SparkPopulation::PrintTheBest()
{
  if (
      ( ( this->GetIteration() ) % this->ctx.config.nFWA.status_print_iter == 0 )
      ||
      ( this->IsStopCondition() )
     )
  {
    printf("\r\033[%dCfw:%ld iter:%ld/%ld   best:%lf   sf:%ld%*c\r", PRINT_STAT_ITER_POS,
            this->fw_index,
            this->GetIteration(),
            this->GetMaxIteration(),
            this->GetBestFitness(),
            (COUNT_T)this->GetBestShapeFitness(),
            20, ' '
           );
  }
}

void
SparkPopulation::SelectNewBestShapeParam()
{
  if ( this->bestShapeParamFitness > this->GetBestShapeFitness() )
  {
    this->bestShapeParamFitness = this->GetBestShapeFitness();
    this->bestShapeParams->copyFrom( this->GetBestParam() );
  }

  if ( this->bestShapeParamFitness == 0 )
  {
    this->bestShapeParams->copyFrom( this->GetBestParam() );
  }
}

void
SparkPopulation::BestEvaluate()
{
  COUNT_T bestRandom;
  COUNT_T bestGaussian;
  COUNT_T bestQuantum;

  this->CalculateBestIndex( &bestRandom, &bestGaussian, &bestQuantum);
  this->SelectNewBestSpark( bestRandom, bestGaussian, bestQuantum);
}

bool
SparkPopulation::IsStopCondition()
{
  return (
           ( this->GetIteration() >= this->GetMaxIteration() )
           ||
           (
              ( this->GetBestShapeFitness() == 0 )
              &&
              ( this->GetBestFitness() < this->boundaries.min_fitness )
           )
         )
         ||
         ( simutation_stop_running );
}

void
SparkPopulation::RunSimulation(COUNT_T _fw_iter, const Boundaries& _boundaries)
{
  this->fw_iter = _fw_iter;
  this->boundaries = _boundaries;
  this->SetZeroIteration();

  while ( not this->IsStopCondition() )
  {
    this->IncIteration();
    this->StateEvaluate();
    this->PrintTheBest();
  }
}

template <typename SparkClass, typename SparkVector> SparkCalculationRepeatingState
SparkPopulation::CalculateSpark(SparkClass* _spark)
{
  SparkCalculationRepeatingState result = SCRS_NEXT;
  switch (this->GetCalculationState())
  {
    case SCS_BORN:
    {
      _spark->Initialize( this->boundaries );
      _spark->SetSubState(SSS_RUN);
      break;
    }

    case SCS_INIT_BEST_EVALUATE:
    {
      _spark->InitBestEvaluate();
      _spark->InitializeShapeFunction( this->boundaries.shapeFunction );
      break;
    }

    case SCS_INIT_POST_CALCULATION:
    {
      _spark->CheckAppliedMovingProfile();
      _spark->InitPostCalculation();
      _spark->SetSubState(SSS_FINISH);
      break;
    }

    case SCS_INIT_NEXT_STATE:
    {
      _spark->SetState(SS_FLOW);
      break;
    }



    case SCS_FLOW:
    {
      _spark->SetSubState(SSS_RUN);
      _spark->NextIteration();
      _spark->GenerateNextParam();
      break;
    }

    case SCS_BEST_EVALUATE:
    {
      _spark->BestEvaluate();
      break;
    }

    case SCS_POST_CALCULATION:
    {
      _spark->CheckAppliedMovingProfile();
      _spark->PostCalculation();
      _spark->BornChildSpark();

      COUNT_T substate = _spark->GetSubState() == SSS_RUN ? SSS_FINISH : _spark->GetSubState();
      _spark->SetSubState(substate);
      break;
    }

    case SCS_NEXT_STATE:
    {
      _spark->SetState(SS_FLOW);
      break;
    }



    case SCS_DIE:
    {
      _spark->SetSubState(SSS_DYING);
      _spark->NextIteration();
      _spark->Death();
      break;
    }

    case SCS_DIE_BEST_EVALUATE:
    {
      break;
    }

    case SCS_DIE_POST_CALCULATION:
    {
      result = this->GetSparkStore()->Kill<SparkClass, SparkVector>(_spark) ? SCRS_AGAIN_BY_KILL : result;
      break;
    }

    case SCS_DIE_NEXT_STATE:
    {
      break;
    }


    default:
    {
      DOES_NOT_REACH();
    }
  }

  return result;
}

bool
SparkPopulation::CheckStateToCalculate(Spark* _spark)
{
  switch (this->GetCalculationSubState())
  {
    case SCSS_MAIN:
      return ( _spark->GetSubState() == SSS_START );

    case SCSS_BEST_EVALUATE:
      return ( _spark->GetSubState() == SSS_RUN ) ||
             ( _spark->GetSubState() == SSS_DYING );

    case SCSS_POST_CALCULATION:
      return ( _spark->GetSubState() == SSS_RUN ) ||
             ( _spark->GetSubState() == SSS_DYING ) ||
             ( _spark->GetSubState() == SSS_DIED ) ;

    case SCSS_NEXT_STATE:
      return ( _spark->GetSubState() == SSS_FINISH );
  }

  return FALSE;
}

void
SparkPopulation::SetNextCalculationState(Spark* _spark)
{
  assume( _spark->GetState() < UINT_MAX, "Spark state error" );
  this->SetCalculationState( (uint)(_spark->GetState() - 1 ) * (SCSS_MAX - 1) + this->GetCalculationSubState() );
}

void
SparkPopulation::CalculateRandomSparks()
{
  COUNT_T index = 0;
  while ( index < this->GetNumberOfRandomSparks() )
  {
    RandomSpark* spark = this->GetSparkStore()->GetRandomSpark(index);
    this->SetNextCalculationState(spark);
    if ( this->CheckStateToCalculate(spark) )
    {
      const uint calc_result = this->CalculateSpark<RandomSpark, TRandomSpark>(spark);
      assume( calc_result == SCRS_NEXT, "State calc result is not next; state='%d'", calc_result );
    }
    index++;
  }
}

void
SparkPopulation::CalculateGaussianSparks()
{
  COUNT_T index = 0;
  while ( index < this->GetNumberOfGaussianSparks() )
  {
    GaussianSpark* spark = this->GetSparkStore()->GetGaussianSpark(index);

    this->SetNextCalculationState(spark);

    if (
        ( this->CheckStateToCalculate(spark) ) &&
        ( this->CalculateSpark<GaussianSpark, TGaussianSpark>(spark) == SCRS_AGAIN_BY_KILL )
       )
    {
      this->GetSparkStore()->Born<GaussianSpark, TGaussianSpark>( this->GetSparkStore()->GetGaussianSparks(), SBT_NORMAL );
      continue;
    }
    index++;
  }
}

void
SparkPopulation::CalculateQuantumSparks()
{
  COUNT_T index = 0;
  while ( index < this->GetNumberOfQuantumSparks() )
  {
    QuantumSpark* spark = this->GetSparkStore()->GetQuantumSpark(index);

    this->SetNextCalculationState(spark);

    if ( this->CheckStateToCalculate(spark) )
    {
      const uint calc_result = this->CalculateSpark<QuantumSpark, TQuantumSpark>(spark);

      if ( calc_result == SCRS_AGAIN_BY_KILL )
      {
        this->GetSparkStore()->Born<QuantumSpark, TQuantumSpark>( this->GetSparkStore()->GetQuantumSparks(), SBT_NORMAL );
        continue;
      }
    }
    index++;
  }
}

void
SparkPopulation::CalculateFireworkSparks()
{
  if ( this->GetNumberOfFireworkSparks() == 0 )
  {
    return;
  }

  FireworkSpark* spark = this->GetSparkStore()->GetFireworkSpark();
  this->SetNextCalculationState(spark);
  if ( this->CheckStateToCalculate(spark) )
  {
    this->CalculateSpark<FireworkSpark, TFireworkSpark>(spark);
  }
}

void
SparkPopulation::CalculateBestSpark()
{
  if ( this->GetNumberOfBestSparks() == 0 )
  {
    return;
  }

  BestSpark* spark = this->GetSparkStore()->GetBestSpark();
  this->SetNextCalculationState(spark);
  if ( this->CheckStateToCalculate(spark) )
  {
    this->CalculateSpark<BestSpark, TBestSpark>(spark);
  }
}

void
SparkPopulation::CalculateAllSpark(uint _state)
{
  this->SetCalculationSubState( _state );

  this->CalculateFireworkSparks();
  this->CalculateBestSpark();

  this->CalculateRandomSparks();
  this->CalculateGaussianSparks();
  this->CalculateQuantumSparks();
}

void
SparkPopulation::StateEvaluate()
{
  this->CalculateAllSpark(SCSS_MAIN); // set next positions

  this->FitnessEvaluate(); // calculate fitnesses
  this->BestEvaluate(); // select best sparks

  this->CalculateAllSpark(SCSS_BEST_EVALUATE); // set best spark

  this->CalculateAllSpark(SCSS_POST_CALCULATION); // calulcate post values
  this->SelectNewBestShapeParam();  // select the best shape fitness

  this->Logging( LL_PARTICLE + LL_BESTS );

  this->CalculateAllSpark(SCSS_NEXT_STATE); // set next state);
}

COUNT_T
SparkPopulation::GetIteration()
{
  return this->iter;
}

COUNT_T
SparkPopulation::GetMaxIteration()
{
  return this->boundaries.spark_max_iter;
}

PREC
SparkPopulation::GetBestFitness()
{
  return this->GetSparkStore()->GetBestSpark()->GetFitness();
}

PREC
SparkPopulation::GetBestShapeFitness()
{
  return this->GetSparkStore()->GetBestSpark()->GetShapeFitness();
}

TRecord(PREC)*
SparkPopulation::GetBestParam()
{
  return this->GetSparkStore()->GetBestSpark()->GetParamPtr();
}

void
SparkPopulation::PrepareLogging()
{
  const COUNT_T count = this->GetSparkStore()->GetParams()->getCount();

  assume(
         ( count == this->GetSparkStore()->GetPropertys()->getCount() ) &&
         ( count == this->GetSparkStore()->GetStates()->getCount() ) &&
         ( count == this->GetSparkStore()->GetStatistics()->getCount() ) &&
         ( count == this->GetSparkStore()->GetCustoms()->getCount() ) &&
         ( count == this->GetSparkStore()->GetCalculatedFunctionValue()->getCount() ) &&
         ( count == this->GetSparkStore()->GetFitnessFunction()->getCount() ) &&
         ( count == this->GetSparkStore()->GetRealAmaxFunction()->getCount() ) &&
         ( count == this->GetSparkStore()->GetCustomLocation()->getCount() )
         ,
         "Sparkstore counting error"
        );

  LogWorker* logworker = new LogWorker(this->ctx.logthread, QT_nFWA, this->logindex, LSAAI_MAX, LSLCAAI_MAX, LSLAAI_MAX, NFWA_LOGWORKER_INTS_COUNT);
  THREADPOOL(LogWorker::send, logworker,
             this->ctx.threadpools->LogWorkerThreadpool, THREADTYPE_LOG,
             "SPARK",
             count,

             this->GetSparkStore()->GetParams()->copyArray(),
             this->GetSparkStore()->GetPropertys()->copyArray(),
             this->GetSparkStore()->GetStatistics()->copyArray(),
             this->GetSparkStore()->GetCustoms()->copyArray(),
             this->GetSparkStore()->GetCalculatedFunctionValue()->copyArray(),
             this->GetSparkStore()->GetFitnessFunction()->copyArray(),
             this->GetSparkStore()->GetRealAmaxFunction()->copyArray(),

             this->GetSparkStore()->GetStates()->copyArray(),

             this->GetSparkStore()->GetCustomLocation()->copyArray(),

             this->iter,
             this->fw_iter
             );

  this->IncLogItemCount( count );
}


template <typename prec_type>
TArray(prec_type)*
SparkPopulation::ConvertBestRecordToArray(TRecord(prec_type)* _record)
{
  TArray(prec_type)* array = new TArray(prec_type)();
  array->addRecord( _record );
  return array;
}

TLArray(PREC)*
SparkPopulation::ConvertBestArrayToLArray(TArray(PREC)* _array)
{
  TLArray(PREC)* larray = new TLArray(PREC)();
  larray->addArray( _array );
  return larray;
}

void
SparkPopulation::PrepareBestLogging(const std::string& _desc)
{
  BestSpark* bestSpark = this->GetSparkStore()->GetBestSpark();
  LogWorker* logworker = new LogWorker(this->ctx.logthread, QT_nFWA, this->logindex, LSAAI_MAX, LSLCAAI_MAX, LSLAAI_MAX, NFWA_LOGWORKER_INTS_COUNT);
  THREADPOOL(LogWorker::send, logworker,
             this->ctx.threadpools->LogWorkerThreadpool, THREADTYPE_LOG,
             _desc,
             MAX_BEST_SPARK_NUMBER,

             this->ConvertBestRecordToArray<PREC>( bestSpark->GetParamPtr()->copy() ),
             this->ConvertBestRecordToArray<PREC>( bestSpark->GetPropertyPtr()->copy() ),
             this->ConvertBestRecordToArray<PREC>( bestSpark->GetStatisticPtr()->copy() ),
             this->ConvertBestRecordToArray<PREC>( bestSpark->GetCustomPtr()->copy() ),
             this->ConvertBestRecordToArray<PREC>( bestSpark->GetCalculatedFunctionValuePtr()->copy() ),
             this->ConvertBestRecordToArray<PREC>( bestSpark->GetFitnessFunctionPtr()->copy() ),
             this->ConvertBestRecordToArray<PREC>( bestSpark->GetRealAmaxFunctionPtr()->copy() ),

             this->ConvertBestRecordToArray<COUNT_T>( bestSpark->GetStatePtr()->copy() ),

             this->ConvertBestArrayToLArray( bestSpark->GetCustomLocationPtr()->copyArray() ),

             this->iter,
             this->fw_iter
             );

  this->IncLogItemCount( MAX_BEST_SPARK_NUMBER );
}

void
SparkPopulation::Logging( uint _logLevel )
{
  switch ( this->ctx.logLevel & _logLevel )
  {
    case LL_NO:
    case LL_BEST:
      break;

    case LL_BESTS:
      this->PrepareBestLogging("");
      break;

    case LL_PARTICLE:
      this->PrepareBestLogging("BEST");
      this->PrepareLogging();
      break;

    default:
      DOES_NOT_REACH();
  }
}

