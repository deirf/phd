#pragma once

#include <list>
#include "../../common/Types.h"
#include "../../common/Array.h"
#include "../../common/LArray.h"
#include "Spark.h"
#include "Fireworks.h"
#include "Common.h"

class SparkStore
{
private:
  SparkArrayData sparkdata;
  Fireworks* fireworks = NULL;

  TRandomSpark* random_sparks = NULL;
  TGaussianSpark* gaussian_sparks = NULL;
  TFireworkSpark* firework_sparks = NULL;
  TBestSpark* best_sparks = NULL;
  TQuantumSpark* quantum_sparks = NULL;

  TSparkListMap sparklistmap;

  std::unordered_map< std::type_index, int > sparkTypeID;

  COUNT_T searching_dimension = 0;
  COUNT_T qcurve_points = 0;
  COUNT_T calculatedFunction_N = 0;
  COUNT_T fitnessFunction_N = 0;

  COUNT_T fw_index = 0;
  COUNT_T fw_pop_index = 0;
  COUNT_T spark_id_counter = 0;

  template <typename SparkClass> SparkClass* CreateOneSpark();
  template <typename SparkVector> SparkVector* CreateSparkArray(COUNT_T _number_of_sparks);
  template <typename SparkClass, typename SparkVector> SparkVector* CreateSparks(COUNT_T _number_of_sparks, SparkBornType _bornType);
  template <typename SparkClass> COUNT_T CreateSparkData(SparkClass* _spark);
  template <typename SparkClass> void SetSparkData(SparkClass* _spark, COUNT_T _recordIndex, COUNT_T _sparkIndex, COUNT_T _ID);
  template <typename SparkClass> void SparkFree(SparkClass* _spark);

public:
  SparkStore(Fireworks* _fireworks,
             COUNT_T _fw_index,
             COUNT_T _fw_pop_index,
             COUNT_T _searching_dimension,
             COUNT_T _calculatedFunction_N,
             COUNT_T _fitnessFunction_N);
  ~SparkStore();

  void CreateBestSpark(COUNT_T _number_of_best_sparks);

  void CreateWorkerSparks(
                          COUNT_T _number_of_firework_sparks,
                          COUNT_T _number_of_random_sparks,
                          COUNT_T _number_of_gaussian_sparks,
                          COUNT_T _number_of_quantum_sparks
                         );

  template <typename SparkClass, typename SparkVector>
    void KillSparks(SparkVector* _sparks);
  void KillWorkerSparks();
  void KillBestSpark();

  template <typename SparkVector>
    void MurderingSparks(SparkVector* _sparks);
  void MurderingWorkerSparks();
  void MurderingBestSpark();

  COUNT_T GetNextSparkIDCounter() { return this->spark_id_counter++; } // postincrement !

  template <typename SparkClass, typename SparkVector> SparkClass* CreateSpark(SparkVector* _sparks, COUNT_T* _sparkIndex);
  template <typename SparkClass, typename SparkVector> bool Kill(SparkClass* _spark);
  template <typename SparkClass, typename SparkVector> SparkClass* Born(SparkVector* _sparks, SparkBornType _bornType);

  TArray(PREC)* GetParams() { return this->sparkdata.param; }
  TArray(PREC)* GetCalculatedFunctionValue() { return this->sparkdata.calculatedFunctionValue; }
  TArray(PREC)* GetFitnessFunction() { return this->sparkdata.fitnessFunction; }
  TArray(PREC)* GetPropertys() { return this->sparkdata.property; }
  TArray(COUNT_T)* GetStates() { return this->sparkdata.state; }
  TArray(PREC)* GetStatistics() { return this->sparkdata.statistic; }
  TArray(PREC)* GetCustoms() { return this->sparkdata.custom; }
  TArray(PREC)* GetRealAmaxFunction() { return this->sparkdata.realAmaxFunction; }
  TLArray(PREC)* GetCustomLocation() { return this->sparkdata.customLocation; }
  TArray(Spark*)* GetSparks() { return this->sparkdata.spark; }

  TRecord(PREC)* GetParamPtr(COUNT_T _recordIndex) { return this->GetParams()->getRecordPtr(_recordIndex); }
  TRecord(PREC)* GetCalculatedFunctionValuePtr(COUNT_T _recordIndex) { return this->GetCalculatedFunctionValue()->getRecordPtr(_recordIndex); }
  TRecord(PREC)* GetFitnessFunctionPtr(COUNT_T _recordIndex) { return this->GetFitnessFunction()->getRecordPtr(_recordIndex); }
  TRecord(PREC)* GetPropertyPtr(COUNT_T _recordIndex) { return this->GetPropertys()->getRecordPtr(_recordIndex); }
  TRecord(COUNT_T)* GetStatePtr(COUNT_T _recordIndex) { return this->GetStates()->getRecordPtr(_recordIndex); }
  TRecord(PREC)* GetStatisticPtr(COUNT_T _recordIndex) { return this->GetStatistics()->getRecordPtr(_recordIndex); }
  TRecord(PREC)* GetCustomPtr(COUNT_T _recordIndex) { return this->GetCustoms()->getRecordPtr(_recordIndex); }
  TRecord(PREC)* GetRealAmaxFunctionPtr(COUNT_T _recordIndex) { return this->GetRealAmaxFunction()->getRecordPtr(_recordIndex); }
  TArray(PREC)* GetCustomLocationPtr(COUNT_T _recordIndex) { return this->GetCustomLocation()->getArrayPtr(_recordIndex); }
  TRecord(Spark*)* GetSparkPtr(COUNT_T _recordIndex) { return this->GetSparks()->getRecordPtr(_recordIndex); }
  Spark* GetSpark(COUNT_T _recordIndex) { return this->GetSparks()->getRecordPtr(_recordIndex)->getData(SPARKLIST_SPARKINDEX); }

  TRandomSpark* GetRandomSparks() { return this->random_sparks; }
  TGaussianSpark* GetGaussianSparks() { return this->gaussian_sparks; }
  TFireworkSpark* GetFireworkSparks() { return this->firework_sparks; }
  TBestSpark* GetBestSparks() { return this->best_sparks; }
  TQuantumSpark* GetQuantumSparks() { return this->quantum_sparks; }

  FireworkSpark* GetFireworkSpark(COUNT_T _index) { return this->GetFireworkSparks()->getData(_index); }
  FireworkSpark* GetFireworkSpark() { return this->GetFireworkSpark(FIREWORK_SPARK_INDEX); }
  BestSpark* GetBestSpark(COUNT_T _index) { return this->GetBestSparks()->getData(_index); }
  BestSpark* GetBestSpark() { return this->GetBestSpark(BEST_SPARK_INDEX); }
  RandomSpark* GetRandomSpark(COUNT_T _index) { return this->GetRandomSparks()->getData(_index); }
  GaussianSpark* GetGaussianSpark(COUNT_T _index) { return this->GetGaussianSparks()->getData(_index); }
  QuantumSpark* GetQuantumSpark(COUNT_T _index) { return this->GetQuantumSparks()->getData(_index); }

  void AddSparkToSparkMap(Spark* _spark);
  void DeleteSparkFromSparkMap(COUNT_T _ID);
  Spark* GetSparkBySparkID(COUNT_T _ID);

  COUNT_T GetParamCount() { return this->searching_dimension; }
  COUNT_T GetOutputFunctionSize() { return this->calculatedFunction_N; }
  COUNT_T GetFitnessFunctionSize() { return this->fitnessFunction_N; }
};


template <typename SparkClass> SparkClass*
SparkStore::CreateOneSpark()
{
  return new SparkClass(this->fireworks);
}

template <typename SparkVector> SparkVector*
SparkStore::CreateSparkArray(COUNT_T _number_of_sparks)
{
  return new SparkVector(_number_of_sparks);
}

template <typename SparkClass, typename SparkVector> SparkVector*
SparkStore::CreateSparks(COUNT_T _number_of_sparks, SparkBornType _bornType)
{
  SparkVector* sparks = this->CreateSparkArray<SparkVector>(_number_of_sparks);
  for (COUNT_T index = 0; index < _number_of_sparks; index++)
  {
    this->Born<SparkClass, SparkVector>(sparks, _bornType);
  }

  return sparks;
}

template <typename SparkClass> COUNT_T
SparkStore::CreateSparkData(SparkClass* _spark)
{
  SparkRecordData* data = new SparkRecordData();
  Stats* stats = new Stats();
  THistoryData* historydata = new THistoryData(SH_MAX);
  SparkHistory* history = new SparkHistory(historydata);
  _spark->SetSparkPtrs(data,
                       stats,
                       history
                       );

  const COUNT_T recordIndex1 = this->GetParams()->addRecord(this->searching_dimension);
  this->GetParamPtr(recordIndex1)->setCount(this->searching_dimension);

  const COUNT_T recordIndex3 = this->GetPropertys()->addRecord(SPT_MAX);
  this->GetPropertyPtr(recordIndex3)->setCount(SPT_MAX);

  const COUNT_T recordIndex4 = this->GetStates()->addRecord(SSET_MAX);
  this->GetStatePtr(recordIndex4)->setCount(SSET_MAX);

  const COUNT_T recordIndex5 = this->GetStatistics()->addRecord(SSCT_MAX);
  this->GetStatisticPtr(recordIndex5)->setCount(SSCT_MAX);

  const COUNT_T recordIndex6 = this->GetCustoms()->addRecord(LargestCustomProperty);
  this->GetCustomPtr(recordIndex6)->setCount(LargestCustomProperty);

  const COUNT_T recordIndex7 = this->GetSparks()->addRecord(SPARKLIST_SPARKCOUNT);
  this->GetSparkPtr(recordIndex7)->setCount(SPARKLIST_SPARKCOUNT);

  const COUNT_T recordIndex8 = this->GetCalculatedFunctionValue()->addRecord(this->calculatedFunction_N);
  this->GetCalculatedFunctionValuePtr(recordIndex8)->setCount(this->calculatedFunction_N);

  const COUNT_T recordIndex9 = this->GetFitnessFunction()->addRecord(this->fitnessFunction_N);
  this->GetFitnessFunctionPtr(recordIndex9)->setCount(this->fitnessFunction_N);

  const COUNT_T recordIndex10 = this->GetRealAmaxFunction()->addRecord(this->searching_dimension);
  this->GetRealAmaxFunctionPtr(recordIndex10)->setCount(this->searching_dimension);

  const COUNT_T arraySize = SparkCustomLocationTypeSize[ this->sparkTypeID[ std::type_index( typeid( SparkClass ) ) ] ];
  assume( arraySize > 0, "SparkCustomLocationType size is 0");

  const COUNT_T arrayIndex1 = this->GetCustomLocation()->addArray( new Array<PREC>( this->searching_dimension, arraySize ) );
  for (COUNT_T index = 0; index < this->GetCustomLocationPtr(arrayIndex1)->getSize(); ++index)
  {
    this->GetCustomLocationPtr(arrayIndex1)->getRecordPtr(index)->setCount(this->searching_dimension);
  }

  assume(
         (recordIndex1 == arrayIndex1) &&
         (recordIndex1 == recordIndex3) &&
         (recordIndex1 == recordIndex4) &&
         (recordIndex1 == recordIndex5) &&
         (recordIndex1 == recordIndex6) &&
         (recordIndex1 == recordIndex7) &&
         (recordIndex1 == recordIndex8) &&
         (recordIndex1 == recordIndex9) &&
         (recordIndex1 == recordIndex10)
         ,
         "Recordindex error"
        );

  return recordIndex1;
}

template <typename SparkClass> void
SparkStore::SetSparkData(SparkClass* _spark, COUNT_T _recordIndex, COUNT_T _sparkIndex, COUNT_T _ID)
{
  _spark->SetParamPtr(this->GetParamPtr(_recordIndex));
  _spark->SetCalculatedFunctionValuePtr(this->GetCalculatedFunctionValuePtr(_recordIndex));
  _spark->SetFitnessFunctionPtr(this->GetFitnessFunctionPtr(_recordIndex));
  _spark->SetPropertyPtr(this->GetPropertyPtr(_recordIndex));
  _spark->SetStatisticPtr(this->GetStatisticPtr(_recordIndex));
  _spark->SetStatePtr(this->GetStatePtr(_recordIndex));
  _spark->SetCustomPtr(this->GetCustomPtr(_recordIndex));
  _spark->SetSparkPtr(this->GetSparkPtr(_recordIndex));
  _spark->SetRealAmaxFunctionPtr(this->GetRealAmaxFunctionPtr(_recordIndex));
  _spark->SetCustomLocationPtr(this->GetCustomLocationPtr(_recordIndex));
  _spark->SetSpark( static_cast<Spark*>(_spark) );

  _spark->SetID(_ID);

  _spark->SetSparkIndexes(this->fw_index,
                          this->fw_pop_index,
                          _recordIndex,
                          _sparkIndex
                          );
}

template <typename SparkClass> void
SparkStore::SparkFree(SparkClass* _spark)
{
  _spark->DeInitialize();
  _spark->GetHistory()->Free();
  delete _spark->GetHistory();

  delete _spark->GetStats();
  delete _spark->GetDataPtr();
  delete _spark;
}

template <typename SparkClass, typename SparkVector> SparkClass*
SparkStore::CreateSpark(SparkVector* _sparks, COUNT_T* _sparkIndex)
{
  SparkClass* spark = this->CreateOneSpark<SparkClass>();
  *_sparkIndex = _sparks->addData(spark);

  return spark;
}

template <typename SparkClass, typename SparkVector> bool
SparkStore::Kill(SparkClass* _spark)
{
  // delete spark and move last spark data to deleted place
  // so sparkindex and recordIndex are as same as deleted

  COUNT_T recordIndex = _spark->GetRecordIndex();
  COUNT_T sparkIndex = _spark->GetIndex();
  COUNT_T sparkID = _spark->GetID();
  SparkVector* _sparks =_spark->GetSparkList();

  assume(
          ( this->GetParams()->getLastIndex() == this->GetCalculatedFunctionValue()->getLastIndex() ) &&
          ( this->GetParams()->getLastIndex() == this->GetFitnessFunction()->getLastIndex() ) &&
          ( this->GetParams()->getLastIndex() == this->GetPropertys()->getLastIndex() ) &&
          ( this->GetParams()->getLastIndex() == this->GetStates()->getLastIndex() ) &&
          ( this->GetParams()->getLastIndex() == this->GetStatistics()->getLastIndex() ) &&
          ( this->GetParams()->getLastIndex() == this->GetCustoms()->getLastIndex() ) &&
          ( this->GetParams()->getLastIndex() == this->GetSparks()->getLastIndex() ) &&
          ( this->GetParams()->getLastIndex() == this->GetCustomLocation()->getLastIndex() ) &&
          ( this->GetParams()->getLastIndex() == this->GetRealAmaxFunction()->getLastIndex() )
          ,
          "Spark lastindex error"
        );

  bool resPa = this->GetParams()->delRecord(recordIndex);
  bool resHfv = this->GetCalculatedFunctionValue()->delRecord(recordIndex);
  bool resFf = this->GetFitnessFunction()->delRecord(recordIndex);
  bool resPr = this->GetPropertys()->delRecord(recordIndex);
  bool resSt = this->GetStates()->delRecord(recordIndex);
  bool resSc = this->GetStatistics()->delRecord(recordIndex);
  bool resCu = this->GetCustoms()->delRecord(recordIndex);
  bool resSp = this->GetSparks()->delRecord(recordIndex);
  bool resRam = this->GetRealAmaxFunction()->delRecord(recordIndex);
  bool resCr = this->GetCustomLocation()->delArray(recordIndex);

  assume(
          ( resPa == resHfv ) &&
          ( resPa == resFf ) &&
          ( resPa == resPr ) &&
          ( resPa == resSt ) &&
          ( resPa == resSc ) &&
          ( resPa == resCu ) &&
          ( resPa == resRam ) &&
          ( resPa == resCr ) &&
          ( resPa == resSp )
          ,
          "Spark deletion index error"
        );

  this->SparkFree<SparkClass>( _spark );
  this->DeleteSparkFromSparkMap( sparkID );

  if ( resPa ) // is there re-placed ?
  {
    Spark* moved_spark = this->GetSpark(recordIndex);
    this->SetSparkData<SparkClass>( static_cast<SparkClass*>( moved_spark ), recordIndex, moved_spark->GetIndex(), moved_spark->GetID() );
    this->AddSparkToSparkMap(moved_spark);
  }

  bool spark_moving =  _sparks->delData(sparkIndex, 0);
  if ( spark_moving ) // is there re-placed ?
  {
    Spark* moved_spark = _sparks->getData(sparkIndex);
    this->SetSparkData<SparkClass>( static_cast<SparkClass*>( moved_spark ), moved_spark->GetRecordIndex(), sparkIndex, moved_spark->GetID() );
  }

  return spark_moving;
}


template <typename SparkClass, typename SparkVector> SparkClass*
SparkStore::Born(SparkVector* _sparks, SparkBornType _bornType)
{
  SparkClass* spark = NULL;

  switch (_bornType)
  {
    case SBT_NORMAL:
    {
      COUNT_T sparkIndex = 0;
      spark = this->CreateSpark<SparkClass, SparkVector>(_sparks, &sparkIndex);

      const COUNT_T recordIndex = this->CreateSparkData<SparkClass>(spark);
      this->SetSparkData<SparkClass>(spark, recordIndex, sparkIndex, this->GetNextSparkIDCounter());
      this->AddSparkToSparkMap(spark);

      spark->SetSparkList(_sparks);
      spark->SetBornType(_bornType);
      spark->SetState(SS_BORN);
      spark->PreInitialize();
      break;
    }
    case SBT_CHILD:
    {
      break;
    }
    default:
    {
      DOES_NOT_REACH();
    }
  }

  return spark;
}
