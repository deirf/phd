#include <cmath>
#include <algorithm>
#include "SimpleShape.h"



void
SimpleShape::Initialize(
                      COUNT_T _shapeFunction,
                      PREC _min_param,
                      PREC _max_param
                     )
{
  UNUSED( _shapeFunction );
  this->min_param = _min_param;
  this->max_param = _max_param;
  this->initialized = TRUE;
}

void
SimpleShape::SetCalculation(
                          OffsetCalculationType _offset_calculation_type,
                          PREC* _moving_base,
                          PREC* _direction,
                          PREC* _param,
                          PREC _fitness,
                          COUNT_T _param_N
                        )
{
  UNUSED(_offset_calculation_type);
  UNUSED(_moving_base);
  UNUSED(_direction);
  UNUSED(_param);
  UNUSED(_fitness);
  UNUSED(_param_N);

  DOES_NOT_REACH();
}

void
SimpleShape::SetCalculation(
                            OffsetCalculationType _offset_calculation_type,
                            PREC* _moving_base,
                            PREC* _direction,
                            PREC* _param,
                            COUNT_T _param_N
                           )
{
  UNUSED(_offset_calculation_type);

  assume(this->initialized, "Must initialize HTCShape before use it");
  this->SetMovingBase( _moving_base );
  this->SetDirection( _direction );
  this->SetParam( _param );
  this->param_N = _param_N;
}

PREC
SimpleShape::SimpleBoundaryCheck(PREC _new_pos)
{
  if ( ( _new_pos < this->GetMinParam() ) )
  {
    return std::abs( this->GetMinParam() + std::fmod( _new_pos, ( this->GetMaxParam() - this->GetMinParam() ) ) );
  }

  if ( ( _new_pos > this->GetMaxParam() ) )
  {
    return std::abs( this->GetMaxParam() - std::fmod( _new_pos, ( this->GetMaxParam() - this->GetMinParam() ) ) );
  }

  return _new_pos;
}

void
SimpleShape::SimpleSetParamValueOffsetCalculation(COUNT_T _index)
{
  const PREC offset = this->GetDirection( _index );
  const PREC new_pos = this->GetMovingBase( _index ) + offset;
  const PREC boundary_new_pos = this->SimpleBoundaryCheck( new_pos );
  assume( ( boundary_new_pos >= this->GetMinParam() ) && ( boundary_new_pos <= this->GetMaxParam() ), "New pos is out of bounds; index='%ld'new_pos='%lf'", _index, new_pos );

  this->SetParam( _index, boundary_new_pos );
}

void
SimpleShape::GenerateParamByBase()
{
  for (COUNT_T i = 0; i < this->GetParamCount(); i++)
  {
    this->SimpleSetParamValueOffsetCalculation( i );
  }
}
