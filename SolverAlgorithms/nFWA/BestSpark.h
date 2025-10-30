#pragma once

#include "../../common/Types.h"
#include "Spark.h"

class BestSpark : public Spark
{
  private:
    SparkType origin_type = ST_UNKNOWN;
    Spark* newBestSpark = NULL;
    bool bestSparkChanged = FALSE;
    TBestSpark* sparkList = NULL;
    Spark* bestRandomSpark = NULL;

    PREC firstFitness = PREC_MAX;
    void NextMovingProfile();

  public:
    explicit BestSpark(Fireworks* _fireworks) : Spark(_fireworks) {}
    ~BestSpark(){}

    void Initialize(Boundaries _boundaries);
    void GenerateNextParam();
    void SetSparkList(TBestSpark* _list) { this->sparkList = _list; }
    TBestSpark* GetSparkList() { return this->sparkList; }

    void SetNewBestSpark(Spark* _spark);
    Spark* GetNewBestSpark() { return this->newBestSpark; }
    bool GetBestSparkChanged() { return this->bestSparkChanged; }
    void Reset();

    void DetermineSparksDistance(Spark* _fromSpark, Spark* _toSpark, PREC* _Amax);

    void SetNewBestRandomSpark(Spark* _spark) { this->bestRandomSpark = _spark; }
    Spark* GetNewBestRandomSpark() { return this->bestRandomSpark; } ;
    void CallSpecialPostCalculation() override;
    void CallSpecialInitPostCalculation() override;

    void CalculateBestRandomValues();

    void InitBestEvaluate() override;
    void BestEvaluate() override;

    COUNT_T GetPartnerID() { return this->GetCustomData<COUNT_T>(SCPT_BEST_PARTNER_ID); }
    void SetPartnerID(COUNT_T _value) { this->SetCustomData<COUNT_T>(SCPT_BEST_PARTNER_ID, _value); }

    COUNT_T GetPartnerIter() { return GetCustomData<COUNT_T>(SCPT_BEST_PARTNER_ITER); }
    void SetPartnerIter(COUNT_T _value) { SetCustomData<COUNT_T>(SCPT_BEST_PARTNER_ITER, _value); }

    COUNT_T GetPartnerType() { return GetCustomData<COUNT_T>(SCPT_BEST_PARTNER_TYPE); }
    void SetPartnerType(COUNT_T _value) { SetCustomData<COUNT_T>(SCPT_BEST_PARTNER_TYPE, _value); }

    COUNT_T GetParentPartnerID() { return GetCustomData<COUNT_T>(SCPT_BEST_PARENT_PARTNER_ID); }
    void SetParentPartnerID(COUNT_T _value) { SetCustomData<COUNT_T>(SCPT_BEST_PARENT_PARTNER_ID, _value); }

    COUNT_T GetParentPartnerType() { return GetCustomData<COUNT_T>(SCPT_BEST_PARENT_PARTNER_TYPE); }
    void SetParentPartnerType(COUNT_T _value) { SetCustomData<COUNT_T>(SCPT_BEST_PARENT_PARTNER_TYPE, _value); }

    PREC GetBestRandomFitness() { return this->GetCustomPtr()->getData(SCPT_BEST_RANDOM_FITNESS); }
    void SetBestRandomFitness(PREC _value) { this->GetCustomPtr()->setData(SCPT_BEST_RANDOM_FITNESS, _value); }

    PREC GetBestRandomRealAmax(COUNT_T _index) { return this->GetCustomLocationValue( SCLT_BEST_BESTRANDOM_REAL_AMAX, _index ); }
    void SetBestRandomRealAmax(COUNT_T _index, PREC _value) { return this->SetCustomLocationValue( SCLT_BEST_BESTRANDOM_REAL_AMAX, _index, _value ); }

    PREC GetSparkRealAmax(COUNT_T _index) { return this->GetCustomLocationValue( SCLT_BEST_SPARK_REAL_AMAX, _index ); }
    void SetSparkRealAmax(COUNT_T _index, PREC _value) { return this->SetCustomLocationValue( SCLT_BEST_SPARK_REAL_AMAX, _index, _value ); }

    PREC GetSparkAmax(COUNT_T _index) { return this->GetCustomLocationValue( SCLT_BEST_AMAX, _index ); }
    void SetSparkAmax(COUNT_T _index, PREC _value) { return this->SetCustomLocationValue( SCLT_BEST_AMAX, _index, _value ); }

    PREC GetFirstFitness() { return this->firstFitness; }
    void SetFirstFitness(PREC _value) { this->firstFitness = _value; }
};