#pragma once

#include <string>
#include "../../common/Types.h"
#include "../../common/Record.h"


void SetSparkRecordDataPtrs(SparkRecordData* _dstdata, SparkRecordData* _srcdata);
void SetSparkRecordDataToEmpty(SparkRecordData* _dstdata);
void FreeSparkRecordDataPtrs(SparkRecordData* _srcdata);
