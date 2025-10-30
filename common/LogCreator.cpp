#include <fstream>
#include <string>
#include "LogCreator.h"

#define LINE_BUF_START_LEN 1024
#define MIN_FREE_BUF_SIZE 256

LogCreator::LogCreator()
{
  this->lineBuf = static_cast<char*>(  malloc(LINE_BUF_START_LEN) );
  this->maxBufLen = LINE_BUF_START_LEN;
  this->ZeroPosBuf();
}

LogCreator::~LogCreator()
{
  free(this->lineBuf);
}

template<typename... Args> void
LogCreator::addToLinebuf(Args... args)
{
  if ( this->maxBufLen - this->posBuf <= MIN_FREE_BUF_SIZE )
  {
    this->maxBufLen *= 2;
    this->lineBuf = static_cast<char*>( realloc( this->lineBuf, this->maxBufLen ) );
  }

  const int chars_count = snprintf(this->lineBuf + this->posBuf, this->maxBufLen - this->posBuf, args...);
  assume(chars_count > 0, "Error in snprintf; error='%d'", chars_count);
  this->posBuf += (uint)chars_count;
}

template <typename ItemType> void
LogCreator::CollectLocation(TVArray(ItemType) _data, COUNT_T _item_index)
{
  ItemType* data = _data.at(_item_index)->getRecordDataPtr();
  const COUNT_T size = _data.at(_item_index)->getCount();
  for (COUNT_T PREC_index = 0; PREC_index < size - 1; PREC_index++)
  {
    this->addToLinebuf(LOCATION_PRINT_FORMAT " ", data[PREC_index]);
  }
  this->addToLinebuf(LOCATION_PRINT_FORMAT ": ", data[size - 1]);
}

template <typename ItemType> void
LogCreator::CheckIndexData(ItemType _data,
                           uint _max,
                           std::string _errormsg)
{
  const uint data = (uint)_data;
  assume( ( data == _data ) && ( data < _max), "%s out of bounds: %d", _errormsg.c_str(), data );
}

template <typename ItemType> void
LogCreator::CollectData(ItemType _data,
                       const std::string& _info,
                       const uint _type
                       )
{
  this->IncAndGetLineItemCounter();
  switch (_type)
  {
    case SDT_PREC:
    {
      this->addToLinebuf("%s:%d:" PREC_FORMAT ": ", _info.c_str(), this->GetLineItemCounter(), (PREC)_data);
      break;
    }

    case SDT_DOUBLE:
    {
      this->addToLinebuf("%s:%d:" DOUBLE_FORMAT ": ", _info.c_str(), this->GetLineItemCounter(), (double)_data);
      break;
    }

    case SDT_PERCENT:
    {
      this->addToLinebuf("%s:%d:" PREC_PERCENT_FORMAT ": ", _info.c_str(), this->GetLineItemCounter(), (PREC)_data * 100);
      break;
    }

    case SDT_INT:
    {
      this->addToLinebuf("%s:%d:%lli: ", _info.c_str(), this->GetLineItemCounter(), (COUNT_T)_data);
      break;
    }

    case SDT_TYPENAME:
    {
      this->CheckIndexData( _data, ST_MAX, "SparkTypeName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), SparkTypeName[ (uint)_data ].c_str());
      break;
    }

    case SDT_ELEMENTTYPENAME:
    {
      this->CheckIndexData( _data, SWT_MAX, "ElementTypeName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), ElementTypeName[ (uint)_data ].c_str());
      break;
    }

    case SDT_STATENAME:
    {
      this->CheckIndexData( _data, SS_MAX, "SparkStateName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), SparkStateName[ (uint)_data ].c_str());
      break;
    }

    case SDT_SUBSTATENAME:
    {
      this->CheckIndexData( _data, SSS_MAX, "SparkSubStateName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), SparkSubStateName[ (uint)_data ].c_str());
      break;
    }

    case SDT_DIRECTION:
    {
      this->CheckIndexData( _data, SD_MAX, "SparkDirectionName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), SparkDirectionName[ (uint)_data ].c_str());
      break;
    }

    case SDT_TERRAIN_DIR:
    {
      this->CheckIndexData( _data, SCT_MAX, "SparkTerrainDirectionName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), SparkTerrainDirectionName[ (uint)_data ].c_str());
      break;
    }

    case SDT_TERRAIN:
    {
      this->CheckIndexData( _data, SET_MAX, "SparkTerrainName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), SparkTerrainName[ (uint)_data ].c_str());
      break;
    }

    case SDT_TELEMETRIC_DIR:
    {
      this->CheckIndexData( _data, STMD_MAX, "SparkTelemetricDirectionName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), SparkTelemetricDirectionName[ (uint)_data ].c_str());
      break;
    }

    case SDT_DOMSPEED_DIR:
    {
      this->CheckIndexData( _data, SDOM_MAX, "SparkDominantSpeedDirectionName");
      this->addToLinebuf("%s:%d:%s: ", _info.c_str(), this->GetLineItemCounter(), SparkDominantSpeedDirectionName[ (uint)_data ].c_str());
      break;
    }

    case SDT_NO_PRINT:
    {
      this->DecAndGetLineItemCounter();
      break;
    }

    default:
    {
      printf("Wrong data type; type='%d'\n", _type);
      DOES_NOT_REACH();
    }
  }
}

template <typename ItemType> void
LogCreator::CollectRecord(TVArray(ItemType) _data,
                         COUNT_T _item_index,
                         const std::string _info[],
                         const uint _type[]
                         )
{
  ItemType* data = _data.at(_item_index)->getRecordDataPtr();
  const COUNT_T size = _data.at(_item_index)->getCount();

  for (COUNT_T index = 0; index < size; index++)
  {
    this->CollectData<ItemType>(data[index], _info[index], _type[index]);
  }
}

template <typename ItemType> void
LogCreator::CollectCustomPropertyRecord(TVArray(ItemType) _data,
                                       COUNT_T _item_index,
                                       const std::string _info[],
                                       const uint _type[],
                                       uint _sparktype
                                       )
{
  const uint count = SparkCustomPropertyTypeSize[ _sparktype ];
  const uint start = SparkCustomPropertyStartType[ _sparktype ];

  ItemType* data = _data.at( _item_index )->getRecordDataPtr();
  for (COUNT_T index = 0; index < count; index++)
  {
    this->CollectData<ItemType>( data[index], _info[ start + index ], _type[ start + index ] );
  }
}

template <typename ItemType> void
LogCreator::CollectCustomLocation(TVLArray(ItemType) _data,
                                 COUNT_T _item_index,
                                 const std::string _info[],
                                 uint _sparktype
                                 )
{
  TArray(ItemType)* array = _data.at( _item_index );
  const COUNT_T recordCount = array->getCount();
  const uint start = SparkCustomLocationStartType[ _sparktype ];
  for (COUNT_T recordIndex = 0; recordIndex < recordCount; recordIndex++)
  {
    TRecord(ItemType)* record = array->getRecordPtr( recordIndex );
    this->addToLinebuf("%s:%d:", _info[ start + recordIndex ].c_str(), this->IncAndGetLineItemCounter());
    for (COUNT_T index = 0; index < record->getCount() - 1; ++index)
    {
      this->addToLinebuf( LOCATION_PRINT_FORMAT " ", record->getData( index ) );
    }
    this->addToLinebuf( LOCATION_PRINT_FORMAT ": ", record->getData( record->getCount() - 1 ) );
  }
}

template <typename ItemType> void
LogCreator::CollectInts(TVector(ItemType) _data, const std::string info[])
{
  const COUNT_T size = _data.size();
  for (COUNT_T int_index = 0; int_index < size; int_index++)
  {
    this->addToLinebuf("%s:%d:%d: ", info[ int_index ].c_str(), this->IncAndGetLineItemCounter(), _data[ int_index ]);
  }
}

COUNT_T
LogCreator::PrintnFWA(LogQueueItem* _item)
{
  const SparkSubState sparkSubState = (SparkSubState)_item->count_records.at(LSLCAAI_STATES)->getData(SSET_SUBSTATE);
  const SparkState sparkState = (SparkState)_item->count_records.at(LSLCAAI_STATES)->getData(SSET_STATE);
  if ( (sparkState == SS_BORN) && (sparkSubState != SSS_FINISH) )
  {
    return 0;
  }

  const std::string params_name = "Spark";
  const std::string desc = _item->desc;
  const std::string calculatedFunctionValue_name = "HFV";
  const std::string realAmaxFunction = "REAL_AMAX";
  const std::string fitnessFunction_name = "FF";
  const std::string ints_name[] = { "SP_ITER", "FW_ITER" };

  this->ZeroPosBuf();
  this->ZeroLineItemCounter();

  if ( ! desc.empty() )
  {
    this->addToLinebuf("%s:%d:", desc.c_str(), this->IncAndGetLineItemCounter());
  }

  this->CollectInts<COUNT_T>(_item->ints, ints_name);

  this->CollectRecord<PREC>(_item->prec_records, LSAAI_PROPERTY, SparkPropertyTypeName, SparkPropertyDataTypeType);

  SparkType sparktype = (SparkType)_item->count_records.at(LSLCAAI_STATES)->getRecordDataPtr()[SSET_TYPE];
  this->CollectRecord<COUNT_T>(_item->count_records, LSLCAAI_STATES, SparkStateTypeName, SparkStateDataTypeType);

  this->CollectCustomPropertyRecord<PREC>(_item->prec_records, LSAAI_CUSTOMS, SparkCustomPropertyTypeName, SparkCustomPropertyDataTypeType, sparktype);

  this->addToLinebuf("%s:%d:", params_name.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LSAAI_PARAMS);

  this->CollectRecord<PREC>(_item->prec_records, LSAAI_STATISTICS, SparkStatisticTypeName, SparkStatisticDataTypeType);

  this->addToLinebuf("%s:%d:", realAmaxFunction.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LSAAI_REALAMAXFUNCTION);

  this->CollectCustomLocation<PREC>(_item->arrays, LSLAAI_CUSTOMLOCATION, SparkCustomLocationTypeName, sparktype);

  this->addToLinebuf("%s:%d:", calculatedFunctionValue_name.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LSAAI_CALCFUNCTIONVALUE);

  this->addToLinebuf("%s:%d:", fitnessFunction_name.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LSAAI_FITNESSFUNCTION);

  this->addToLinebuf("%s", "\n");

  return this->GetPosBuf();
}

COUNT_T
LogCreator::PrintSwarm(LogQueueItem* _item)
{
  const std::string name = "Swarm";
  const std::string veloname = "VELO";
  const std::string calculatedFunctionValue_name = "HFV";
  const std::string desc = _item->desc;

  this->ZeroPosBuf();
  this->ZeroLineItemCounter();

  if ( ! desc.empty() )
  {
    this->addToLinebuf("%s:%d:", desc.c_str(), this->IncAndGetLineItemCounter());
  }

  this->CollectRecord<PREC>(_item->prec_records, LSPAAI_PROPERTY, SwarmPropertyTypeName, SwarmPropertyDataTypeType);
  this->addToLinebuf("%s:%d:", name.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LSPAAI_PARAMS);
  this->addToLinebuf("%s:%d:", veloname.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LSPAAI_VELO);
  this->addToLinebuf("%s:%d:", calculatedFunctionValue_name.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LSPAAI_CALCFUNCTIONVALUE);

  this->addToLinebuf("%s", "\n");

  return this->GetPosBuf();
}

COUNT_T
LogCreator::PrintoFWA(LogQueueItem* _item)
{
  const std::string name = "Spark";
  const std::string desc = _item->desc;
  const std::string calculatedFunctionValue_name = "HFV";

  this->ZeroPosBuf();
  this->ZeroLineItemCounter();

  if ( ! desc.empty() )
  {
    this->addToLinebuf("%s:%d:", desc.c_str(), this->IncAndGetLineItemCounter());
  }

  this->CollectRecord<PREC>(_item->prec_records, LOSAAI_PROPERTY, SFWAPropertyTypeName, SFWAPropertyDataTypeType);
  this->addToLinebuf("%s:%d:", name.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LOSAAI_PARAMS);
  this->addToLinebuf("%s:%d:", calculatedFunctionValue_name.c_str(), this->IncAndGetLineItemCounter());
  this->CollectLocation<PREC>(_item->prec_records, LOSAAI_CALCFUNCTIONVALUE);

  this->addToLinebuf("%s", "\n");

  return this->GetPosBuf();
}
