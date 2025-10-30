#include "nFwa.h"
#include "../../common/Types.h"

/*
  FWA:
    firework_population:
      firework_count
      firework_max_iter
      firework:
        spark_population:
          spark_max_iter
          spark_count:
            random_spark_count
            gaussian_spark_count
*/

nFwa::nFwa(Context& _ctx, ProblemFunctionProcessor& _processor) : ctx ( _ctx ), processor( _processor )
{
  srand ((uint)time(NULL));
  setbuf(stdout, NULL);

  assume(ARRAY_SIZE(SparkPropertyTypeName) == ARRAY_SIZE(SparkPropertyDataTypeType), "Array sizes are not equal");
  assume(ARRAY_SIZE(SparkPropertyTypeName) == SPT_MAX, "Array size and enum max element are not fits; MAX='%d'", SPT_MAX);

  assume(ARRAY_SIZE(SwarmPropertyTypeName) == ARRAY_SIZE(SwarmPropertyDataTypeType), "Array sizes are not equal");
  assume(ARRAY_SIZE(SwarmPropertyTypeName) == SWP_MAX, "Array size and enum max element are not fits; MAX='%d'", SWP_MAX);

  assume(ARRAY_SIZE(ElementTypeName) == SWT_MAX, "Array size and enum max element are not fits; MAX='%d'", SWT_MAX);

  assume(ARRAY_SIZE(SFWAPropertyTypeName) == ARRAY_SIZE(SFWAPropertyDataTypeType), "Array sizes are not equal");
  assume(ARRAY_SIZE(SFWAPropertyTypeName) == SSP_MAX, "Array size and enum max element are not fits; MAX='%d'", SSP_MAX);

  assume(ARRAY_SIZE(SparkStateTypeName) == ARRAY_SIZE(SparkStateDataTypeType), "Array sizes are not equal");
  assume(ARRAY_SIZE(SparkStateTypeName) == SSET_MAX, "Array size and enum max element are not fits; MAX='%d'", SSET_MAX);

  assume(ARRAY_SIZE(SparkTypeName) == ST_MAX, "Array size and enum max element are not fits; MAX='%d'", ST_MAX);

  assume(ARRAY_SIZE(SparkStatisticTypeName) == ARRAY_SIZE(SparkStatisticDataTypeType), "Array sizes are not equal");
  assume(ARRAY_SIZE(SparkStatisticTypeName) == SSCT_MAX, "Array size and enum max element are not fits; MAX='%d'", SSCT_MAX);

  assume(ARRAY_SIZE(SparkCustomPropertyTypeSize) == ST_MAX, "Array size and enum max element are not fits; MAX='%d'", ST_MAX);
  assume(ARRAY_SIZE(SparkCustomPropertyStartType) == ST_MAX, "Array size and enum max element are not fits; MAX='%d'", ST_MAX);
  assume(ARRAY_SIZE(SparkCustomPropertyTypeName) == SCPT_MAX, "Array size and enum max element are not fits; MAX='%d'", SCPT_MAX);
  assume(ARRAY_SIZE(SparkCustomPropertyTypeName) == ARRAY_SIZE(SparkCustomPropertyDataTypeType), "Array sizes are not equal");

  assume(ARRAY_SIZE(SparkCustomPropertyDataTypeType) == ARRAY_SIZE(SparkCustomPropertyTypeName), "Array sizes are not equal");
  assume(ARRAY_SIZE(SparkCustomPropertyDataTypeType) == SCPT_MAX, "Array size and enum max element are not fits; MAX='%d'", SCPT_MAX);

  assume(ARRAY_SIZE(SparkCustomLocationTypeSize) == ST_MAX, "Array size and enum max element are not fits; MAX='%d'", ST_MAX);
  assume(ARRAY_SIZE(SparkCustomLocationStartType) == ST_MAX, "Array size and enum max element are not fits; MAX='%d'", ST_MAX);
  assume(ARRAY_SIZE(SparkCustomLocationTypeName) == SCLT_MAX, "Array size and enum max element are not fits; MAX='%d'", SCLT_MAX);

  assume(ARRAY_SIZE(SparkCustomLocationAmaxID) == ST_MAX, "Array size and enum max element are not fits; MAX='%d'", ST_MAX);
  for ( uint index = 0; index < ST_MAX; index ++)
  {
    assume( SparkCustomLocationTypeName[ SparkCustomLocationAmaxID[ index ] ] == "AMAX", "CustomLocationMaxID is not AMAX; index='%d'", index);
  }

  assume(ARRAY_SIZE(SparkDirectionName) == SD_MAX, "Array size and enum max element are not fits; MAX='%d'", SD_MAX);
  assume(ARRAY_SIZE(SparkCalculationStateName) == SCS_MAX, "Array size and enum max element are not fits; MAX='%d'", SCS_MAX);

  assume(ARRAY_SIZE(SparkStateName) == SS_MAX, "Array size and enum max element are not fits; MAX='%d'", SS_MAX);
  assume(ARRAY_SIZE(SparkSubStateName) == SSS_MAX, "Array size and enum max element are not fits; MAX='%d'", SSS_MAX);
  assume(ARRAY_SIZE(SparkTeleMetricSignName) == STMS_MAX, "Array size and enum max element are not fits; MAX='%d'", STMS_MAX);
  assume(ARRAY_SIZE(SparkTelemetricDirectionName) == STMD_MAX, "Array size and enum max element are not fits; MAX='%d'", STMD_MAX);
  assume(ARRAY_SIZE(SparkDominantSpeedDirectionName) == SDOM_MAX, "Array size and enum max element are not fits; MAX='%d'", SDOM_MAX);
  assume(ARRAY_SIZE(SparkTerrainName) == SET_MAX, "Array size and enum max element are not fits; MAX='%d'", SET_MAX);
  assume(ARRAY_SIZE(SparkTerrainDirectionName) == SCT_MAX, "Array size and enum max element are not fits; MAX='%d'", SCT_MAX);

  assume((SCSS_MAX - 1) * (SS_BORN - 1) + SCSS_MAIN == SCS_BORN, "State machine state id calculation error; state='%d'", SCS_BORN);
  assume((SCSS_MAX - 1) * (SS_FLOW - 1) + SCSS_MAIN == SCS_FLOW, "State machine state id calculation error; state='%d'", SCS_FLOW);
  assume((SCSS_MAX - 1) * (SS_DIE  - 1) + SCSS_MAIN == SCS_DIE, "State machine state id calculation error; state='%d'", SCS_DIE);

  assume((SCSS_MAX - 1) * (SS_BORN - 1) + SCSS_BEST_EVALUATE == SCS_INIT_BEST_EVALUATE, "State machine state id calculation error; state='%d'", SCS_INIT_BEST_EVALUATE);
  assume((SCSS_MAX - 1) * (SS_FLOW - 1) + SCSS_BEST_EVALUATE == SCS_BEST_EVALUATE, "State machine state id calculation error; state='%d'", SCS_BEST_EVALUATE);
  assume((SCSS_MAX - 1) * (SS_DIE  - 1) + SCSS_BEST_EVALUATE == SCS_DIE_BEST_EVALUATE, "State machine state id calculation error; state='%d'", SCS_DIE_BEST_EVALUATE);

  assume((SCSS_MAX - 1) * (SS_BORN - 1) + SCSS_POST_CALCULATION == SCS_INIT_POST_CALCULATION, "State machine state id calculation error; state='%d'", SCS_INIT_POST_CALCULATION);
  assume((SCSS_MAX - 1) * (SS_FLOW - 1) + SCSS_POST_CALCULATION == SCS_POST_CALCULATION, "State machine state id calculation error; state='%d'", SCS_POST_CALCULATION);
  assume((SCSS_MAX - 1) * (SS_DIE  - 1) + SCSS_POST_CALCULATION == SCS_DIE_POST_CALCULATION, "State machine state id calculation error; state='%d'", SCS_DIE_POST_CALCULATION);

  assume((SCSS_MAX - 1) * (SS_BORN - 1) + SCSS_NEXT_STATE == SCS_INIT_NEXT_STATE, "State machine state id calculation error; state='%d'", SCS_INIT_NEXT_STATE);
  assume((SCSS_MAX - 1) * (SS_FLOW - 1) + SCSS_NEXT_STATE == SCS_NEXT_STATE, "State machine state id calculation error; state='%d'", SCS_NEXT_STATE);
  assume((SCSS_MAX - 1) * (SS_DIE  - 1) + SCSS_NEXT_STATE == SCS_DIE_NEXT_STATE, "State machine state id calculation error; state='%d'", SCS_DIE_NEXT_STATE);
}

nFwa::~nFwa()
{
  delete this->firework_population;
}

void
nFwa::Initialize()
{
  COUNT_T fw_pop_index = 0;

  this->boundaries.Hmin = this->ctx.config.min_param;
  this->boundaries.Hmax = this->ctx.config.max_param;
  this->boundaries.Dim =  this->ctx.config.searching_dimension;
  this->boundaries.Dmin = this->ctx.config.IHCP.min_dimension;
  this->boundaries.Dmax = this->ctx.config.IHCP.max_dimension;
  this->boundaries.fw_count = this->ctx.config.nFWA.FireworkCount;
  this->boundaries.fw_max_iter = this->ctx.config.nFWA.FireworkMaxIteration;
  this->boundaries.spark_max_iter = this->ctx.config.nFWA.SparkMaxIteration;
  this->boundaries.qcurve_point_size = this->ctx.config.reference_function_N;
  this->boundaries.min_fitness = this->ctx.config.nFWA.min_fitness;

  this->boundaries.random_spark_count = this->ctx.config.nFWA.RandomSparkCount;
  this->boundaries.gaussian_spark_count = this->ctx.config.nFWA.GaussianSparkCount;
  this->boundaries.quantum_spark_count = this->ctx.config.nFWA.QuantumSparkCount;
  this->boundaries.shapeFunction = this->ctx.config.WorkPiece.shapeFunction;

  this->firework_population = new FireworkPopulation(this->ctx, this->processor);
  this->firework_population->Initialize(fw_pop_index, this->boundaries);
}

void
nFwa::RunSimulation()
{
  this->firework_population->RunSimulation();
}

PREC
nFwa::GetBestFitness()
{
  return this->firework_population->GetBestFitness();
}

PREC
nFwa::GetBestShapeFitness()
{
  return this->firework_population->GetBestShapeFitness();
}

TRecord(PREC)*
nFwa::GetBestParam()
{
  return this->firework_population->GetBestParam();
}

COUNT_T
nFwa::GetIteration()
{
  return this->firework_population->GetIteration();
}

COUNT_T
nFwa::GetCalculation()
{
  return this->firework_population->GetAllCalculations();
}
