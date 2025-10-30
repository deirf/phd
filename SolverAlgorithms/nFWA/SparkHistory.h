#pragma once

#include <limits.h>
#include "../../common/Types.h"
#include "../../common/Record.h"
#include "../../common/Array.h"
#include "Common.h"


typedef enum
{
  HS_UNKNOWN = 0,
  HS_CREATE,
  HS_INIT,
  HS_FLOW,
} HistoryState;

class SparkHistory
{
  private:
    THistoryData* historyStore = NULL;
    uint historyState = HS_UNKNOWN;

    void CheckHistoryDataIndex(COUNT_T _index);
    void UpdateUnnamedHistoryData(SparkRecordData* _presentdata);
    void AddNewHistoryData(SparkRecordData* _presentdata);
    void FreeHistoryData(COUNT_T _index);
    void RotateHistoryData();
    void UpdateNamedHistoryData();
    void AddNewEmptyHistoryData();
    void SetNewNamedHistoryData();
    void SetNewUnnamedHistoryData();
    void SetWorstHistoryData(SparkRecordData* _presentdata);
    void SetBestHistoryData(SparkRecordData* _presentdata);
    void SetHBestHistoryData(SparkRecordData* _presentdata);

    void UpdateBestHistoryData();
    void UpdateWorstHistoryData();
    void UpdateHBestHistoryData();

  public:
    explicit SparkHistory(THistoryData* _historydata);
    ~SparkHistory() {};

    THistoryData* GetHistoryStore() { return this->historyStore; }
    void SetHistoryStore(THistoryData* _historyStore) { this->historyStore = _historyStore; }

    void SetHistoryState(uint _state) { this->historyState = _state; };
    uint GetHistoryState() { return this->historyState; };

    void Free();

    SparkRecordData* GetHistoryData(COUNT_T _index);
    void SetNewHistoryData(SparkRecordData* _presentdata);

    COUNT_T GetUnnamedHistorySize() { return this->GetHistorySize() - this->GetNamedHistorySize(); }
    COUNT_T GetHistorySize() { return this->GetHistoryStore()->getCount(); }
    COUNT_T GetNamedHistorySize() { return this->GetHistorySize() >= SH_NAMED_MAX ? SH_NAMED_MAX : this->GetHistorySize(); }

    TRecord(PREC)* GetParamPtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->param; }
    TRecord(PREC)* GetCalculatedFunctionValuePtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->calculatedFunctionValue; }
    TRecord(PREC)* GetFitnessFunctionPtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->fitnessFunction; }
    TRecord(PREC)* GetPropertyPtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->property; }
    TRecord(PREC)* GetStatisticPtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->statistic; }
    TRecord(COUNT_T)* GetStatePtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->state; }
    TRecord(PREC)* GetCustomPtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->custom; }
    TRecord(PREC)* GetRealAmaxFunctionPtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->realAmaxFunction; }
    TArray(PREC)* GetCustomLocationPtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->customLocation; }
    TRecord(Spark*)* GetSparkPtr(COUNT_T _history_index) { return this->GetHistoryData(_history_index)->spark; }

    COUNT_T GetParamCount(COUNT_T _history_index) { return this->GetParamPtr(_history_index)->getCount(); }
    COUNT_T GetCalculatedFunctionValueCount(COUNT_T _history_index) { return this->GetCalculatedFunctionValuePtr(_history_index)->getCount(); }
    COUNT_T GetFitnessFunctionCount(COUNT_T _history_index) { return this->GetFitnessFunctionPtr(_history_index)->getCount(); }
    COUNT_T GetPropertyCount(COUNT_T _history_index) { return this->GetPropertyPtr(_history_index)->getCount(); }
    COUNT_T GetStatisticCount(COUNT_T _history_index) { return this->GetStatisticPtr(_history_index)->getCount(); }
    COUNT_T GetStateCount(COUNT_T _history_index) { return this->GetStatePtr(_history_index)->getCount(); }
    COUNT_T GetCustomCount(COUNT_T _history_index) { return this->GetCustomPtr(_history_index)->getCount(); }
    COUNT_T GetRealAmaxFunctionCount(COUNT_T _history_index) { return this->GetRealAmaxFunctionPtr(_history_index)->getCount(); }
    COUNT_T GetCustomLocationCount(COUNT_T _history_index) { return this->GetCustomLocationPtr(_history_index)->getCount(); }

    PREC GetCalculatedFunctionValue(COUNT_T _history_index, COUNT_T _index) { return this->GetCalculatedFunctionValuePtr(_history_index)->getData(_index); }
    PREC GetFitnessFunction(COUNT_T _history_index, COUNT_T _index) { return this->GetFitnessFunctionPtr(_history_index)->getData(_index); }
    PREC GetParam(COUNT_T _history_index, COUNT_T _index) { return this->GetParamPtr(_history_index)->getData(_index); }
    PREC GetRealAmaxFunction(COUNT_T _history_index, COUNT_T _index) { return this->GetRealAmaxFunctionPtr(_history_index)->getData(_index); }
    TRecord(PREC)* GetCustomLocation(COUNT_T _history_index, COUNT_T _index) { return this->GetCustomLocationPtr(_history_index)->getRecordPtr(_index); }

    COUNT_T GetType(COUNT_T _history_index) { return (COUNT_T)this->GetStatePtr(_history_index)->getData(SSET_TYPE); }
    COUNT_T GetIndex(COUNT_T _history_index) { return (COUNT_T)this->GetStatePtr(_history_index)->getData(SSET_INDEX); }
    COUNT_T GetID(COUNT_T _history_index) { return (COUNT_T)this->GetStatePtr(_history_index)->getData(SSET_ID); }
    COUNT_T GetFwIndex(COUNT_T _history_index) { return (COUNT_T)this->GetStatePtr(_history_index)->getData(SSET_FW_INDEX); }
    COUNT_T GetFwPopIndex(COUNT_T _history_index) { return (COUNT_T)this->GetStatePtr(_history_index)->getData(SSET_FW_POP_INDEX); }
    COUNT_T GetRecordIndex(COUNT_T _history_index) { return (COUNT_T)this->GetStatePtr(_history_index)->getData(SSET_RECORDINDEX); }
    COUNT_T GetIter(COUNT_T _history_index) { return (COUNT_T)this->GetStatePtr(_history_index)->getData(SSET_ITER); }

    PREC GetFitness(COUNT_T _history_index) { return this->GetPropertyPtr(_history_index)->getData(SPT_FITNESS); }
    PREC GetFitness(SparkRecordData* _sparkdata) { return _sparkdata->property->getData(SPT_FITNESS); }

    PREC GetVELO(COUNT_T _index, COUNT_T _history_index) { return this->GetStatisticPtr(_history_index)->getData(_index + SSCT_VELO_FIRST ); }
    PREC GetACCEL(COUNT_T _index, COUNT_T _history_index) { return this->GetStatisticPtr(_history_index)->getData(_index + SSCT_ACCEL_FIRST); }
};
