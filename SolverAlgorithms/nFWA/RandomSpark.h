#pragma once

#include "../../common/Types.h"
#include "Spark.h"
#include "Firework.h"
#include "BestSpark.h"

class RandomSpark : public Spark
{
  private:
    TRandomSpark* sparkList = NULL;
    void NextMovingProfile();

  public:
    explicit RandomSpark(Fireworks* _fireworks) : Spark(_fireworks) {}
    ~RandomSpark() {}

    void SaveData() { this->SaveNamedDataToHistory(); }
    void Initialize(Boundaries _boundaries);
    void GenerateNextParam();

    void SetSparkList(TRandomSpark* _list) { this->sparkList = _list; }
    TRandomSpark* GetSparkList() { return this->sparkList; }

   void CalculateAmaxFunction(TRecord(PREC)* _lastBestRealAmax,/* TRecord(PREC)* _lastBestAmax,*/ PREC _lastBestFitness);
};
