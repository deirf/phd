#pragma once

#include "Record.h"
#include "Types.h"

// First point; Two‐point forward difference formula: ( f(xn+l) - f(xn) ) / tl
// Last point; Two‐point backward difference formula: ( f(xn) - f(xn-l) ) / tl
// Second and before the last point: Two‐point central difference formula: ( f(xn+l) - f(xn-l) ) / 2tl
// Mid Point: Five‐point central difference formula: (8 * (f(xn+l) - f(xn-l))  -  (f(xn+2l) - f(xn-2l))) / 12tl

template <typename DataType>
class Derivate
{
private:
  TRecord(DataType)* result = NULL;
  TRecord(DataType)* input = NULL;
  DataType h = 0;
  DataType h2 = 0;

  DataType CalculateMidPointDerivate( COUNT_T _index );
  DataType CalculateFirstPointDerivate( COUNT_T _index );
  DataType CalculateLastPointDerivate( COUNT_T _index );
  DataType CalculateBeforeLastPointDerivate( COUNT_T _index );
  DataType CalculateSecondPointDerivate( COUNT_T _index );

public:
  explicit Derivate( const COUNT_T _N );
  ~Derivate();

  void Calculate( TRecord(DataType)* _input, DataType _h );

  void SetResult( TRecord(DataType)* _result );
  TRecord(DataType)* GetResult();
  TRecord(DataType)* GetResultRelease();
  TRecord(DataType)* GetResultRenew();
  TRecord(DataType)* GetResultSet( TRecord(DataType)* _new_result );
};

