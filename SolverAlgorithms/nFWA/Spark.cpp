#include <cmath>
#include "Spark.h"

#include "FireworkSpark.h"
#include "RandomSpark.h"
#include "GaussianSpark.h"
#include "BestSpark.h"

#include "Stats.h"
#include "SparkHistory.h"
#include "../../common/Types.h"
#include "Firework.h"
#include "Fireworks.h"

#include <iostream>
#include <algorithm>

void
Spark::SetSparkPtrs(SparkRecordData* _data,
                    Stats* _stats,
                    SparkHistory* _history
                    )
{
  this->SetDataPtr(_data);
  this->SetStats(_stats);
  this->SetHistory(_history);
  this->SetStatsSpark(this);
}

void
Spark::SetSparkIndexes(COUNT_T _fw_index,
                       COUNT_T _fw_pop_index,
                       COUNT_T _recordIndex,
                       COUNT_T _sparkIndex
                       )
{
  this->SetRecordIndex(_recordIndex);
  this->SetFwIndex(_fw_index);
  this->SetFwPopIndex(_fw_pop_index);
  this->SetIndex(_sparkIndex);
}

void
Spark::PreInitialize()
{
  this->SetFitness(PREC_MAX);
  this->SetShapeFitness(PREC_MAX);
  this->SetITL(PREC_MAX);
}

void
Spark::InitializeShapeFunction(COUNT_T _ShapeFunction)
{
  switch ( _ShapeFunction )
  {
    case SF_HTC:
    {
      this->problemShape =  static_cast<ProblemShape*>( new HTCShape() );
      break;
    }
    case SF_SIMPLE:
    {
      this->problemShape =  static_cast<ProblemShape*>( new SimpleShape() );
      break;
    }

    default:
    {
      DOES_NOT_REACH();
    }
  }

  const PREC min = this->GetHmin();
  const PREC max = this->GetHmax();
  this->GetProblemShape()->Initialize( _ShapeFunction, min, max );
}

void
Spark::Initialize(Boundaries _boundaries)
{
  this->SetHmin(_boundaries.Hmin);
  this->SetHmax(_boundaries.Hmax);
  this->SetDmin(_boundaries.Dmin);
  this->SetDmax(_boundaries.Dmax);
  this->SetDim(_boundaries.Dim);
  this->SetFwCount(_boundaries.fw_count);

  this->SetType(ST_UNKNOWN);

  this->SetFmax(0);
  this->SetFmin(PREC_MAX);
  this->SetQCurvePointSize(_boundaries.qcurve_point_size);
  this->SetFitness(PREC_MAX);
  this->SetShapeFitness(PREC_MAX);

  this->SetIter(1);
  this->SetITL(10);
  for (uint i = SSIV_VELO_FIRST; i < SSIV_MAX; ++i)
  {
    this->SetVELO(i, 0);
  }
  for (uint i = SSIA_ACCEL_FIRST; i < SSIA_MAX; ++i)
  {
    this->SetACCEL(i, 0);
  }
  this->InitializeShapeFunction( SF_SIMPLE );

  this->SetInitialized();
}

void
Spark::DeInitialize()
{
}

TFirework*
Spark::GetFireworks()
{
  return this->fireworks->GetFireworks();
}

Firework*
Spark::GetFirework(COUNT_T _fw_index)
{
  return this->fireworks->GetFirework(_fw_index);
}

Spark*
Spark::GetSparkBySparkID(COUNT_T _ID)
{
  return this->GetFirework()->GetSparkPopulation()->GetSparkStore()->GetSparkBySparkID(_ID);
}

PREC
Spark::GetUniformRandom01()
{
  return this->GetFirework()->GetSparkPopulation()->GetUniformRandom01();
}

int
Spark::GetRandomDirection()
{
  return this->GetFirework()->GetSparkPopulation()->GetRandomDirection();
}

PREC
Spark::GetUniformRandom(PREC _min, PREC _max)
{
  return this->GetFirework()->GetSparkPopulation()->GetUniformRandom(_min, _max);
}

PREC
Spark::GetNormalRandom_1T0B()
{
  return this->GetFirework()->GetSparkPopulation()->GetNormalRandom_1T0B();
}

PREC
Spark::GetNormalRandom_0T1B()
{
  return this->GetFirework()->GetSparkPopulation()->GetNormalRandom_0T1B();
}

PREC
Spark::GetNormalRandom_0B1T2B()
{
  return this->GetFirework()->GetSparkPopulation()->GetNormalRandom_0B1T2B();
}

TRecord(PREC)*
Spark::ChangeParamsRecord(TRecord(PREC)* restrict _new_record)
{
    TRecord(PREC)* old_record = this->GetParamPtr();
    this->SetParamPtr( _new_record );
    this->GetFirework()->GetSparkPopulation()->GetSparkStore()->GetParams()->setRecordPtr(_new_record, this->GetRecordIndex() );

    return old_record;
}

TRecord(PREC)*
Spark::GetFwParamPtr(COUNT_T _fw_index)
{
  if (this->GetFirework(_fw_index))
  {
    return this->GetFirework(_fw_index)->GetFireworkSpark()->GetParamPtr();
  }
  return NULL;
}

void
Spark::GenerateParamByBase(OffsetCalculationType _offsetCalculationType)
{
  this->GetProblemShape()->SetCalculation(
                                           _offsetCalculationType,
                                           this->GetMovingBase(),
                                           this->GetDirection(),
                                           this->GetParamPtr(),
                                           this->GetFitness(),
                                           this->GetParamCount()
                                          );

  this->GetProblemShape()->GenerateParamByBase();
}

void
Spark::GenerateRandomParamByBase()
{
  this->CheckMovingProfile();

  this->SetDirection(this->GetAmaxFunctionPtr());
  this->GenerateParamByBase(OCT_AMAX_RANDOM);

  this->SetMovingProfileOff();
}

void
Spark::GenerateDirectedParamByBase()
{
  this->CheckMovingProfile();
  this->GenerateParamByBase(OCT_DIRECTED_RANDOM);
  this->SetMovingProfileOff();
}

void
Spark::GenerateRandomParamInArea()
{
  this->CheckMovingProfile();

  for (COUNT_T index = 0; index < this->GetParamCount(); ++index)
  {
    PREC value = this->GetUniformRandom(this->GetHmin(), this->GetHmax());
    this->SetParam(index, value);
  }

  this->SetMovingProfileOff();
}

PREC
Spark::CalculateShapeFitness()
{
  this->GetProblemShape()->SetCalculation( OCT_SHAPE, NULL, NULL, this->GetParamPtr(), this->GetParamCount() );
  return this->GetProblemShape()->CalculateShapeFitness();
}

void
Spark::CalculateDirection(TRecord(PREC)* _startpoint, TRecord(PREC)* _endpoint)
{
  assume( _startpoint->getCount() == _endpoint->getCount(), "Startpoint and endpoint vector counts are not the same" );
  assume( this->GetDirection(), "Direciton record is not set" );

  this->GetDirection()->clearCount();
  for (COUNT_T index = 0; index < _startpoint->getCount(); ++index)
  {
    PREC distance = _endpoint->getData(index) - _startpoint->getData(index);
    this->GetDirection()->addData( distance );
  }
}

void
Spark::CalculateRealAmaxFunction(COUNT_T _HistoryIndex)
{
  for ( COUNT_T index = 0; index < this->GetRealAmaxFunctionCount(); ++index)
  {
    const PREC offset = std::abs(this->GetHistory()->GetParamPtr( _HistoryIndex )->getData(index) - this->GetParamPtr()->getData(index));
    this->SetRealAmaxFunctionValue( index, offset );
  }
}

Spark*
Spark::SelectOneSparkFromAll()
{
  const COUNT_T max_spark_id = this->GetFirework()->GetSparkPopulation()->GetSparkStore()->GetSparks()->getCount() - 1;
  assume( (representable_as_type<COUNT_T, PREC>(max_spark_id)), "PREC is not representable as COUNT_T" );
  COUNT_T selected_spark_id = this->GetID();
  while (
          ( selected_spark_id == this->GetID() )
        )
  {
    selected_spark_id = (COUNT_T)round( this->GetUniformRandom(0, (PREC)max_spark_id ) );
  }
  assume( selected_spark_id != this->GetID(), "Selected spark id is the Spark ID" );
  return this->GetFirework()->GetSparkPopulation()->GetSpark(selected_spark_id);
}

void
Spark::DefaultRandomMovingProfileAroundFw()
{
  this->SetMovingProfileOn();

  this->SetMovingBase( this->GetFwParamPtr() );
  this->GenerateRandomParamByBase();
}

void
Spark::InitRandomMovingProfileInArea()
{
  this->SetMovingProfileOn();
  this->GenerateRandomParamInArea();
}

void
Spark::SaveAllDataToHistory()
{
  SparkRecordData presentdata;

  presentdata.param = this->GetParamPtr()->copy();
  presentdata.calculatedFunctionValue = this->GetCalculatedFunctionValuePtr()->copy();
  presentdata.fitnessFunction = this->GetFitnessFunctionPtr()->copy();
  presentdata.property = this->GetPropertyPtr()->copy();
  presentdata.state = this->GetStatePtr()->copy();
  presentdata.statistic = this->GetStatisticPtr()->copy();
  presentdata.custom = this->GetCustomPtr()->copy();
  presentdata.realAmaxFunction = this->GetRealAmaxFunctionPtr()->copy();
  presentdata.customLocation = this->GetCustomLocationPtr()->copyArray();
  presentdata.spark = this->GetSparkPtr()->copy();

  this->GetHistory()->SetNewHistoryData(&presentdata);
}

void
Spark::CalculateFmaxFmin()
{
  if (this->GetFmin() > this->GetFitness())
  {
    this->SetFmin(this->GetFitness());
  }

  if (this->GetFmax() < this->GetFitness())
  {
    this->SetFmax(this->GetFitness());
  }
}

void
Spark::SetFitness(PREC _fitness)
{
    this->GetPropertyPtr()->setData(SPT_FITNESS, _fitness );

    const COUNT_T point_size = this->GetQCurvePointSize();
    assume( (representable_as_type<COUNT_T, PREC>(point_size)), "PREC is not representable as COUNT_T" );
    this->SetNormFitness( _fitness / (PREC)point_size );
}

void
Spark::SetITL(PREC _ITL)
{
  this->GetPropertyPtr()->setData(SPT_ITL, _ITL);
}

void
Spark::CalculateITL()
{
  PREC ITLDirection = 0;
  switch (this->GetTerrainDirection())
  {
    case SCT_INC:
    {
      ITLDirection = -1;
      break;
    }

    case SCT_NOT_CALCULATED:
    {
      ITLDirection = 0;
      break;
    }

    case SCT_EQ:
    {
      ITLDirection = -1;
      break;
    }

    case SCT_DEC:
    {
      ITLDirection = 1;
      break;
    }

    default:
    {
      DOES_NOT_REACH();
    }
  }

  this->RelITL(ITLDirection);
}



bool
Spark::IsBest()
{
  return this->GetFirework()->GetBestSpark()->GetPartnerID() == this->GetID();
}

bool
Spark::IsPlato()
{
  return this->GetFirework()->GetBestSpark()->GetEQTC() > 0;
}


void
Spark::CalculateEQTC()
{
  switch (this->GetTerrainDirection())
  {
    case SCT_NOT_CALCULATED:
    case SCT_INC:
    case SCT_DEC:
    {
      this->SetEQTC(0);
      break;
    }

    case SCT_EQ:
    {
      this->SetINCEQTC();
      break;
    }

    default:
    {
      DOES_NOT_REACH();
    }
  }
}

void
Spark::CalculateTelemetrics()
{
  this->GetStats()->CalculateTelemetrics();
  const COUNT_T lastTerrain = this->GetStats()->GetLastTerrain();
  const COUNT_T lastDominantSpeed = this->GetStats()->GetLastDominantSpeed();
  this->SetTelemetricDirection( (SparkTelemetricDirection)(lastTerrain) );
  this->SetTerrainDirection( (SparkTerrainDirection)(this->GetStats()->EvaluateTerrain(lastTerrain, lastDominantSpeed)) );
  this->SetTerrain( (SparkTerrain)(this->GetStats()->EvaluateTerrainSET(lastTerrain, lastDominantSpeed)) );
  this->SetDominantSpeedDirection( (SparkDominantSpeedDirection)(lastDominantSpeed) );
}

void
Spark::PostCalculation()
{
  this->CalculateFmaxFmin();
  this->CalculateTelemetrics();
  this->CalculateITL();
  this->CalculateRealAmaxFunction(SH_FIRST);
  this->CalculateEQTC();

  this->CallSpecialPostCalculation();

  this->SaveAllDataToHistory();
}

void
Spark::InitPostCalculation()
{
  this->CalculateFmaxFmin();
  this->CalculateTelemetrics();

  this->CallSpecialInitPostCalculation();

  this->SaveAllDataToHistory();
}

void
Spark::BornChildSpark() {}
