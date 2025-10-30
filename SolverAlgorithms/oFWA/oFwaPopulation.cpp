#include "oFwaPopulation.h"
#include "../../common/Configuration.h"
#include "../../common/FileIOHelper.h"

oFwaPopulation::oFwaPopulation(Context& _ctx, ProblemFunctionProcessor& _processor) : ctx( _ctx ), processor ( _processor )
{
  this->max_firework_number = this->ctx.config.oFWA.max_firework_number;
  this->max_iteration = this->ctx.config.oFWA.max_firework_iteration;
  this->param_N = this->ctx.config.searching_dimension;
  this->min_fitness = this->ctx.config.oFWA.min_fitness;

  this->CreateFwas();
}

oFwaPopulation::~oFwaPopulation()
{
  this->DestroyFwas();
}

void
oFwaPopulation::CreateFwas()
{
  this->fwas_params = new PREC[ this->param_N * this->max_firework_number ]();
  this->fwas_fitness = new PREC[ this->max_firework_number ]();
  this->fwas_index = new COUNT_T[ this->max_firework_number ]();

  this->Fwas = new oFwa*[ this->max_firework_number ];
  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    switch (this->ctx.sa_index)
    {
      case SA_FWA:             // original FWA
        this->Fwas[i] = static_cast<oFwa*>( new oFwa(ctx, processor) );
        break;

      case SA_cFWA:           // conventional FWA
        this->Fwas[i] = static_cast<oFwa*>( new CFwa(ctx, processor) );
        break;

      case SA_AFWA:           // adaptive FWA
        this->Fwas[i] = static_cast<oFwa*>( new AFwa(ctx, processor) );
        break;

      case SA_EFWA:           // enhanced FWA
        this->Fwas[i] = static_cast<oFwa*>( new EFwa(ctx, processor) );
        break;

      case SA_EFWADM:         // FWA with differential mutation
        this->Fwas[i] = static_cast<oFwa*>( new EFwaDM(ctx, processor) );
        break;

      default:
        DOES_NOT_REACH();
    }
  }
}

void
oFwaPopulation::DestroyFwas()
{
  for (COUNT_T i = 0; i < this->max_firework_number; i++)
    delete this->Fwas[i];
  delete[] this->Fwas;

  delete[] this->fwas_params;
  delete[] this->fwas_index;
  delete[] this->fwas_fitness;
}

void
oFwaPopulation::Initialize()
{
  this->iteration = 0;

  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    this->Fwas[i]->Initialize(this->Fwas, i);
  }

  switch ( this->ctx.logLevel )
  {
    case LL_NO:
    default:
      break;

    case LL_BEST:
    case LL_BESTS:
    case LL_PARTICLE:
      std::string filename = this->ctx.config.oFWA.log_file + "_" +
                             solver_algsName[ this->ctx.sa_index ] + "_" +
                             problem_funcsName[ this->ctx.pf_index ] + "_" +
                             this->ctx.starttime;
      this->logindex = this->ctx.logthread->OpenFile(filename);
      FileIOHelper::copyConfig(this->ctx.config.config_filename, filename);
      break;
  }

  this->SetWinnerFWA();
}

void
oFwaPopulation::PrintTheBest()
{
  printf("\r\033[%dCPop: iter:%ld/%ld   best:%lf%*c\r", PRINT_STAT_POPULATION_POS,
          this->iteration,
          this->max_iteration,
          this->GetBestFitness(),
          20, ' '
         );
}

bool
oFwaPopulation::IsStopCondition()
{
  return ( this->iteration >= this->max_iteration ) ||
         ( this->GetBestFitness() < this->min_fitness ) ||
         ( simutation_stop_running );
}

void
oFwaPopulation::RunSimulation()
{
  this->iteration = 0;

  while ( not this->IsStopCondition() )
  {
    this->iteration++;
    this->Evaluate();
    this->PrintTheBest();
  }
  this->Logging( LL_BEST + LL_SPARK + LL_BESTS );
}

void
oFwaPopulation::Evaluate()
{
  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    THREADPOOL(oFwa::RunSimulation, this->Fwas[i],
               this->ctx.threadpools->SolverFunctionThreadpool, THREADTYPE_SF,
               this->iteration
               );
    // this->Fwas[i]->RunSimulation(this->iteration);
  }

  this->ctx.threadpools->SolverFunctionThreadpool->wait_for_empty();
  this->SetWinnerFWA();
}

void
oFwaPopulation::SetWinnerFWA()
{
  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    this->fwas_fitness[i] = this->Fwas[i]->GetBestFitness();
    this->fwas_index[i] = this->Fwas[i]->GetBestIndex();

    PREC* fwa_params = this->Fwas[i]->GetBestParams();
    for (COUNT_T j = 0; j < this->param_N; j++)
    {
      fwas_params[ i * this->param_N + j ] = fwa_params[j];
    }
  }

  this->best_fitness = PREC_MAX;
  this->best_index = 0;
  this->best_params = fwas_params;

  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    const PREC fitness = this->Fwas[i]->GetBestFitness();
    if (this->best_fitness > fitness)
    {
      this->best_fitness = fitness;
      this->best_index = i;
      this->best_params = &fwas_params[ i * param_N ];
    }
  }
}

COUNT_T
oFwaPopulation::GetCalculation( COUNT_T index )
{
  return this->Fwas[ index ]->GetCalculation();
}

COUNT_T
oFwaPopulation::GetIteration( COUNT_T index )
{
  return this->Fwas[ index ]->GetIteration();
}

COUNT_T
oFwaPopulation::GetCalculation()
{
  COUNT_T sum = 0;
  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    sum += this->GetCalculation( i );
  }
  return sum;
}

COUNT_T
oFwaPopulation::GetIteration()
{
  COUNT_T sum = 0;
  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    sum += this->GetIteration( i );
  }
  return sum;
}

COUNT_T
oFwaPopulation::GetLogItemCount( COUNT_T index )
{
  return this->Fwas[ index ]->GetLogItemCount();
}

COUNT_T
oFwaPopulation::GetLogItemCount()
{
  COUNT_T sum = 0;
  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    sum += this->GetLogItemCount( i );
  }
  return sum + this->logItemCount;
}


PREC
oFwaPopulation::GetBestFitness( COUNT_T index )
{
  return this->Fwas[ index ]->GetBestFitness();
}

PREC
oFwaPopulation::GetBestShapeFitness( COUNT_T index )
{
  return this->Fwas[ index ]->GetBestShapeFitness();
}

PREC
oFwaPopulation::GetBestShapeFitness()
{
  const COUNT_T bestFwIndex = this->GetBestIndex();
  return (PREC)this->GetBestShapeFitness( bestFwIndex );
}

PREC
oFwaPopulation::GetBestFitness()
{
  const COUNT_T bestFwIndex = this->GetBestIndex();
  return this->GetBestFitness( bestFwIndex );
}

PREC*
oFwaPopulation::GetBestParams( COUNT_T index )
{
  return this->Fwas[ index ]->GetBestParams();
}

PREC*
oFwaPopulation::GetBestFunctionValue( COUNT_T index )
{
  return this->Fwas[ index ]->GetBestFunctionValue();
}

PREC*
oFwaPopulation::GetBestParams()
{
  const COUNT_T bestFwIndex = this->GetBestIndex();
  return this->GetBestParams( bestFwIndex );
}

PREC*
oFwaPopulation::GetBestFunctionValue()
{
  const COUNT_T bestFwIndex = this->GetBestIndex();
  return this->GetBestFunctionValue( bestFwIndex );
}

COUNT_T
oFwaPopulation::GetBestIndex( COUNT_T index )
{
  return this->Fwas[ index ]->GetBestIndex();
}

COUNT_T
oFwaPopulation::GetBestIndex()
{
  PREC best_fitness_ = PREC_MAX;
  COUNT_T best_index_ = 0;
  for (COUNT_T i = 0; i < this->max_firework_number; i++)
  {
    if ( best_fitness_ > this->GetBestFitness( i ) )
    {
      best_fitness_ = this->GetBestFitness( i );
      best_index_ = i;
    }
  }
  assume( best_fitness_ < PREC_MAX, "Best fitness has not been set" );
  return best_index_;
}

PREC
oFwaPopulation::GetBestAmplitude( COUNT_T index )
{
  return this->Fwas[ index ]->GetBestAmplitude();
}

PREC
oFwaPopulation::GetBestAmplitude()
{
  const COUNT_T bestFwIndex = this->GetBestIndex();
  return this->Fwas[ bestFwIndex ]->GetBestAmplitude();
}

void
oFwaPopulation::ConvertOneArrayToTArray(TArray(PREC)* _to, PREC* _from, COUNT_T _len)
{
  COUNT_T recordIndex = _to->addRecord( _len );
  _to->copyData(_from, recordIndex, ACD_TOARRAY);
  _to->getRecordPtr(recordIndex)->setCount( _len );
}

TArray(PREC)*
oFwaPopulation::ConvertBestParamsArrayToTArray()
{
  TArray(PREC)* params = new TArray(PREC)();
  this->ConvertOneArrayToTArray(params, this->GetBestParams(), this->ctx.config.searching_dimension);
  return params;
}

void
oFwaPopulation::ConvertOneOtherArrayToTArray(TArray(PREC)* _others)
{
  COUNT_T recordIndex = _others->addRecord();
  _others->addData( recordIndex, SSP_FITNESS, this->GetBestFitness() );
  _others->addData( recordIndex, SSP_A, this->GetBestAmplitude() );
  _others->addData( recordIndex, SSP_FWAINDEX, (PREC)this->GetBestIndex() );
  _others->addData( recordIndex, SSP_SPARKINDEX, (PREC)this->GetBestIndex( this->GetBestIndex() ) );
  _others->addData( recordIndex, SSP_FWAITER, (PREC)this->ctx.config.oFWA.max_spark_iteration );
  _others->addData( recordIndex, SSP_POPITER, (PREC)this->ctx.config.oFWA.max_firework_iteration );
  _others->addData( recordIndex, SSP_TYPE, (PREC)SWT_BEST );
  _others->getRecordPtr( recordIndex )->setCount( SSP_MAX );
}

TArray(PREC)*
oFwaPopulation::ConvertBestOthersArrayToTArray()
{
  TArray(PREC)* others = new TArray(PREC)();
  this->ConvertOneOtherArrayToTArray(others);
  return others;
}

TArray(PREC)*
oFwaPopulation::ConvertBestFunctionValueArrayToTArray()
{
  TArray(PREC)* functionValue = new TArray(PREC)();
  this->ConvertOneArrayToTArray(functionValue, this->GetBestFunctionValue(), this->ctx.config.reference_function_N);
  return functionValue;
}

void
oFwaPopulation::PrepareLogging(TArray(PREC)* _params, TArray(PREC)* _others, TArray(PREC)* _functionValue)
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
oFwaPopulation::BestLogging()
{
  TArray(PREC)* params = ConvertBestParamsArrayToTArray();
  TArray(PREC)* others = ConvertBestOthersArrayToTArray();
  TArray(PREC)* functionValue = ConvertBestFunctionValueArrayToTArray();

  this->PrepareLogging(params, others, functionValue);
}

void
oFwaPopulation::Logging( uint _logLevel )
{
  UNUSED(_logLevel);
  this->BestLogging();
}
