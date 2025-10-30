#include <cmath>
#include <algorithm>
#include "HTCShape.h"



void
HTCShape::Initialize(
                      COUNT_T _shapeFunction,
                      PREC _min_param,
                      PREC _max_param
                     )
{
  switch ( _shapeFunction )
  {
    case SF_HTC:
    {
      this->maxAmplitudeFunction = &HTCShape::CalculateMaxHTCAmplitude;
      break;
    }
    case SF_SIMPLE:
    {
      this->maxAmplitudeFunction = &HTCShape::CalculateMaxSimpleAmplitude;
      break;
    }
    default:
    {
      DOES_NOT_REACH();
    }
  }

  this->min_param = _min_param;
  this->max_param = _max_param;
  this->initialized = TRUE;
}

void
HTCShape::SetCalculation(
                          OffsetCalculationType _offset_calculation_type,
                          PREC* _moving_base,
                          PREC* _direction,
                          PREC* _param,
                          PREC _fitness,
                          COUNT_T _param_N
                        )
{
  this->SetFitness(_fitness);
  this->SetCalculation(
                       _offset_calculation_type,
                       _moving_base,
                       _direction,
                       _param,
                       _param_N
                      );
}

void
HTCShape::SetCalculation(
                          OffsetCalculationType _offset_calculation_type,
                          PREC* _moving_base,
                          PREC* _direction,
                          PREC* _param,
                          COUNT_T _param_N
                        )
{
  assume(this->initialized, "Must initialize HTCShape before use it");
  this->SetMovingBase( _moving_base );
  this->SetDirection( _direction );
  this->SetParam( _param );
  this->param_N = _param_N;

  switch (_offset_calculation_type)
  {
    case OCT_DIRECTED_RANDOM:
    {
      this->paramOffsetFunction = &HTCShape::CalculateParamDirectedRandomOffsetValue;
      break;
    }
    case OCT_AMAX_RANDOM:
    {
      this->paramOffsetFunction = &HTCShape::CalculateParamAmaxRandomOffsetValue;
      break;
    }
    case OCT_DIRECTED:
    {
      this->paramOffsetFunction = &HTCShape::CalculateParamDirectedOffsetValue;
      break;
    }
    case OCT_SHAPE:
    {
      this->paramOffsetFunction = NULL;
      break;
    }
    default:
    {
      DOES_NOT_REACH();
    }
  }
}

COUNT_T
HTCShape::FindMaxDataIndex()
{
  bool changed = FALSE;
  PREC max = - PREC_MAX;
  COUNT_T max_index = 0;
  for (COUNT_T index = 0; index < this->GetParamCount(); index++)
  {
    const PREC data_i = this->GetParam( index );
    if (data_i > max)
    {
      max = data_i;
      max_index = index;
      changed = TRUE;
    }
  }

  assume(changed, "Max data index is not set");
  return max_index;
}

COUNT_T
HTCShape::FindNextBiggerMovingBaseDataIndex(COUNT_T _start_index, int _inc)
{
  const COUNT_T count = this->GetParamCount();
  assume( _start_index < INT_MAX, "Start index is bigger than INT_MAX");
  assume( count < INT_MAX, "Count is bigger than INT_MAX" );
  const long int int_count = (long int)count;

  long int index = (long int)_start_index + _inc;
  while (
          ( index > -1) &&
          ( index <  int_count ) &&
          ( this->GetMovingBase( _start_index ) > this->GetMovingBase( (COUNT_T)index ) )
        )
  {
    index = index + _inc;
  }

  return ( ( index > (long int)count - 1 ) || ( index < 0 ) ) ? _start_index : (COUNT_T)index;
}

COUNT_T
HTCShape::FindNextSmallerMovingBaseDataIndex(COUNT_T _start_index, int _inc)
{
  const COUNT_T count = this->GetParamCount();
  assume( _start_index < INT_MAX, "Start index is bigger than INT_MAX");
  assume( count < INT_MAX, "Count is bigger than INT_MAX" );
  const long int int_count = (long int)count;

  long int index = (long int)_start_index + _inc;
  while (
          ( index > -1) &&
          ( index <  int_count ) &&
          ( this->GetMovingBase( _start_index ) < this->GetMovingBase( (COUNT_T)index ) )
        )
  {
    index = index + _inc;
  }

  return ( ( index > (long int)count - 1 ) || ( index < 0 ) ) ? _start_index : (COUNT_T)index;
}

void
HTCShape::CalculateMaxAmplitudeUpDownPoint(COUNT_T _index,
                                         COUNT_T _max_index,
                                         PREC* restrict _up_point,
                                         PREC* restrict _down_point
                                        )
{
  if ( _index == 0 ) // left bottom
  {
    const COUNT_T bigger_data_index = FindNextBiggerMovingBaseDataIndex( _index, 1 );
    *_down_point = this->GetMinParam();
    *_up_point = this->GetMovingBase( bigger_data_index );
  }
  else if ( _index < _max_index ) // left
  {
    const COUNT_T bigger_data_index = FindNextBiggerMovingBaseDataIndex( _index, 1 );
    const COUNT_T bigger_data_index_m1 = FindNextBiggerMovingBaseDataIndex( _index - 1, 1 );
    *_down_point = this->GetMovingBase( _index - 1 );
    *_up_point = std::max( this->GetMovingBase( bigger_data_index ), this->GetMovingBase( bigger_data_index_m1 ) );

    if ( *_up_point < *_down_point )
    {
      *_up_point = this->GetMaxParam();
    }

  }
  else if ( _index == this->GetParamCount() - 1 ) //right bottom
  {
    const COUNT_T bigger_data_index = FindNextBiggerMovingBaseDataIndex( _index, -1 );
    // *_down_point = this->GetMovingBase( _index - 1 ) / 2; // this->GetMinParam();
    *_down_point = this->GetMinParam();
    *_up_point = this->GetMovingBase( bigger_data_index );
  }
  else if ( _index > _max_index ) // right
  {
    const COUNT_T bigger_data_index = FindNextBiggerMovingBaseDataIndex( _index, -1 );
    const COUNT_T bigger_data_index_p1 = FindNextBiggerMovingBaseDataIndex( _index + 1, -1 );
    *_down_point = this->GetMovingBase( _index + 1 );
    *_up_point = std::max( this->GetMovingBase( bigger_data_index ), this->GetMovingBase( bigger_data_index_p1 ) );

    if ( *_up_point < *_down_point )
    {
      *_up_point = this->GetMaxParam();
    }
  }
  else if ( _index == _max_index ) // peak
  {
    const COUNT_T left_smaller_data_index = FindNextSmallerMovingBaseDataIndex( _index, -1 );
    const COUNT_T right_smaller_data_index = FindNextSmallerMovingBaseDataIndex( _index, 1 );
    const COUNT_T max_smaller_data_index = std::max(left_smaller_data_index, right_smaller_data_index);
    *_down_point = this->GetMovingBase( max_smaller_data_index );
    *_up_point = this->GetMaxParam();
  }
  else
  {
    DOES_NOT_REACH();
  }

  assume( *_down_point <= *_up_point, "wrong max aplitude calculation" );
}

void
HTCShape::CalculateMaxAmplitudeBorders(COUNT_T _index, COUNT_T _max_index, PREC* restrict _up_direction, PREC* restrict _down_direction)
{
  PREC up_point = -1;    // in y direction
  PREC down_point = -1;  // in y direction
  this->CalculateMaxAmplitudeUpDownPoint( _index, _max_index, &up_point, &down_point );

  *_up_direction = up_point - this->GetMovingBase( _index );     //in y direction
  *_down_direction = this->GetMovingBase( _index ) - down_point; //in y direction
}

PREC
HTCShape::CalculateMaxHTCAmplitude(COUNT_T _index, COUNT_T _max_index, int* _offset_direction)
{
  assume( *_offset_direction == 1 || *_offset_direction == -1, "Offset direction is not direction; value='%d'", *_offset_direction );

  PREC up_distance = 0;
  PREC down_distance = 0;
  PREC max_amplitude = 0;
  this->CalculateMaxAmplitudeBorders(_index, _max_index, &up_distance, &down_distance);

  if ( *_offset_direction == 1 )
  {
    if ( up_distance > 0 )
    {
      max_amplitude = up_distance;
    }
    else if ( up_distance <= 0 )
    {
      *_offset_direction = -1;
      max_amplitude = down_distance;
    }
    else
    {
      DOES_NOT_REACH();
    }
  }

  if ( *_offset_direction == -1 )
  {
    if ( down_distance > 0 )
    {
      max_amplitude = down_distance;
    }
    else if ( down_distance <= 0 )
    {
      *_offset_direction = 1;
      max_amplitude = up_distance;
    }
    else
    {
      DOES_NOT_REACH();
    }
  }

  if ( max_amplitude <= 0 )
  {
    return this->CalculateMaxSimpleAmplitude( _index, _max_index, _offset_direction);
  }

  assume(max_amplitude >= 0, "Max amplitude is negative");

  return max_amplitude;
}

PREC
HTCShape::CalculateMaxSimpleAmplitude(COUNT_T _index, COUNT_T _max_index, int* restrict _offset_direction)
{
  UNUSED( _max_index );
  const PREC pos = this->GetMovingBase( _index );
  return *_offset_direction == 1 ? this->GetMaxParam() - pos : pos - this->GetMinParam();
}

PREC
HTCShape::CalculateOffset(COUNT_T _index, COUNT_T _max_index, PREC _real_amplitude, PREC _boundary_offset_coef, int _offset_direction)
{
  assume( _real_amplitude > 0, "Real amplitude is not positive");
  assume(this->maxAmplitudeFunction != NULL, "MaxAmplitude function is not set");

  const PREC max_amplitude = (this->*maxAmplitudeFunction)(_index, _max_index, &_offset_direction);
  const PREC boundary_amplitude = max_amplitude * _boundary_offset_coef;

  return ( _real_amplitude <= max_amplitude ? _real_amplitude : boundary_amplitude ) * (PREC)_offset_direction;
}

void
HTCShape::BoundaryCheck(PREC _base, PREC* _amplitude, int* _direction)
{
  if ( this->GetFitness() < 20 )
  {
    return;
  }

  const PREC new_pos = _base + *_amplitude * *_direction;

  if ( ( new_pos >= this->GetMinParam() ) && ( new_pos <= this->GetMaxParam() ) )
  {
    return;
  }

  const PREC offset_coef = this->GetNormalRandom_1T0B();
  PREC calc_pos = new_pos;
  if ( ( new_pos < this->GetMinParam() ) )
  {
    calc_pos = this->GetMinParam() + ( this->GetMaxParam() - this->GetMinParam() ) * offset_coef;
  }

  if ( ( new_pos > this->GetMaxParam() ) )
  {
    calc_pos = this->GetMaxParam() - ( this->GetMaxParam() - this->GetMinParam() ) * offset_coef;
  }

  *_amplitude = ABS( calc_pos - _base );
  *_direction = SIGN( calc_pos - _base );

  assume( *_amplitude > 0, "amplitude is not positive; amplitude: %lf", *_amplitude);
  assume( *_direction != 0, "direction is zero");
}

PREC
HTCShape::CalculateParamDirectedRandomOffsetValue(COUNT_T _index, COUNT_T _max_index)
{
  const PREC offset_coef = this->GetNormalRandom_0B1T2B();
  const PREC real_offset = this->GetDirection( _index );
  int offset_direction = SIGN(this->GetDirection( _index ));

  if ( offset_direction == 0)
  {
    return 0;
  }

  PREC real_amplitude = ABS(real_offset * offset_coef) + EPSILON;

  this->BoundaryCheck(this->GetMovingBase( _index ), &real_amplitude, &offset_direction);

  const PREC boundary_offset_coef = this->GetNormalRandom_1T0B();
  return this->CalculateOffset(_index, _max_index, real_amplitude, boundary_offset_coef, offset_direction);
}

PREC
HTCShape::CalculateParamAmaxRandomOffsetValue(COUNT_T _index, COUNT_T _max_index)
{
  const PREC offset_coef = this->GetUniformRandom01();
  int offset_direction = this->GetRandomDirection();
  PREC real_amplitude = this->GetDirection( _index ) * offset_coef + EPSILON; //Amax

  if ( offset_direction == 0)
  {
    return 0;
  }

  this->BoundaryCheck(this->GetMovingBase( _index ), &real_amplitude, &offset_direction);

  const PREC boundary_offset_coef = this->GetNormalRandom_1T0B();
  return this->CalculateOffset(_index, _max_index, real_amplitude, boundary_offset_coef, offset_direction);
}

PREC
HTCShape::CalculateParamDirectedOffsetValue(COUNT_T _index, COUNT_T _max_index)
{
  PREC real_amplitude = ABS(this->GetDirection( _index )) + EPSILON;
  int offset_direction = SIGN(this->GetDirection( _index ));

  if ( offset_direction == 0)
  {
    return 0;
  }

  this->BoundaryCheck(this->GetMovingBase( _index ), &real_amplitude, &offset_direction);

  const PREC boundary_offset_coef = this->GetNormalRandom_1T0B();
  return this->CalculateOffset(_index, _max_index, real_amplitude, boundary_offset_coef, offset_direction);
}

void
HTCShape::SetParamValueOffsetCalculation(COUNT_T _index, COUNT_T _max_index)
{
  assume(this->paramOffsetFunction != NULL, "paramOffsetFunction function is not set");
  const PREC offset = (this->*paramOffsetFunction)(_index, _max_index);

  const PREC new_pos = this->GetMovingBase( _index ) + offset;
  assume( ( new_pos >= this->GetMinParam() ) && ( new_pos <= this->GetMaxParam() ), "New pos is out of bounds; index='%ld'; new_pos='%lf'", _index, new_pos );

  this->SetParam( _index, new_pos );
}

void
HTCShape::GenerateParamByBase()
{
  const COUNT_T max_index = this->FindMaxDataIndex();

  for (COUNT_T index = this->GetParamCount() - 1; index > max_index; --index)
  {
    this->SetParamValueOffsetCalculation(index, max_index);
  }
  const COUNT_T right_max_index = this->FindMaxDataIndex();

  for (COUNT_T index = 0; index < right_max_index; ++index)
  {
    this->SetParamValueOffsetCalculation(index, right_max_index);
  }
  const COUNT_T left_max_index = this->FindMaxDataIndex();

  if ( left_max_index > right_max_index )
  {
    for (COUNT_T index = right_max_index; index < left_max_index; --index)
    {
      this->SetParamValueOffsetCalculation(index, right_max_index);
    }
  }
  else if ( right_max_index > left_max_index)
  {
    for (COUNT_T index = left_max_index; index < right_max_index; ++index)
    {
      this->SetParamValueOffsetCalculation(index, right_max_index);
    }
  }
  else
  {
    this->SetParamValueOffsetCalculation(left_max_index, right_max_index);
  }
}

PREC
HTCShape::CalculateShapeFitness()
{
  const COUNT_T max_index = this->FindMaxDataIndex();
  const COUNT_T max_count = this->GetParamCount();

  PREC fitness = 0;

  fitness += ( this->GetParam( 0 ) > this->GetParam( 1 ) ) ? 1 : 0;
  fitness += ( this->GetParam( max_count - 1 ) > this->GetParam( max_count - 2 ) ) ? 1 : 0;

  for ( COUNT_T i = 1; i < max_index; i++ )
  {
    fitness += ( this->GetParam( i ) > this->GetParam( i + 1 ) ) ? 1 : 0;
  }

  for ( COUNT_T i = max_index; i < max_count - 1 ; i++ )
  {
    fitness += ( this->GetParam( i ) < this->GetParam( i + 1 ) ) ? 1 : 0;
  }

 return fitness;
}

