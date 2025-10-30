#pragma once

#include "../../common/Types.h"
#include "Spark.h"

class GaussianSpark : public Spark
{
  private:
    void NextMovingProfile(Spark* _spark);
    Spark* SelectOneSpark();
    TGaussianSpark* sparkList = NULL;
    bool bestSparkWasPreviousSelected = FALSE;

  public:
    explicit GaussianSpark(Fireworks* _fireworks) : Spark(_fireworks) {}
    ~GaussianSpark() {}

    void DeInitialize();
    void Initialize(Boundaries _boundaries);
    void GenerateNextParam();
    void SetSparkList(TGaussianSpark* _list) { this->sparkList = _list; }
    TGaussianSpark* GetSparkList() { return this->sparkList; }

    COUNT_T GetSparkID() { return this->GetCustomData<COUNT_T>(SCPT_GAUSSIAN_SPARK_ID); }
    void SetSparkID(COUNT_T _ID) { this->SetCustomData<COUNT_T>(SCPT_GAUSSIAN_SPARK_ID, _ID); }

    COUNT_T GetSparkType() { return this->GetCustomData<COUNT_T>(SCPT_GAUSSIAN_SPARK_TYPE); }
    void SetSparkType(COUNT_T _type) { this->SetCustomData<COUNT_T>(SCPT_GAUSSIAN_SPARK_TYPE, _type); }
};
