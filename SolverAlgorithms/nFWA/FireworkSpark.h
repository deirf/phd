#pragma once

#include "../../common/Types.h"
#include "Spark.h"

class FireworkSpark : public Spark
{
  private:
    TFireworkSpark* sparkList = NULL;
    void NextMovingProfile();

  public:
    explicit FireworkSpark(Fireworks* _fireworks) : Spark(_fireworks) {}
    ~FireworkSpark() {}

    void Initialize(Boundaries _boundaries);
    void GenerateNextParam();

    void SetSparkList(TFireworkSpark* _list) { this->sparkList = _list; }
    TFireworkSpark* GetSparkList() { return this->sparkList; }

    void CalculateMovingBase();

    PREC GetAmax();
    void SetAmax(PREC _Amax);
};
