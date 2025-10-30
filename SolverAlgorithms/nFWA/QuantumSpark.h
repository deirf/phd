#pragma once

#include "../../common/Types.h"
#include "Spark.h"
#include "Firework.h"

class QuantumSpark : public Spark
{
  private:
    PREC Alpha = 1.2f;
    TQuantumSpark* sparkList = NULL;
    TRecord(PREC)* best_params_avg = NULL;
    TRecord(PREC)* plannedPosition = NULL;

    void NextMovingProfile();
    PREC CalculateNewPosition(COUNT_T index);

    void UpdateLocalBest();
    PREC UpdateAlpha();
    void CalculateBestParamAVG();

  public:
    explicit QuantumSpark(Fireworks* _fireworks) : Spark(_fireworks) {}
    ~QuantumSpark() {}

    void Initialize(Boundaries _boundaries);
    void DeInitialize();
    void GenerateNextParam();

    void SetSparkList(TQuantumSpark* _list) { this->sparkList = _list; }
    TQuantumSpark* GetSparkList() { return this->sparkList; }

    void CalculateMovingBase();

    void CallSpecialPostCalculation() override;
    void CallSpecialInitPostCalculation() override;

    PREC GetAlpha() { return this->GetCustomData<PREC>(SCPT_QUANTUM_ALPHA); }
    void SetAlpha(PREC _value) { this->SetCustomData<PREC>(SCPT_QUANTUM_ALPHA, _value); }

    PREC GetBestParam(COUNT_T _index) { return this->GetCustomLocationValue( SCLT_QUANTUM_BESTPARAM, _index ); }
    void SetBestParam(COUNT_T _index, PREC _value) { return this->SetCustomLocationValue( SCLT_QUANTUM_BESTPARAM, _index, _value ); }

    PREC GetBestFitness() { return this->GetCustomPtr()->getData(SCPT_QUANTUM_BEST_FITNESS); }
    void SetBestFitness(PREC _value) { this->GetCustomPtr()->setData(SCPT_QUANTUM_BEST_FITNESS, _value); }

    void SetBestParamsAVG( COUNT_T _index, PREC _value ) { this->GetBestParamsAVGPtr()->setData( _index, _value ); }
    PREC GetBestParamsAVG( COUNT_T _index ) { return this->GetBestParamsAVGPtr()->getData( _index ); }

    TRecord(PREC)* GetBestParamsAVGPtr() { return this->best_params_avg; }
    void SetBestParamsAVGPtr( TRecord(PREC)* _value ) { this->best_params_avg = _value; }

    TRecord(PREC)* GetPlannedPositionPtr() { return this->plannedPosition; }
    void SetPlannedPositionPtr( TRecord(PREC)* _value ) { this->plannedPosition = _value; }

    SparkStore* GetSparkStore() { return this->GetFirework()->GetSparkStore(); }
    BestSpark* GetBestSpark() { return this->GetFirework()->GetBestSpark(); }
    COUNT_T GetNumberOfQuantumSparks() { return this->GetSparkStore()->GetQuantumSparks()->getCount(); }
};
