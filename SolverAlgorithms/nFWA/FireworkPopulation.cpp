#include "../../common/Types.h"
#include "../../common/Configuration.h"
#include "../../common/Array.h"
#include "../../common/LArray.h"
#include "../../common/Record.h"
#include "../../common/FileIOHelper.h"
#include "FireworkPopulation.h"
#include "Fireworks.h"
#include "BestSpark.h"

void
FireworkPopulation::DestroyFireworks()
{
  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
    delete this->GetFirework(fw_index);
  }
  this->GetFireworks()->GetFireworks()->free();
  delete this->GetFireworks();
}

Firework*
FireworkPopulation::GetFirework(COUNT_T _index)
{
  return this->GetFireworks()->GetFirework(_index);
}

void
FireworkPopulation::CreateFireworks()
{
  TFirework* firework_vector = new TFirework(this->GetFireworkCount());
  this->fireworks = new Fireworks(firework_vector, this->fw_pop_index);
  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
    Firework* firework = new Firework(this->ctx, this->processor);
    this->GetFireworks()->SetFirework(firework, fw_index);
  }
}

void
FireworkPopulation::Initialize(COUNT_T _fw_pop_index, const Boundaries& _boundaries)
{
  this->fw_pop_index = _fw_pop_index;
  this->boundaries = _boundaries;

  CreateFireworks();

  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
    this->GetFirework(fw_index)->Initialize(this->GetFireworks(), fw_index, this->fw_pop_index, this->boundaries);
  }

  const COUNT_T param_N = this->GetFirework(0)->GetBestSpark()->GetParamPtr()->getCount();
  avgParams = new TArray(PREC)( param_N, this->GetFireworkCount() );
  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
    this->avgParams->getRecordPtr( fw_index )->setCount( param_N );
  }

  switch ( this->ctx.logLevel )
  {
    case LL_NO:
    default:
      break;

    case LL_BEST:
    case LL_BESTS:
    case LL_PARTICLE:
      std::string filename = this->ctx.config.nFWA.Log_file + "_" +
                             solver_algsName[ this->ctx.sa_index ] + "_" +
                             problem_funcsName[ this->ctx.pf_index ] + "_" +
                             this->ctx.starttime;
      this->logindex = this->ctx.logthread->OpenFile(filename);
      FileIOHelper::copyConfig(this->ctx.config.config_filename, filename);
      break;
  }

  this->SelectTheBestFirework();
}

void
FireworkPopulation::PrintTheBest()
{
  printf("\r\033[%dCPop: iter:%ld/%ld   best:%lf    sf:%ld%*c\r", PRINT_STAT_POPULATION_POS,
          this->GetIteration() + 1,
          this->GetMaxIteration(),
          this->GetBestFitness(),
          (COUNT_T)this->GetBestShapeFitness(),
          20, ' '
         );
}

bool
FireworkPopulation::IsStopCondition()
{
  return (
          ( this->GetBestShapeFitness() == 0 ) &&
          ( this->GetBestFitness() < this->boundaries.min_fitness )
         ) ||
         ( this->GetIteration() >= this->GetMaxIteration() ) ||
         ( simutation_stop_running );
}

void
FireworkPopulation::RunSimulation()
{
  this->SetZeroIteration();

  while ( not this->IsStopCondition() )
  {
    this->IncIteration();
    this->Evaluate();
    this->SelectTheBestFirework();
    this->PrintTheBest();
    if ( not this->IsStopCondition() )
    {
      this->CalculateAVGShapeParams();
    }
  }

  this->SelectTheBestFirework();
  this->Logging( LL_PARTICLE + LL_BESTS + LL_BEST );
}

void
FireworkPopulation::Evaluate()
{
  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
    THREADPOOL(Firework::RunSimulation, this->GetFirework( fw_index ),
               this->ctx.threadpools->SolverFunctionThreadpool, THREADTYPE_SF,
               this->GetIteration(),
               this->boundaries
              );
  }

  this->ctx.threadpools->SolverFunctionThreadpool->wait_for_empty();
}


void
FireworkPopulation::CalculateAVGShapeParams()
{
  if ( this->GetFireworkCount() == 1 )
  {
    return;
  }

  const COUNT_T param_N = this->GetFirework(0)->GetBestShapeParams()->getCount();
  const PREC param_Nm1 = (PREC)param_N - 1;
  for (COUNT_T param_index = 0; param_index < param_N; param_index++)
  {
    for (COUNT_T selected_fw_index = 0; selected_fw_index < this->GetFireworkCount(); selected_fw_index++)
    {
      assume ( param_N == this->GetFirework( selected_fw_index )->GetBestShapeParams()->getCount(), "ParamsCounts are not fits" );

      PREC sum = 0;
      for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
      {
        const PREC param = fw_index == selected_fw_index ?
                           this->GetFirework(fw_index)->GetBestShapeParams()->getData( param_index ) * param_Nm1:
                           this->GetFirework(fw_index)->GetBestParam()->getData( param_index );
        sum += param;
      }
      const PREC avg = sum / (param_Nm1 * 2);

      this->avgParams->setData( selected_fw_index, param_index, avg );
    }
  }

  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
    TRecord(PREC)* newParamsRecord = this->avgParams->getRecordPtr( fw_index );
    newParamsRecord->setCount( param_N );
    TRecord(PREC)* oldParamsRecord = this->GetFirework( fw_index )->GetBestSpark()->ChangeParamsRecord( newParamsRecord );
    this->avgParams->setRecordPtr( oldParamsRecord, fw_index );
  }
}

void
FireworkPopulation::SelectTheBestFirework()
{
  TVector(COUNT_T) shapeFitnesses;

  // the best shape fitness
  PREC bestShapeFitness = PREC_MAX;
  shapeFitnesses.clear();
  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
    const PREC shapeFitness= this->GetFirework(fw_index)->GetBestShapeParamFitness();
    if ( shapeFitness < bestShapeFitness )
    {
      shapeFitnesses.clear();
      shapeFitnesses.emplace_back(fw_index);
      bestShapeFitness = shapeFitness;
    }
    else if ( shapeFitness == bestShapeFitness )
    {
      shapeFitnesses.emplace_back(fw_index);
    }

    assume( shapeFitnesses.size() > 0, "Nincs legjobb shape fitness??");
  }

  // the best fitness in the best shape
  COUNT_T bestIndex = 0;
  PREC bestFitness = PREC_MAX;
  for (COUNT_T index = 0; index < shapeFitnesses.size(); index++)
  {
    const COUNT_T fw_index = shapeFitnesses[index];
    if ( bestFitness > this->GetFirework( fw_index )->GetBestFitness() )
    {
      bestFitness = this->GetFirework( fw_index )->GetBestFitness();
      bestIndex = fw_index;
    }
  }

  this->SetBestFireworkIndex( bestIndex );
}

COUNT_T
FireworkPopulation::GetBestCalculation()
{
  return this->GetFirework( this->GetBestFireworkIndex() )->GetCalculation();
}

COUNT_T
FireworkPopulation::GetAllCalculations()
{
  COUNT_T calc = 0;
  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
    calc += this->GetFirework(fw_index)->GetCalculation();
  }

  return calc;
}

COUNT_T
FireworkPopulation::GetAllFWLogItemCount()
{
  COUNT_T calc = 0;
  for (COUNT_T fw_index = 0; fw_index < this->GetFireworkCount(); fw_index++)
  {
      calc += this->GetFirework( fw_index )->GetLogItemCount();
  }

  return calc;
}

PREC
FireworkPopulation::GetBestFitness()
{
  return this->GetFirework( this->GetBestFireworkIndex() )->GetBestFitness();
}

PREC
FireworkPopulation::GetBestShapeFitness()
{
  return this->GetFirework( this->GetBestFireworkIndex() )->GetBestShapeFitness();
}

TRecord(PREC)*
FireworkPopulation::GetBestParam()
{
  return this->GetFirework( this->GetBestFireworkIndex() )->GetBestParam();
}

template <typename prec_type>
TArray(prec_type)*
FireworkPopulation::ConvertBestRecordToArray(TRecord(prec_type)* _record)
{
  TArray(prec_type)* array = new TArray(prec_type)();
  array->addRecord( _record );
  return array;
}

TLArray(PREC)*
FireworkPopulation::ConvertBestArrayToLArray(TArray(PREC)* _array)
{
  TLArray(PREC)* larray = new TLArray(PREC)();
  larray->addArray( _array );
  return larray;
}

void
FireworkPopulation::PrepareBestLogging()
{
  BestSpark* bestSpark = this->GetFirework( this->GetBestFireworkIndex() )->GetBestSpark();

  LogWorker* logworker = new LogWorker(this->ctx.logthread, QT_nFWA, this->logindex, LSAAI_MAX, LSLCAAI_MAX,LSLAAI_MAX, NFWA_LOGWORKER_INTS_COUNT);
  THREADPOOL(LogWorker::send, logworker,
             this->ctx.threadpools->LogWorkerThreadpool, THREADTYPE_LOG,
             "",
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

             this->GetFirework( this->GetBestFireworkIndex() )->GetIteration(),
             this->GetIteration()
             );

  this->IncLogItemCount( MAX_BEST_SPARK_NUMBER );
}

void
FireworkPopulation::Logging( uint _logLevel )
{
  switch ( this->ctx.logLevel & _logLevel )
  {
    case LL_NO:
      break;

    case LL_BEST:
    case LL_BESTS:
    case LL_PARTICLE:
      this->PrepareBestLogging();
      break;

    default:
      DOES_NOT_REACH();
  }
}