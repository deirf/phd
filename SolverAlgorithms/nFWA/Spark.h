#pragma once

#include <typeinfo>
#include <limits.h>

#include "../../common/Types.h"
#include "../../common/Array.h"
#include "../../common/Random.h"
#include "../../common/SimpleShape.h"
#include "../../common/HTCShape.h"
#include "Stats.h"
#include "SparkHistory.h"


class Firework;
class Fireworks;

class Spark
{
  private:
    typedef PREC (Spark::*ParamOffsetCalculationFunction)(COUNT_T _index, COUNT_T _max_index);

    Fireworks* fireworks = NULL;
    SparkRecordData* data = NULL;
    SparkHistory* history = NULL;
    Stats* stats = NULL;

    TRecord(PREC)* direction = NULL;
    TRecord(PREC)* movingBase = NULL;

    bool movingProfile = FALSE;

    SparkBornType bornType= SBT_UNKNOWN;
    COUNT_T numberOfCalculatedTelemetrics = 0;
    bool moved = FALSE;
    bool skipHistory = FALSE;
    COUNT_T qcurve_point_size = 0;
    bool appliedMovingProfile = FALSE;
    bool initialized = FALSE;
    ProblemShape* problemShape = NULL;

  public:
    explicit Spark(Fireworks* _fireworks)  : fireworks( _fireworks ) {}
    virtual ~Spark() {}

    std::string getClassName() { return typeid(this).name(); }

    void Initialize(Boundaries _boundaries);
    void InitializeShapeFunction(COUNT_T _ShapeFunction);
    void PreInitialize();
    void DeInitialize();

    void SetSparkPtrs(SparkRecordData* _data,
                      Stats* _stats,
                      SparkHistory* _history);
    void SetSparkIndexes(COUNT_T _fw_index,
                         COUNT_T _fw_pop_index,
                         COUNT_T _recordIndex,
                         COUNT_T _sparkIndex
                         );

    SparkRecordData* GetDataPtr() { return this->data; }
    void SetDataPtr(SparkRecordData* _data) { this->data = _data; }

    void SetInitialized() { this->initialized = TRUE; }
    bool GetInitialized() { return this->initialized; }

    void SetMovingProfileOn() { this->movingProfile = TRUE; this->appliedMovingProfile = TRUE; }
    void SetMovingProfileOff() { this->movingProfile = FALSE; }
    void CheckMovingProfile() { assume(this->movingProfile, "Moving profile is not set"); assume( this->GetInitialized(), "Spark is not initialized");}
    void CheckAppliedMovingProfile() { assume(this->appliedMovingProfile, "Moving profile is not applied"); this->appliedMovingProfile = FALSE; }

    void CalculateTelemetrics();

    void SetMovingBase(TRecord(PREC)* _param) { this->movingBase = _param; }
    TRecord(PREC)* GetMovingBase() { return this->movingBase; }

    void SetDirection(TRecord(PREC)* _param) { this->direction = _param; }
    TRecord(PREC)* GetDirection() { return this->direction; }

    void SetQCurvePointSize(COUNT_T _count) { this->qcurve_point_size = _count; }
    COUNT_T GetQCurvePointSize() { return this->qcurve_point_size; }

    TRecord(PREC)* GetParamPtr() { return this->GetDataPtr()->param; }
    TRecord(PREC)* GetCalculatedFunctionValuePtr() { return this->GetDataPtr()->calculatedFunctionValue; }
    TRecord(PREC)* GetFitnessFunctionPtr() { return this->GetDataPtr()->fitnessFunction; }
    TRecord(PREC)* GetPropertyPtr() { return this->GetDataPtr()->property; }
    TRecord(PREC)* GetStatisticPtr() { return this->GetDataPtr()->statistic; }
    TRecord(COUNT_T)* GetStatePtr() { return this->GetDataPtr()->state; }
    TRecord(PREC)* GetCustomPtr() { return this->GetDataPtr()->custom; }
    TRecord(PREC)* GetRealAmaxFunctionPtr() { return this->GetDataPtr()->realAmaxFunction; }
    TArray(PREC)* GetCustomLocationPtr() { return this->GetDataPtr()->customLocation; }
    TRecord(Spark*)* GetSparkPtr() { return this->GetDataPtr()->spark; }

    void SetParamPtr(TRecord(PREC)* _param) { this->GetDataPtr()->param = _param; }
    void SetCalculatedFunctionValuePtr(TRecord(PREC)* _calculatedFunctionValue) { this->GetDataPtr()->calculatedFunctionValue = _calculatedFunctionValue; }
    void SetFitnessFunctionPtr(TRecord(PREC)* _fitnessFunction) { this->GetDataPtr()->fitnessFunction = _fitnessFunction; }
    void SetPropertyPtr(TRecord(PREC)* _property) { this->GetDataPtr()->property = _property; }
    void SetStatisticPtr(TRecord(PREC)* _statistic) { this->GetDataPtr()->statistic = _statistic; }
    void SetStatePtr(TRecord(COUNT_T)* _state) { this->GetDataPtr()->state = _state; }
    void SetCustomPtr(TRecord(PREC)* _custom) { this->GetDataPtr()->custom = _custom; }
    void SetRealAmaxFunctionPtr(TRecord(PREC)* _realAmaxFunction) { this->GetDataPtr()->realAmaxFunction = _realAmaxFunction; }
    void SetCustomLocationPtr(TArray(PREC)* _customLocation) { this->GetDataPtr()->customLocation = _customLocation; }
    void SetSparkPtr(TRecord(Spark*)* _spark) { this->GetDataPtr()->spark = _spark; }

    TRecord(PREC)* ChangeParamsRecord(TRecord(PREC)* restrict _new_record);
    void SetParam(COUNT_T _index, PREC _value)
    {
        assume( !(this->GetType() == ST_BEST && this->GetSubState() != SSS_START), "SetParam checking error; type='%ld', state='%ld'", this->GetType(), this->GetSubState() );
        _value = BOTTOMLIMIT( TOPLIMIT( _value, this->GetHmax() ), this->GetHmin() );
        this->GetParamPtr()->setData(_index, _value);
    }

    void SetRealAmaxFunctionValue(COUNT_T _index, PREC _value) { this->GetRealAmaxFunctionPtr()->setData(_index, _value); }

    PREC GetParam(COUNT_T _index) { return this->GetParamPtr()->getData(_index); }
    PREC GetCalculatedFunctionValue(COUNT_T _index) { return this->GetCalculatedFunctionValuePtr()->getData(_index); }
    PREC GetFitnessFunction(COUNT_T _index) { return this->GetFitnessFunctionPtr()->getData(_index); }
    PREC GetRealAmaxFunctionValue(COUNT_T _index) { return this->GetRealAmaxFunctionPtr()->getData(_index); }

    TRecord(PREC)* GetCustomLocation(COUNT_T _location) { return this->GetCustomLocationPtr()->getRecordPtr( _location ); }

    PREC GetCustomLocationValue(COUNT_T _location, COUNT_T _index) { return this->GetCustomLocation( _location )->getData( _index ); }
    void SetCustomLocationValue(COUNT_T _location, COUNT_T _index, PREC _value) { this->GetCustomLocation( _location )->setData( _index, _value ); }

    template <typename dataType>
    void SetCustomData(uint _id, dataType _value)
    {
      assume( (representable_as_type<dataType, PREC>( _value )), "PREC is not representable as dataType" );
      this->GetCustomPtr()->setData( _id, (PREC)_value);
    }

    template <typename dataType>
    dataType GetCustomData(uint _id)
    {
      const PREC raw_data = this->GetCustomPtr()->getData( _id );
      assume( (representable_as_type<PREC, dataType>( raw_data )), "dataType is not representable as PREC" );
      return (dataType) raw_data;
    }

    PREC GetAmaxFunction(COUNT_T _index)
    {
        const uint amaxID = SparkCustomLocationAmaxID[ this->GetType() ];
        return this->GetCustomLocationValue( amaxID, _index );
    }

    TRecord(PREC)* GetAmaxFunctionPtr()
    {
        const COUNT_T amaxID = SparkCustomLocationAmaxID[ this->GetType() ];
        return this->GetCustomLocation( amaxID );
    }

    void SetAmaxFunction(COUNT_T _index, PREC _value)
    {
        const uint amaxID = SparkCustomLocationAmaxID[ this->GetType() ];
        return this->SetCustomLocationValue( amaxID, _index, _value );
    }

    COUNT_T GetParamSize() { return this->GetParamPtr()->getSize(); }
    COUNT_T GetCalculatedFunctionValueSize() { return this->GetCalculatedFunctionValuePtr()->getSize(); }
    COUNT_T GetFitnessFunctionSize() { return this->GetFitnessFunctionPtr()->getSize(); }
    COUNT_T GetPropertySize() { return this->GetPropertyPtr()->getSize(); }
    COUNT_T GetStatisticSize() { return this->GetStatisticPtr()->getSize(); }
    COUNT_T GetStateSize() { return this->GetStatePtr()->getSize(); }
    COUNT_T GetCustomSize() { return this->GetCustomPtr()->getSize(); }
    COUNT_T GetRealAmaxFunctionSize() { return this->GetRealAmaxFunctionPtr()->getSize(); }
    COUNT_T GetCustomLocationSize() { return this->GetCustomLocationPtr()->getSize(); }
    COUNT_T GetSparkSize() { return this->GetSparkPtr()->getSize(); }

    COUNT_T GetParamCount() { return this->GetParamPtr()->getCount(); }
    COUNT_T GetCalculatedFunctionValueCount() { return this->GetCalculatedFunctionValuePtr()->getCount(); }
    COUNT_T GetFitnessFunctionCount() { return this->GetFitnessFunctionPtr()->getCount(); }
    COUNT_T GetPropertyCount() { return this->GetPropertyPtr()->getCount(); }
    COUNT_T GetStatisticCount() { return this->GetStatisticPtr()->getCount(); }
    COUNT_T GetStateCount() { return this->GetStatePtr()->getCount(); }
    COUNT_T GetCustomCount() { return this->GetCustomPtr()->getCount(); }
    COUNT_T GetRealAmaxFunctionCount() { return this->GetRealAmaxFunctionPtr()->getCount(); }
    COUNT_T GetCustomLocationCount() { return this->GetCustomLocationPtr()->getCount(); }
    COUNT_T GetSparkCount() { return this->GetSparkPtr()->getCount(); }

    void SetHistory(SparkHistory* _history) { this->history = _history; }
    SparkHistory* GetHistory() { return this->history; }

    // -- State
    COUNT_T GetType() { return this->GetStatePtr()->getData(SSET_TYPE); }
    void SetType(COUNT_T _type) { this->GetStatePtr()->setData(SSET_TYPE, _type); }

    COUNT_T GetState() { return this->GetStatePtr()->getData(SSET_STATE); }
    void SetState(COUNT_T _state) { this->GetStatePtr()->setData(SSET_STATE, _state); this->SetSubState(SSS_START); }

    COUNT_T GetSubState() { return this->GetStatePtr()->getData(SSET_SUBSTATE); }
    void SetSubState(COUNT_T _substate) { this->GetStatePtr()->setData(SSET_SUBSTATE, _substate); }

    COUNT_T GetID() { return this->GetStatePtr()->getData(SSET_ID); }
    void SetID(COUNT_T _index) { this->GetStatePtr()->setData(SSET_ID, _index); }

    COUNT_T GetIndex() { return this->GetStatePtr()->getData(SSET_INDEX); }
    void SetIndex(COUNT_T _index) { this->GetStatePtr()->setData(SSET_INDEX, _index); }

    COUNT_T GetFwIndex() { return this->GetStatePtr()->getData(SSET_FW_INDEX); }
    void SetFwIndex(COUNT_T _fw_index) { this->GetStatePtr()->setData(SSET_FW_INDEX, _fw_index); }

    COUNT_T GetFwPopIndex() { return this->GetStatePtr()->getData(SSET_FW_POP_INDEX); }
    void SetFwPopIndex(COUNT_T _fw_pop_index) { this->GetStatePtr()->setData(SSET_FW_POP_INDEX, _fw_pop_index); }

    COUNT_T GetRecordIndex() { return this->GetStatePtr()->getData(SSET_RECORDINDEX); }
    void SetRecordIndex(COUNT_T _recordIndex) { this->GetStatePtr()->setData(SSET_RECORDINDEX, _recordIndex); }

    COUNT_T GetIter() { return this->GetStatePtr()->getData(SSET_ITER); }
    void SetIter(COUNT_T _iter) { this->GetStatePtr()->setData(SSET_ITER, _iter); }

    void NextIteration() { this->SetIter(this->GetIter() + 1); }

    COUNT_T GetDmin() { return this->GetStatePtr()->getData(SSET_DMIN); }
    void SetDmin(COUNT_T _Dmin) { this->GetStatePtr()->setData(SSET_DMIN, _Dmin); }

    COUNT_T GetDmax() { return this->GetStatePtr()->getData(SSET_DMAX); }
    void SetDmax(COUNT_T _Dmax) { this->GetStatePtr()->setData(SSET_DMAX, _Dmax); }

    COUNT_T GetDim() { return this->GetStatePtr()->getData(SSET_DIM); }
    void SetDim(COUNT_T _Dim) { this->GetStatePtr()->setData(SSET_DIM, _Dim); }

    SparkTelemetricDirection GetTelemetricDirection() { return (SparkTelemetricDirection)this->GetStatePtr()->getData(SSET_TELEMETRIC_DIR); }
    void SetTelemetricDirection(SparkTelemetricDirection _value) { this->GetStatePtr()->setData(SSET_TELEMETRIC_DIR, _value); }
    SparkTelemetricDirection GetLastTelemetricDirection() { return (SparkTelemetricDirection)this->GetHistory()->GetStatePtr(SH_PREV)->getData(SSET_TELEMETRIC_DIR); }

    SparkTerrainDirection GetTerrainDirection() { return (SparkTerrainDirection)this->GetStatePtr()->getData(SSET_TERRAIN_DIR); }
    void SetTerrainDirection(SparkTerrainDirection _value) { this->GetStatePtr()->setData(SSET_TERRAIN_DIR, _value); }
    SparkTerrainDirection GetLastTerrainDirection() { return (SparkTerrainDirection)this->GetHistory()->GetStatePtr(SH_PREV)->getData(SSET_TERRAIN_DIR); }

    SparkTerrain GetTerrain() { return (SparkTerrain)this->GetStatePtr()->getData(SSET_TERRAIN); }
    void SetTerrain(SparkTerrain _value) { this->GetStatePtr()->setData(SSET_TERRAIN, _value); }
    SparkTerrain GetLastTerrain() { return (SparkTerrain)this->GetHistory()->GetStatePtr(SH_PREV)->getData(SSET_TERRAIN); }

    SparkDominantSpeedDirection GetDominantSpeedDirection() { return (SparkDominantSpeedDirection)this->GetStatePtr()->getData(SSET_DOMSPEED_DIR); }
    void SetDominantSpeedDirection(SparkDominantSpeedDirection _value) { this->GetStatePtr()->setData(SSET_DOMSPEED_DIR, _value); }
    SparkDominantSpeedDirection GetLastDominantSpeedDirection() { return (SparkDominantSpeedDirection)this->GetHistory()->GetStatePtr(SH_PREV)->getData(SSET_DOMSPEED_DIR); }

    uint GetEQTC() { return (uint)this->GetStatePtr()->getData(SSET_EQ_TERRAINCOUNT); }
    void SetEQTC(uint _count) { this->GetStatePtr()->setData(SSET_EQ_TERRAINCOUNT, _count); }
    void SetINCEQTC() { this->GetStatePtr()->setData(SSET_EQ_TERRAINCOUNT, this->GetEQTC() + 1); }

    COUNT_T GetFwCount() { return this->GetStatePtr()->getData(SSET_FWCOUNT); }
    void SetFwCount( COUNT_T _FwCount ) { this->GetStatePtr()->setData(SSET_FWCOUNT, _FwCount); }

    // -- Property
    PREC GetFitness() { return this->GetPropertyPtr()->getData(SPT_FITNESS); }
    void InvalidateFitness() { this->GetPropertyPtr()->setData(SPT_FITNESS, PREC_MAX); }
    void SetFitness(PREC _fitness);

    PREC GetShapeFitness() { return this->GetPropertyPtr()->getData(SPT_SHAPE_FITNESS); }
    void SetShapeFitness(PREC _fitness) { this->GetPropertyPtr()->setData(SPT_SHAPE_FITNESS, _fitness); }

    void SetNormFitness(PREC _fitness) { this->GetPropertyPtr()->setData(SPT_NORM_FITNESS, _fitness); }
    PREC GetNormFitness() { return this->GetPropertyPtr()->getData(SPT_NORM_FITNESS); }

    PREC GetHmin() { return this->GetPropertyPtr()->getData(SPT_HMIN); }
    void SetHmin(PREC _Hmin) { this->GetPropertyPtr()->setData(SPT_HMIN, _Hmin); }

    PREC GetHmax() { return this->GetPropertyPtr()->getData(SPT_HMAX); }
    void SetHmax(PREC _Hmax) { this->GetPropertyPtr()->setData(SPT_HMAX, _Hmax); }

    PREC GetITL() { return this->GetPropertyPtr()->getData(SPT_ITL); }
    void SetITL(PREC _ITL);
    void RelITL(PREC _value) { this->SetITL( this->GetITL() + _value); }

    PREC GetFmin() { return this->GetPropertyPtr()->getData(SPT_FMIN); }
    void SetFmin(PREC _Fmin) { this->GetPropertyPtr()->setData(SPT_FMIN, _Fmin); }

    PREC GetFmax() { return this->GetPropertyPtr()->getData(SPT_FMAX); }
    void SetFmax(PREC _Fmax) { if (UNLIKELY( (_Fmax < 1) && (_Fmax > 0) ) ) { _Fmax = 2*this->GetFmin() + FMAX_IF_FMAX_LESS_THAN_1; } this->GetPropertyPtr()->setData(SPT_FMAX, _Fmax); }

    // -- Statistic
    PREC GetVELO(COUNT_T _index) { assume( _index < SSCT_VELO_MAX, "Index out of bounds; index='%ld'", _index ); return this->GetStatisticPtr()->getData(_index + SSCT_VELO_FIRST); }
    void SetVELO(COUNT_T _index, PREC _value) { assume( _index < SSCT_VELO_MAX, "Index out of bounds; index='%ld'", _index ); this->GetStatisticPtr()->setData(_index + SSCT_VELO_FIRST, _value); }

    PREC GetACCEL(COUNT_T _index) { assume( _index < SSCT_ACCEL_MAX, "Index out of bounds; index='%ld'", _index ); return this->GetStatisticPtr()->getData(_index + SSCT_ACCEL_FIRST); }
    void SetACCEL(COUNT_T _index, PREC _value) { assume( _index <SSCT_ACCEL_MAX, "Index out of bounds; index='%ld'", _index ); this->GetStatisticPtr()->setData(_index + SSCT_ACCEL_FIRST, _value ); }

    // -- Spark
    void SetSpark(Spark* _spark) { this->GetSparkPtr()->setData(SPARKLIST_SPARKINDEX, _spark); }
    Spark* GetSparkBySparkID(COUNT_T _ID);

    bool IsBest();
    bool IsPlato();

    Spark* SelectOneSparkFromAll();
    ProblemShape* GetProblemShape() { return this->problemShape; }

    void SetNumberOfCalculatedTelemetrics(COUNT_T _value) { this->numberOfCalculatedTelemetrics = _value; }
    COUNT_T GetNumberOfCalculatedTelemetrics() { return this->numberOfCalculatedTelemetrics; }

    void DefaultRandomMovingProfileAroundFw();
    void InitRandomMovingProfileInArea();

    TFirework* GetFireworks();
    Firework* GetFirework(COUNT_T _fw_index);
    Firework* GetFirework() { return this->GetFirework( this->GetFwIndex() ); }

    TRecord(PREC)* GetFwParamPtr(COUNT_T _fw_index);
    TRecord(PREC)* GetFwParamPtr() { return this->GetFwParamPtr(this->GetFwIndex()); }

    void GenerateRandomParamByBase();
    void GenerateParamByBase(OffsetCalculationType _offsetCalculationType);
    void GenerateDirectedParamByBase();
    void GenerateRandomParamInArea();

    void SetParamValueOffsetCalculation(COUNT_T _index, COUNT_T _max_index, ParamOffsetCalculationFunction _param_offset_calculation);

    PREC CalculateParamRandomOffsetValue(COUNT_T _index, COUNT_T _max_index);
    PREC CalculateParamDirectedOffsetValue(COUNT_T _index, COUNT_T _max_index);
    void CalculateDirection(TRecord(PREC)* _startpoint, TRecord(PREC)* _endpoint);
    COUNT_T FindMaxDataIndex(TRecord(PREC)* _record);
    COUNT_T FindNextBiggerMovingBaseDataIndex(COUNT_T start_index, int inc);
    void CalculateMaxAmplitudeUpDownPoint(COUNT_T _index, COUNT_T _max_index, PREC* _up_point, PREC* _down_point);
    void CalculateMaxAmplitudeMutation(PREC* _max_amplitude, COUNT_T _index, COUNT_T _max_index, int _offset_direction);
    void CalculateMaxAmplitudeBorders(COUNT_T _index, COUNT_T _max_index, PREC* _up_direction, PREC* _down_direction);
    PREC CalculateMaxSimpleAmplitude(COUNT_T _index, COUNT_T _max_index, int* _offset_direction);
    PREC CalculateMaxHTCAmplitude(COUNT_T _index, COUNT_T _max_index, int* _offset_direction);

    void CalculateRealAmaxFunction(COUNT_T _HistoryIndex);

    PREC CalculateShapeFitness();
    PREC CalculateOffset(COUNT_T _index, COUNT_T _max_index, PREC _real_amplitude, PREC _boundary_offset_coef, int _offset_direction);
    void SaveAllDataToHistory();
    void SaveNamedDataToHistory();

    virtual void CalculateITL();
    void CalculateFmaxFmin();

    void SetStatsSpark(Spark* _spark) { this->stats->SetSpark(_spark); }
    void SetStats(Stats* _stats) { this->stats = _stats; }
    Stats* GetStats() { return this->stats; }

    PREC GetHCenter() { return (std::abs(this->GetHmax()) + std::abs(this->GetHmin())) / 2; }

    void SetBornType(SparkBornType _value) { this->bornType = _value; }
    SparkBornType GetBornType() { return this->bornType; }

    bool IsDying() { return (this->GetITL() < 1); }
    bool IsDied() { return ( this->GetSubState() == SSS_DIED ); }

    void Murdering() { this->SetState(SS_DIE); this->SetSubState(SSS_START); }

    void Death() { this->SetSubState(SSS_DIED); }

    virtual void BestEvaluate() {}
    virtual void InitBestEvaluate() {}
    virtual void PostCalculation();
    virtual void CallSpecialPostCalculation() {};
    virtual void CallSpecialInitPostCalculation() {};
    virtual void InitPostCalculation();

    void BornChildSpark();

    bool IsTerrainEQ() { return ( this->GetTerrainDirection() == SCT_EQ ); }
    bool IsTerrainDec() { return ( this->GetTerrainDirection() == SCT_DEC ); }
    bool IsTerrainInc() { return ( this->GetTerrainDirection() == SCT_INC ); }
    bool IsTerrainDecOrEQ() { return this->IsTerrainDec() || this->IsTerrainEQ(); }
    void CalculateEQTC();

    int GetRandomDirection();
    PREC GetUniformRandom01();
    PREC GetUniformRandom(PREC _min, PREC _max);

    PREC GetNormalRandom_0B1T2B();
    PREC GetNormalRandom_0T1B();
    PREC GetNormalRandom_1T0B();
};
