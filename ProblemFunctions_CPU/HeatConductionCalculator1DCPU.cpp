#include "HeatConductionCalculator1DCPU.h"
#include "HeatConductionCalculator1DCPUWorker.h"


inline
PROC_PREC
CalculateK( PROC_PREC T )
{
  const PROC_PREC KPLUS = (PROC_PREC)13.7129;
  const PROC_PREC KMULT = (PROC_PREC)0.017;

  return KPLUS + KMULT * T;
}

inline
PROC_PREC
CalculateCp( PROC_PREC T )
{
  const PROC_PREC CpPLUS = (PROC_PREC)333.73;
  const PROC_PREC CpMULT = (PROC_PREC)0.2762;

  return CpPLUS + CpMULT * T;
}

inline PROC_PREC
CalculateRho( PROC_PREC T )
{
  const PROC_PREC RhoPLUS = (PROC_PREC)7925.4;
  const PROC_PREC RhoMULT = (PROC_PREC)0.4434;

  return RhoPLUS - RhoMULT * T;
}

inline PROC_PREC
CalculateAlpha( PROC_PREC temperature )
{
  const PROC_PREC k = CalculateK(temperature);
  const PROC_PREC cp = CalculateCp(temperature);
  const PROC_PREC rho = CalculateRho(temperature);

  return k / (cp * rho);
}

void
HeatConductionCalculator1DCPU::FillHeatCalcParams(HeatCalcParams* _heatCalcParams)
{
  _heatCalcParams->HTCTimeFunction = this->config.IHCP.HTCTimeFunction;
  _heatCalcParams->referenceFunctionTimes = this->config.CoolingCurve.reference_function_times;
  _heatCalcParams->referenceFunctionValues = this->config.CoolingCurve.reference_function_values;

  _heatCalcParams->endTime = this->config.CoolingCurve.reference_function_end_time;
  _heatCalcParams->temperatureLength = this->config.WorkPiece.N;
  _heatCalcParams->initialTemperature = this->config.CoolingCurve.initial_temperature;
  _heatCalcParams->finalTemperature = this->config.CoolingCurve.final_temperature;

  _heatCalcParams->N = this->config.WorkPiece.N;
  _heatCalcParams->R = this->config.WorkPiece.R;
  _heatCalcParams->htcN = this->config.searching_dimension;
  _heatCalcParams->TCIndex = this->config.WorkPiece.TC - 1;
  _heatCalcParams->referenceFunctionN = this->config.CoolingCurve.reference_function_N;
  _heatCalcParams->lastTemperatureIndex = this->config.WorkPiece.N - 1;

  _heatCalcParams->dx = _heatCalcParams->R / (PROC_PREC)_heatCalcParams->N;
  _heatCalcParams->dx2 = _heatCalcParams->dx * _heatCalcParams->dx;
  _heatCalcParams->dx4 = _heatCalcParams->dx2 * _heatCalcParams->dx2;
  _heatCalcParams->stability_dt = _heatCalcParams->dx2 / ( 2.0f * CalculateAlpha(_heatCalcParams->initialTemperature) );
  _heatCalcParams->min_dt = _heatCalcParams->endTime / (PROC_PREC)_heatCalcParams->referenceFunctionN;
  _heatCalcParams->delta_t = MIN(_heatCalcParams->stability_dt, _heatCalcParams->min_dt);
  _heatCalcParams->dtpdx2 = _heatCalcParams->delta_t / _heatCalcParams->dx2;
  _heatCalcParams->dtdx2m2 = 2.0f * _heatCalcParams->delta_t * _heatCalcParams->dx2;
  _heatCalcParams->decTmod = ( 2.0f + 1.0f / ( (PROC_PREC)_heatCalcParams->lastTemperatureIndex * _heatCalcParams->dx4 ) ) * _heatCalcParams->dx;

  assume( (representable_as_type<COUNT_T, PROC_PREC>( _heatCalcParams->lastTemperatureIndex ) ), "PREC is not representable as COUNT_T" );
  assume( (representable_as_type<COUNT_T, PROC_PREC>( _heatCalcParams->referenceFunctionN ) ), "PREC is not representable as COUNT_T" );
  assume( (representable_as_type<COUNT_T, PROC_PREC>( _heatCalcParams->N ) ), "PREC is not representable as COUNT_T" );

  _heatCalcParams->PREC_N = (PROC_PREC)_heatCalcParams->N;
  _heatCalcParams->PREC_referenceFunctionN = (PROC_PREC)_heatCalcParams->referenceFunctionN;
  _heatCalcParams->PREC_lastTemperatureIndex = (PROC_PREC)_heatCalcParams->lastTemperatureIndex;

  _heatCalcParams->c1pc1p2i[ 0 ] = 1;
  _heatCalcParams->c1mc1p2i[ 0 ] = 1;
  for (PROC_COUNT_T i = 1; i < _heatCalcParams->N; ++i) // divide by zero at i=0
  {
    const PROC_PREC c1p2i = 1.0f / ( (PROC_PREC)(i << 1) );
    _heatCalcParams->c1pc1p2i[ i ] = 1 + c1p2i;
    _heatCalcParams->c1mc1p2i[ i ] = 1 - c1p2i;
  }

  _heatCalcParams->start_iteration_index = COUNT_T_NEG;
  _heatCalcParams->next_iteration_index = COUNT_T_NEG;
}

void
HeatConductionCalculator1DCPU::CalculateTargetFunction(PREC* _htc, PREC* _heatFunctionValue)
{
  CPUCalculateTargetFunction(this->heatCalcParams, _htc, _heatFunctionValue);
}

void
HeatConductionCalculator1DCPU::CalculateFitness(PREC* _heatFunctionValue, PREC* _fitness, PREC* _fitnessFunction)
{
  CPUCalculateFitness(this->heatCalcParams, _heatFunctionValue, _fitness, _fitnessFunction);
}

void
HeatConductionCalculator1DCPU::CalculateFitnessManyAsyncWorker(PREC* _htcs, COUNT_T _N, PREC* _fitnesses, PREC* _heatFunctions, PREC* _fitnessFunctions)
{
  CPUHeatConductionWorker(this->heatCalcParams, _N, _fitnesses, _htcs, _heatFunctions, _fitnessFunctions);
}

void
HeatConductionCalculator1DCPU::CalculateFitnessMany(PREC* _htcs, COUNT_T _N, PREC* _fitnesses, PREC* _heatFunctions, PREC* _fitnessFunctions)
{
  this->CalculateFitnessManyAsyncWorker(_htcs, _N, _fitnesses, _heatFunctions, _fitnessFunctions);
}

void
HeatConductionCalculator1DCPU::CalculateFitnessManyAsync(PREC* _htcs, COUNT_T _N, PREC* _fitnesses, PREC* _heatFunctions, PREC* _fitnessFunctions)
{
  THREADPOOL(HeatConductionCalculator1DCPU::CalculateFitnessManyAsyncWorker, this,
            this->threadpool, THREADTYPE_PF,
            _htcs,
            _N,
            _fitnesses,
            _heatFunctions,
            _fitnessFunctions);
}

void
HeatConductionCalculator1DCPU::DeInitialize()
{
  _free(this->heatCalcParams->temperatures);
  _free(this->heatCalcParams->nextTemperatures);

  _free(this->heatCalcParams->c1pc1p2i);
  _free(this->heatCalcParams->c1mc1p2i);

  _free(this->heatCalcParams->start_iteration_temperatures);
  _free(this->heatCalcParams->next_iteration_temperatures);

  _free(this->heatCalcParams->interpolated_htc);

  _free(this->heatCalcParams);
}

void
HeatConductionCalculator1DCPU::Initialize()
{
  const COUNT_T temperatureSize = this->config.WorkPiece.N * sizeof(PREC);
  this->heatCalcParams = static_cast<HeatCalcParams*>( _malloc( sizeof(HeatCalcParams) ) );

  this->heatCalcParams->temperatures = static_cast<PREC*>( _malloc( temperatureSize ) );
  this->heatCalcParams->nextTemperatures = static_cast<PREC*>( _malloc( temperatureSize ) );

  this->heatCalcParams->c1pc1p2i = static_cast<PREC*>( _malloc( temperatureSize ) );
  this->heatCalcParams->c1mc1p2i = static_cast<PREC*>( _malloc( temperatureSize ) );

  this->heatCalcParams->start_iteration_temperatures = static_cast<PREC*>( _malloc( temperatureSize ) );
  this->heatCalcParams->next_iteration_temperatures = static_cast<PREC*>( _malloc( temperatureSize ) );

  this->heatCalcParams->interpolated_htc = static_cast<PREC*>( _malloc( this->config.CoolingCurve.reference_function_N * sizeof(PREC) ) );

  this->FillHeatCalcParams(this->heatCalcParams);
}

HeatConductionCalculator1DCPU::HeatConductionCalculator1DCPU(ThreadPool* _threadpool, Configuration& _config, uint _cpu_id, uint _pfindex) : ProblemFunctionCalculator(_config, _pfindex)
{
  this->cpu_id = _cpu_id;
  this->threadpool = _threadpool;
  this->Initialize();
}

HeatConductionCalculator1DCPU::~HeatConductionCalculator1DCPU()
{
  this->DeInitialize();
}

HeatCalcParams*
HeatConductionCalculator1DCPU::GetHeatCalcParams()
{
  return this->heatCalcParams;
}
