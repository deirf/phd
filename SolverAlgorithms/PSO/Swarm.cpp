#include <stdarg.h>
#include "../../common/Array.h"
#include "../../common/Record.h"
#include "../../common/ThreadPool.h"
#include "../../common/FileIOHelper.h"
#include "Swarm.h"


Swarm::Swarm(Context& _ctx, ProblemFunctionProcessor& _processor) : ctx(_ctx), processor( _processor )
{
  this->swarm_size = _ctx.config.PSO.size;
  this->min_fitness = _ctx.config.PSO.min_fitness;

  assume(ARRAY_SIZE(SwarmPropertyTypeName) == SWP_MAX, "Array size and enum max element are not fits; MAX='%d'", SWP_MAX);
  assume(ARRAY_SIZE(SwarmPropertyDataTypeType) == SWP_MAX, "Array size and enum max element are not fits; MAX='%d'", SWP_MAX);

  this->CreateParticles();
}

Swarm::~Swarm()
{
  this->DestroyParticles();
}

void
Swarm::CreateParticles()
{
  particle_params = new PREC[ this->ctx.config.searching_dimension * this->swarm_size ]();
  particle_best_params = new PREC[ this->ctx.config.searching_dimension * this->swarm_size ]();
  particle_velocity = new PREC[ this->ctx.config.searching_dimension * this->swarm_size ]();
  particle_fitness = new PREC[ this->swarm_size ]();
  particle_best_fitness = new PREC[ this->swarm_size ]();
  particle_shape_fitness = new PREC[ this->swarm_size ]();
  best_params = new PREC[ this->ctx.config.searching_dimension ]();
  best_velo = new PREC[ this->ctx.config.searching_dimension ]();
  best_params_avg = NULL;
  best_evaluation_FunctionValue = new PREC[ this->ctx.config.reference_function_N ]();

  evaluation_FunctionValue_store = new PREC[ this->swarm_size * this->ctx.config.reference_function_N ]();
  evaluation_FitnessFunction_store = new PREC[ this->swarm_size * this->ctx.config.reference_function_N ]();

  // create and link particles
  this->particles = new Particle*[this->swarm_size]();
  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {

    switch (this->ctx.sa_index)
    {
      case SA_PSO:              // original PSO
        this->particles[i] = static_cast<Particle*>( new ParticlePSO(this->ctx.config, i, this->ctx.pf_index) );
        break;

      case SA_PSOIn:            // PSO In
        this->particles[i] = static_cast<Particle*>( new ParticlePSOIn(this->ctx.config, i, this->ctx.pf_index) );
        break;

      case SA_PSOCo:            // PSO Co
        this->particles[i] = static_cast<Particle*>( new ParticlePSOCo(this->ctx.config, i, this->ctx.pf_index) );
        break;

      case SA_QPSOT1:           // QPSO type 1
        this->particles[i] = static_cast<Particle*>( new ParticleQPSOT1(this->ctx.config, i, this->ctx.pf_index) );
        break;

      case SA_QPSOT2:          // QPSO Type 2
        this->best_params_avg = new PREC[ctx.config.searching_dimension]();
        this->particles[i] = static_cast<Particle*>( new ParticleQPSOT2(this->ctx.config, i, this->ctx.pf_index) );
        break;

      default:
        DOES_NOT_REACH();
    }

    this->particles[i]->SetParam(this->particle_params + i * this->ctx.config.searching_dimension);
    this->particles[i]->SetBestParam(this->particle_best_params + i * this->ctx.config.searching_dimension);
    this->particles[i]->SetVelocity(this->particle_velocity + i * this->ctx.config.searching_dimension);
    this->particles[i]->SetFitness(this->particle_fitness + i);
    this->particles[i]->SetShapeFitness(this->particle_shape_fitness + i);
    this->particles[i]->SetBestFitness(this->particle_best_fitness + i);
    this->particles[i]->SetFunctionValue(this->evaluation_FunctionValue_store + i * this->ctx.config.reference_function_N);
  }
}

void
Swarm::DestroyParticles()
{
  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    delete this->particles[i];
  }
  delete[] this->particles;

  delete[] this->best_params;
  delete[] this->best_velo;
  delete[] this->particle_best_fitness;
  delete[] this->particle_fitness;
  delete[] this->particle_shape_fitness;
  delete[] this->particle_velocity;
  delete[] this->particle_best_params;
  delete[] this->particle_params;
  delete[] this->best_params_avg;
  delete[] this->best_evaluation_FunctionValue;

  delete[] this->evaluation_FitnessFunction_store;
  delete[] this->evaluation_FunctionValue_store;
}

void
Swarm::Initialize()
{
  this->best_fitness = PREC_MAX;
  this->iteration = 0;
  // std::string filename = "";

  switch ( this->ctx.logLevel )
  {
    case LL_NO:
    default:
      break;

    case LL_BESTS:
    case LL_PARTICLE:
    {
      std::string filename = this->ctx.config.PSO.Log_file + "_" +
                             solver_algsName[ this->ctx.sa_index ] + "_" +
                             problem_funcsName[ this->ctx.pf_index ] + "_" +
                             this->ctx.starttime + "_" +
                             std::to_string( this->ctx.round );
      this->logindex = this->ctx.logthread->OpenFile(filename);
      FileIOHelper::copyConfig(this->ctx.config.config_filename, filename);
      break;
    }

    case LL_BEST:
    {
     std::string  filename = this->ctx.config.PSO.Log_file + "_" +
                             solver_algsName[ this->ctx.sa_index ] + "_" +
                             problem_funcsName[ this->ctx.pf_index ] + "_" +
                             this->ctx.starttime;
      this->logindex = this->ctx.logthread->OpenFile(filename);
      FileIOHelper::copyConfig(this->ctx.config.config_filename, filename);
      break;
    }
  }

  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    this->particles[i]->Initialize( SF_SIMPLE );
    this->particles[i]->SetFitnessValue(PREC_MAX);
    this->particles[i]->SetBestFitnessValue(PREC_MAX);
  }

  this->Evaluate();
  this->CalculateBestSpark();
  this->Logging( LL_PARTICLE + LL_BESTS );

  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    this->particles[i]->InitializeShapeFunction( this->ctx.config.WorkPiece.shapeFunction );
  }
}

void
Swarm::PrintTheBest()
{
  // return;

  if ( this->GetIteration() % this->ctx.config.PSO.status_print_iter == 0 )
  {
    printf("\r\033[%dCiter:%ld/%d   best:%lf   sf:%ld%*c\r", PRINT_STAT_POPULATION_POS,
            this->GetIteration(),
            this->ctx.config.PSO.max_iteration,
            this->GetBestFitness(),
            (COUNT_T)this->best_shape_fitness,
            20, ' '
           );
  }
}

bool
Swarm::IsStopCondition()
{
  return ( this->GetIteration() >= this->ctx.config.PSO.max_iteration ) ||
         ( this->GetBestFitness() < this->min_fitness ) ||
         ( simutation_stop_running );
}

void
Swarm::RunSimulation()
{
  this->iteration = 0;

  while ( not this->IsStopCondition() )
  {
    this->iteration++;

    this->Move();
    this->Evaluate();
    this->CalculateBestSpark();
    this->PrintTheBest();

    this->Logging( LL_PARTICLE + LL_BESTS );
  };

  // this->iteration = this->ctx.config.PSO.max_iteration;

  this->PrintTheBest();
  this->Logging( LL_BEST );
}

void
Swarm::CalculateBestSpark()
{
  COUNT_T old_best_index = this->best_index;

  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    this->particles[i]->SetFitnessValue(this->particle_fitness[i]);
    this->particles[i]->FitnessPostProcessing();

    if (this->particles[i]->GetFitnessValue() < this->GetBestFitness())
    {
      this->best_index = i;
      this->best_fitness = this->particles[i]->GetFitnessValue();
      this->best_shape_fitness = this->particles[i]->GetShapeFitnessValue();
    }
  }

  if (this->best_index != old_best_index)
  {
    for (COUNT_T i = 0; i < this->ctx.config.searching_dimension; i++)
    {
      this->best_params[i] = this->particles[this->best_index]->GetParamValue(i);
      this->best_velo[i] = this->particles[this->best_index]->GetVelocityValue(i);
    }

    for (COUNT_T i = 0; i < this->ctx.config.reference_function_N; i++)
    {
      this->best_evaluation_FunctionValue[i] = this->particles[this->best_index]->GetFunctionValueValue(i);
    }
  }

  if (this->ctx.sa_index == SA_QPSOT2)
  {
    memset(this->best_params_avg, 0, sizeof(PREC) * this->ctx.config.searching_dimension);

    for (COUNT_T p = 0; p < this->swarm_size; p++)
    {
      for (COUNT_T i = 0; i < this->ctx.config.searching_dimension; i++)
      {
        this->best_params_avg[i] += this->particles[p]->GetBestParamValue(i);
      }
    }

    for (COUNT_T i = 0; i < this->ctx.config.searching_dimension; i++)
    {
      this->best_params_avg[i] /= (PREC)this->swarm_size;
    }
  }
}

void
Swarm::Evaluate()
{
  processor.BatchEvaluation(this->particle_params, this->ctx.config.searching_dimension,
                            this->particle_fitness,
                            this->swarm_size,
                            this->evaluation_FunctionValue_store, this->ctx.config.reference_function_N,
                            this->evaluation_FitnessFunction_store, this->ctx.config.reference_function_N
                           );

  this->calculation += this->swarm_size;
}

void
Swarm::Move()
{
  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    this->particles[i]->CalculateNewVelocity(this->GetBestParams(), this->best_params_avg);
    this->particles[i]->MoveParticle();
  }
}

COUNT_T
Swarm::GetIteration()
{
  return this->iteration;
}

COUNT_T
Swarm::GetCalculation()
{
  return this->calculation;
}

PREC
Swarm::GetBestFitness()
{
  return this->best_fitness;
}

PREC
Swarm::GetBestShapeFitness()
{
  return this->best_shape_fitness;
}

PREC*
Swarm::GetBestParams()
{
  return this->best_params;
}

PREC*
Swarm::GetBestVelo()
{
  return this->best_velo;
}

PREC*
Swarm::GetBestFunctionValue()
{
  return this->best_evaluation_FunctionValue;
}

void
Swarm::ConvertOneArrayToTArray(TArray(PREC)* _to, PREC* _from, COUNT_T _len)
{
  COUNT_T recordIndex = _to->addRecord( _len );
  _to->copyData(_from, recordIndex, ACD_TOARRAY);
  _to->getRecordPtr(recordIndex)->setCount( _len );
}

TArray(PREC)*
Swarm::ConvertFunctionValueArrayToTArray()
{
  TArray(PREC)* functionValue = new TArray(PREC)();

  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    this->ConvertOneArrayToTArray(functionValue, this->particles[i]->GetFunctionValue(), this->ctx.config.reference_function_N);
  }

  return functionValue;
}

TArray(PREC)*
Swarm::ConvertParamsArrayToTArray()
{
  TArray(PREC)* params = new TArray(PREC)();

  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    this->ConvertOneArrayToTArray(params, this->particles[i]->GetParam(),ctx.config.searching_dimension);
  }

  return params;
}

TArray(PREC)*
Swarm::ConvertVelosArrayToTArray()
{
  TArray(PREC)* velo = new TArray(PREC)();

  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    this->ConvertOneArrayToTArray(velo, this->particles[i]->GetVelo(),ctx.config.searching_dimension);
  }

  return velo;
}

TArray(PREC)*
Swarm::ConvertBestParamsArrayToTArray()
{
  TArray(PREC)* params = new TArray(PREC)();
  this->ConvertOneArrayToTArray(params, this->GetBestParams(),ctx.config.searching_dimension);
  return params;
}

TArray(PREC)*
Swarm::ConvertBestVelosArrayToTArray()
{
  TArray(PREC)* velo = new TArray(PREC)();
  this->ConvertOneArrayToTArray(velo, this->GetBestVelo(),ctx.config.searching_dimension);
  return velo;
}

TArray(PREC)*
Swarm::ConvertBestFunctionValueArrayToTArray()
{
  TArray(PREC)* functionValue = new TArray(PREC)();
  this->ConvertOneArrayToTArray(functionValue, this->GetBestFunctionValue(), this->ctx.config.reference_function_N);
  return functionValue;
}

void
Swarm::ConvertOneArrayToTArray(TArray(PREC)* _to,
                               uint _type,
                               PREC _fitness,
                               PREC _best,
                               COUNT_T _iter,
                               COUNT_T _index,
                               PREC _shape_fitness)
{
  COUNT_T recordIndex = _to->addRecord();
  _to->addData(recordIndex, SWP_TYPE, (PREC)_type);
  _to->addData(recordIndex, SWP_FITNESS, _fitness);
  _to->addData(recordIndex, SWP_BEST, _best);
  _to->addData(recordIndex, SWP_ITER, (PREC)_iter);
  _to->addData(recordIndex, SWP_INDEX, (PREC)_index);
  _to->addData(recordIndex, SWP_SHAPE_FITNESS, _shape_fitness);
  _to->getRecordPtr(recordIndex)->setCount(SWP_MAX);
}

TArray(PREC)*
Swarm::ConvertOthersArrayToTArray()
{
  TArray(PREC)* others = new TArray(PREC)();

  for (COUNT_T i = 0; i < this->swarm_size; i++)
  {
    this->ConvertOneArrayToTArray(others,
                                  SWT_PARTICLE,
                                  this->particles[i]->GetFitnessValue(),
                                  this->particles[i]->GetBestFitnessValue(),
                                  this->GetIteration(),
                                  i,
                                  (PREC)this->particles[i]->GetShapeFitnessValue()
                                 );
  }

  return others;
}

TArray(PREC)*
Swarm::ConvertBestOthersArrayToTArray()
{
  TArray(PREC)* others = new TArray(PREC)();
  this->ConvertOneArrayToTArray(others,
                                SWT_BEST,
                                this->GetBestFitness(),
                                this->GetBestFitness(),
                                this->GetIteration(),
                                this->best_index,
                                (PREC)this->GetBestShapeFitness()
                               );
  return others;
}

void
Swarm::PrepareLogging(TArray(PREC)* _params, TArray(PREC)* _velos, TArray(PREC)* _others, TArray(PREC)* _functionValue)
{
  const COUNT_T count = _params->getCount();
  assume(
         ( count == _velos->getCount() ) &&
         ( count == _others->getCount() )
         ,
         "Others and velos does not have the same count"
        );

  LogWorker* logworker = new LogWorker(this->ctx.logthread, QT_PSO, this->logindex,
                                       SWARM_LOGWORKER_PREC_ARRAY_COUNT,
                                       SWARM_LOGWORKER_COUNT_ARRAY_COUNT,
                                       SWARM_LOGWORKER_LARRAY_COUNT,
                                       SWARM_LOGWORKER_INTS_COUNT);
  THREADPOOL(LogWorker::send, logworker,
             this->ctx.threadpools->LogWorkerThreadpool, THREADTYPE_LOG,
             "",
             count,
             _params,
             _velos,
             _others,
             _functionValue
             );

  this->IncLogItemCount( count );

}

void
Swarm::ParticleLogging()
{
  TArray(PREC)* params = ConvertParamsArrayToTArray();
  TArray(PREC)* velos = ConvertVelosArrayToTArray();
  TArray(PREC)* others = ConvertOthersArrayToTArray();
  TArray(PREC)* functionValue = ConvertFunctionValueArrayToTArray();

  this->PrepareLogging(params, velos, others, functionValue);
}

void
Swarm::BestLogging()
{
  TArray(PREC)* params = ConvertBestParamsArrayToTArray();
  TArray(PREC)* velos = ConvertBestVelosArrayToTArray();
  TArray(PREC)* others = ConvertBestOthersArrayToTArray();
  TArray(PREC)* functionValue = ConvertBestFunctionValueArrayToTArray();

  this->PrepareLogging(params, velos, others, functionValue);
}


   // config   code   res
   //   0       6      0 NO
   //   0       1      0 NO
   //   1       6      0 NO
   //   1       1      1 BEST
   //   2       6      2 BESTS
   //   2       1      0 NO
   //   4       6      4 PARTICLE
   //   4       1      0 NO


void
Swarm::Logging( uint _logLevel )
{
  switch ( this->ctx.logLevel & _logLevel )
  {
    case LL_NO:
      break;

    case LL_BEST:
    case LL_BESTS:
      this->BestLogging();
      break;

    case LL_PARTICLE:
      this->BestLogging();
      this->ParticleLogging();
      break;

    default:
      DOES_NOT_REACH();
  }
}
