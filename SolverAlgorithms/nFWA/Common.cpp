#include <string>
#include "../../common/Types.h"
#include "../../common/Record.h"
#include "../../common/Array.h"
#include "Common.h"

void
SetSparkRecordDataToEmpty(SparkRecordData* _srcdata)
{
  _srcdata->param = NULL;
  _srcdata->calculatedFunctionValue = NULL;
  _srcdata->fitnessFunction = NULL;
  _srcdata->property = NULL;
  _srcdata->statistic = NULL;
  _srcdata->state = NULL;
  _srcdata->custom = NULL;
  _srcdata->realAmaxFunction = NULL;
  _srcdata->customLocation = NULL;
  _srcdata->spark = NULL;
}

void
SetSparkRecordDataPtrs(SparkRecordData* _dstdata, SparkRecordData* _srcdata)
{
  _dstdata->param = _srcdata->param;
  _dstdata->calculatedFunctionValue = _srcdata->calculatedFunctionValue;
  _dstdata->fitnessFunction = _srcdata->fitnessFunction;
  _dstdata->property = _srcdata->property;
  _dstdata->statistic = _srcdata->statistic;
  _dstdata->state = _srcdata->state;
  _dstdata->custom = _srcdata->custom;
  _dstdata->realAmaxFunction = _srcdata->realAmaxFunction;
  _dstdata->customLocation = _srcdata->customLocation;
  _dstdata->spark = _srcdata->spark;
}

void
FreeSparkRecordDataPtrs(SparkRecordData* _srcdata)
{
  _srcdata->param->free();
  _srcdata->calculatedFunctionValue->free();
  _srcdata->fitnessFunction->free();
  _srcdata->property->free();
  _srcdata->statistic->free();
  _srcdata->state->free();
  _srcdata->custom->free();
  _srcdata->realAmaxFunction->free();
  _srcdata->customLocation->free();
  _srcdata->spark->free();
}
