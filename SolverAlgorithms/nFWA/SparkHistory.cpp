#include "SparkHistory.h"

void
SparkHistory::AddNewEmptyHistoryData()
{
  SparkRecordData* newdata = new SparkRecordData;
  SetSparkRecordDataToEmpty(newdata);
  this->GetHistoryStore()->addData(newdata);
}

void
SparkHistory::FreeHistoryData(COUNT_T _index)
{
  SparkRecordData* historyData = this->GetHistoryData(_index);
  assume(historyData, "HistoryData is NULL");
  FreeSparkRecordDataPtrs(historyData);
}

void
SparkHistory::RotateHistoryData()
{
  for (COUNT_T index = SH_FIRST + this->GetUnnamedHistorySize() - 1; index > SH_FIRST; --index)
  {
    SparkRecordData* historyData_1 = this->GetHistoryData(index - 1);
    SparkRecordData* historyData_2 = this->GetHistoryData(index);
    SetSparkRecordDataPtrs(historyData_2, historyData_1);
  }
}

void
SparkHistory::UpdateUnnamedHistoryData(SparkRecordData* _presentdata)
{
  SparkRecordData* historyData = this->GetHistoryData(SH_FIRST);
  SetSparkRecordDataPtrs(historyData, _presentdata);
}

void
SparkHistory::SetBestHistoryData(SparkRecordData* _presentdata)
{
  SparkRecordData* bestdata = this->GetHistoryData(SH_BEST);
  SetSparkRecordDataPtrs(bestdata, _presentdata);
}

void
SparkHistory::SetWorstHistoryData(SparkRecordData* _presentdata)
{
  SparkRecordData* worstdata = this->GetHistoryData(SH_WORST);
  SetSparkRecordDataPtrs(worstdata, _presentdata);
}

void
SparkHistory::SetHBestHistoryData(SparkRecordData* _presentdata)
{
  SparkRecordData* hbestdata = this->GetHistoryData(SH_HBEST);
  SetSparkRecordDataPtrs(hbestdata, _presentdata);
}

void
SparkHistory::UpdateBestHistoryData()
{
  if (this->GetFitness(SH_FIRST) < this->GetFitness(SH_BEST))
  {
    this->SetBestHistoryData(this->GetHistoryData(SH_FIRST));
  }
}

void
SparkHistory::UpdateWorstHistoryData()
{
  if (this->GetFitness(SH_FIRST) > this->GetFitness(SH_WORST))
  {
    this->SetWorstHistoryData(this->GetHistoryData(SH_FIRST));
  }
}

void
SparkHistory::UpdateHBestHistoryData()
{
  // new HBEST
  if (this->GetFitness(SH_FIRST) < this->GetFitness(SH_HBEST))
  {
    this->SetHBestHistoryData(this->GetHistoryData(SH_FIRST));
    return;
  }

  // new HBEST is in history -- nothing to do
  if ( this->GetIter(SH_FIRST) - this->GetIter(SH_HBEST) <  this->GetUnnamedHistorySize() )
  {
    return;
  }

// need to find the HBEST because the history does not contain the old one
  if ( this->GetFitness(SH_FIRST) > this->GetFitness(SH_HBEST) )
  {
    SparkRecordData* hbestdata = this->GetHistoryData(SH_FIRST);
    for ( COUNT_T i = SH_FIRST + 1; i < SH_FIRST + this->GetUnnamedHistorySize(); i++ )
    {
      if ( this->GetFitness(i) < this->GetFitness(hbestdata) )
      {
        hbestdata = this->GetHistoryData(i);
      }
    }

    this->SetHBestHistoryData(hbestdata);
  }
}

void
SparkHistory::UpdateNamedHistoryData()
{
  this->UpdateBestHistoryData();
  this->UpdateWorstHistoryData();
  this->UpdateHBestHistoryData();
}

void
SparkHistory::SetNewHistoryData(SparkRecordData* _presentdata)
{
  switch (this->GetHistoryState())
  {
    case HS_CREATE:
      {
        for (COUNT_T index = SH_BEST; index < SH_FIRST + 1; index++)
        {
          this->AddNewEmptyHistoryData();
        }

        this->SetBestHistoryData(_presentdata);
        this->SetWorstHistoryData(_presentdata);
        this->SetHBestHistoryData(_presentdata);
        this->UpdateUnnamedHistoryData(_presentdata);
        this->SetHistoryState(HS_INIT);
        break;
      }

    case HS_INIT:
      {
        this->AddNewEmptyHistoryData();
        this->RotateHistoryData();
        this->UpdateUnnamedHistoryData(_presentdata);
        this->UpdateNamedHistoryData();
        if ( this->GetUnnamedHistorySize() == SPARK_UNNAMED_HISTORY_SIZE )
        {
          this->SetHistoryState(HS_FLOW);
        }
        break;
      }

    case HS_FLOW:
      {
        this->RotateHistoryData();
        this->UpdateUnnamedHistoryData(_presentdata);
        this->UpdateNamedHistoryData();
        break;
      }
    default:
    {
      DOES_NOT_REACH();
    }
  }
}

SparkHistory::SparkHistory(THistoryData* _historydata)
{
  this->SetHistoryStore(_historydata);
  this->SetHistoryState(HS_CREATE);
}

void
SparkHistory::Free()
{
  for (COUNT_T i = 0; i < this->GetHistorySize(); i++)
  {
    this->FreeHistoryData(i);
    delete this->GetHistoryData(i);
  }
  this->GetHistoryStore()->free();
}

void
SparkHistory::CheckHistoryDataIndex(COUNT_T _index)
{
  assume( (_index < this->GetHistorySize()), "index is not less that histroy size; index='%ld', history size='%ld'",_index, this->GetHistorySize() );
}

SparkRecordData*
SparkHistory::GetHistoryData(COUNT_T _index)
{
  this->CheckHistoryDataIndex(_index);
  return this->GetHistoryStore()->getData(_index);
}
