#include "SparkPopulation.h"
#include "Spark.h"
#include "FireworkSpark.h"
#include "RandomSpark.h"
#include "GaussianSpark.h"
#include "BestSpark.h"
#include "QuantumSpark.h"
#include "Firework.h"
#include "SparkStore.h"

SparkStore::SparkStore(
                       Fireworks* _fireworks,
                       COUNT_T _fw_index,
                       COUNT_T _fw_pop_index,
                       COUNT_T _searching_dimension,
                       COUNT_T _calculatedFunction_N,
                       COUNT_T _fitnessFunction_N
                      )
{
  this->sparkdata.param = new Array<PREC>();
  this->sparkdata.calculatedFunctionValue = new Array<PREC>();
  this->sparkdata.fitnessFunction = new Array<PREC>();
  this->sparkdata.property = new Array<PREC>();
  this->sparkdata.state = new Array<COUNT_T>();
  this->sparkdata.statistic = new Array<PREC>();
  this->sparkdata.custom = new Array<PREC>();
  this->sparkdata.realAmaxFunction = new Array<PREC>();
  this->sparkdata.customLocation = new LArray<PREC>();
  this->sparkdata.spark = new Array<Spark*>();

  this->fireworks = _fireworks;
  this->fw_index = _fw_index;
  this->fw_pop_index = _fw_pop_index;

  this->searching_dimension = _searching_dimension;
  this->calculatedFunction_N = _calculatedFunction_N;
  this->fitnessFunction_N = _fitnessFunction_N;

  this->sparkTypeID =
  {
    { std::type_index( typeid( RandomSpark   ) ) , ST_RANDOM },
    { std::type_index( typeid( GaussianSpark ) ) , ST_GAUSSIAN },
    { std::type_index( typeid( FireworkSpark ) ) , ST_FIREWORK },
    { std::type_index( typeid( QuantumSpark  ) ) , ST_QUANTUM },
    { std::type_index( typeid( BestSpark     ) ) , ST_BEST },
  };
}

SparkStore::~SparkStore()
{
  this->GetQuantumSparks()->free();
  this->GetGaussianSparks()->free();
  this->GetRandomSparks()->free();
  this->GetBestSparks()->free();
  this->GetFireworkSparks()->free();
  this->GetParams()->free();
  this->GetCalculatedFunctionValue()->free();
  this->GetFitnessFunction()->free();
  this->GetPropertys()->free();
  this->GetStates()->free();
  this->GetStatistics()->free();
  this->GetCustoms()->free();
  this->GetSparks()->free();
  this->GetRealAmaxFunction()->free();
  this->GetCustomLocation()->free();
}

void
SparkStore::CreateBestSpark(COUNT_T _number_of_best_sparks)
{
  this->best_sparks = this->CreateSparks<BestSpark, TBestSpark>(_number_of_best_sparks, SBT_NORMAL);
}

void
SparkStore::CreateWorkerSparks(
                               COUNT_T _number_of_firework_sparks,
                               COUNT_T _number_of_random_sparks,
                               COUNT_T _number_of_gaussian_sparks,
                               COUNT_T _number_of_quantum_sparks
                              )
{
  this->firework_sparks = this->CreateSparks<FireworkSpark, TFireworkSpark>(_number_of_firework_sparks, SBT_NORMAL);
  this->random_sparks = this->CreateSparks<RandomSpark, TRandomSpark>(_number_of_random_sparks, SBT_NORMAL);
  this->gaussian_sparks = this->CreateSparks<GaussianSpark, TGaussianSpark>(_number_of_gaussian_sparks, SBT_NORMAL);
  this->quantum_sparks = this->CreateSparks<QuantumSpark, TQuantumSpark>(_number_of_quantum_sparks, SBT_NORMAL);
}

template <typename SparkClass, typename SparkVector> void
SparkStore::KillSparks(SparkVector* _sparks)
{
  while ( _sparks->getCount() != 0 )
  {
    COUNT_T index = _sparks->getCount() - 1;
    SparkClass* spark = _sparks->getData( index );
    this->Kill<SparkClass, SparkVector>( spark );
  }
}

void
SparkStore::KillWorkerSparks()
{
  this->KillSparks<RandomSpark, TRandomSpark>( this->GetRandomSparks() );
  this->KillSparks<GaussianSpark, TGaussianSpark>( this->GetGaussianSparks() );
  this->KillSparks<FireworkSpark, TFireworkSpark>( this->GetFireworkSparks() );
  this->KillSparks<QuantumSpark, TQuantumSpark>( this->GetQuantumSparks() );
}

void
SparkStore::KillBestSpark()
{
  this->KillSparks<BestSpark, TBestSpark>( this->GetBestSparks() );
}

template <typename SparkVector> void
SparkStore::MurderingSparks(SparkVector* _sparks)
{
  for ( COUNT_T index = 0; index < _sparks->getCount(); index++)
  {
    _sparks->getData( index )->Murdering();
  }
}

void
SparkStore::MurderingWorkerSparks()
{
  this->MurderingSparks<TRandomSpark>( this->GetRandomSparks() );
  this->MurderingSparks<TGaussianSpark>( this->GetGaussianSparks() );
  this->MurderingSparks<TFireworkSpark>( this->GetFireworkSparks() );
  this->MurderingSparks<TQuantumSpark>( this->GetQuantumSparks() );
}

void
SparkStore::MurderingBestSpark()
{
  this->MurderingSparks<TBestSpark>( this->GetBestSparks() );
}

Spark*
SparkStore::GetSparkBySparkID(COUNT_T _ID)
{
  TSparkListMap::iterator it;

  it = this->sparklistmap.find( _ID );
  assume( it != sparklistmap.end(), "ListMap itertation error" );

  COUNT_T sparkIndex = it->second;
  Spark* spark = this->GetSpark( sparkIndex );
  assume( spark->GetID() == _ID, "Spark ID error; sparkid='%ld', id='%ld'", spark->GetID(), _ID);

  return spark;
}

void
SparkStore::DeleteSparkFromSparkMap(COUNT_T _ID)
{
  this->sparklistmap.erase( _ID );
}

void
SparkStore::AddSparkToSparkMap(Spark* _spark)
{
  this->sparklistmap[ _spark->GetID() ] = _spark->GetRecordIndex();
}
