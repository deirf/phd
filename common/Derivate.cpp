#include "Derivate.h"


template class Derivate<PREC>;

template <typename DataType>
Derivate<DataType>::~Derivate()
  {
    if ( this->result )
    {
      delete this->result;
    }
  }

template <typename DataType>
Derivate<DataType>::Derivate( const COUNT_T _N )
{
  this->result = new TRecord(DataType)( _N );
}

template <typename DataType> void
Derivate<DataType>::SetResult( TRecord(DataType)* _result )
{
  this->result = _result;
}

template <typename DataType> TRecord(DataType)*
Derivate<DataType>::GetResult()
{
  return this->result;
}

template <typename DataType> TRecord(DataType)*
Derivate<DataType>::GetResultRelease()
{
  return GetResultSet( NULL );
}

template <typename DataType> TRecord(DataType)*
Derivate<DataType>::GetResultRenew()
{
  return GetResultSet( new TRecord(DataType)( this->result->getCount() ) );
}

template <typename DataType> TRecord(DataType)*
Derivate<DataType>::GetResultSet( TRecord(DataType)* _new_result )
{
  TRecord(DataType)* new_result = _new_result;
  SWAP( this->result, new_result );
  return new_result;
}

template <typename DataType> DataType
Derivate<DataType>::CalculateMidPointDerivate( COUNT_T _index )
{
  const DataType const_1 = 8;
  const DataType const_2 = 1;
  const DataType const_3 = 12 * this->h;

  const DataType result = (
                             const_1 * ( this->input->getData( _index + 1 ) - this->input->getData( _index - 1 ) )
                           +
                             const_2 * ( this->input->getData( _index - 2 ) - this->input->getData( _index + 2 ) )
                          )
                           / const_3;

  return result;
}

// First point; Two‐point forward difference formula: ( f(xn+1) - f(xn) ) / t
template <typename DataType> DataType
Derivate<DataType>::CalculateFirstPointDerivate( COUNT_T _index )
{
  return ( this->input->getData( _index + 1 ) - this->input->getData( _index ) ) / this->h;
}

// Last point; Two‐point backward difference formula: ( f(xn) - f(xn-1) ) / t
template <typename DataType> DataType
Derivate<DataType>::CalculateLastPointDerivate( COUNT_T _index )
{
  return ( this->input->getData( _index ) - this->input->getData( _index - 1 ) ) / this->h;
}

// Second and befor the last point: Two‐point central difference formula: ( f(xn+1) - f(xn-1) ) / 2t
template <typename DataType> DataType
Derivate<DataType>::CalculateSecondPointDerivate( COUNT_T _index )
{
  return ( this->input->getData( _index + 1 ) - this->input->getData( _index - 1 ) ) / this->h2;
}

// Second and befor the last point: Two‐point central difference formula: ( f(xn+1) - f(xn-1) ) / 2t
template <typename DataType> DataType
Derivate<DataType>::CalculateBeforeLastPointDerivate( COUNT_T _index )
{
  return ( this->input->getData( _index + 1 ) - this->input->getData( _index - 1 ) ) / this->h2;
}

template <typename DataType> void
Derivate<DataType>::Calculate( TRecord(DataType)* _input, DataType _h )
{
  this->input = _input;
  this->h = _h;
  this->h2 = 2 * _h;

  assume( this->input->getSize() <= this->result->getSize(), "Input and result size is not same; input='%ld', result='%ld'", this->input->getSize(), this->result->getSize());
  assume( this->input->getCount() > 2, "Input count less than 2; 'count='%ld'", this->input->getCount() );

  this->result->setData( 0, this->CalculateFirstPointDerivate( 0 ) );
  this->result->setData( 1, this->CalculateSecondPointDerivate( 1 ) );

  for (COUNT_T index = 2; index < this->input->getCount() - 2; index++)
  {
    this->result->setData( index, this->CalculateMidPointDerivate( index ) );
  }

  this->result->setData( this->input->getCount() - 2, this->CalculateBeforeLastPointDerivate( this->input->getCount() - 2 ) );
  this->result->setData( this->input->getCount() - 1, this->CalculateLastPointDerivate( this->input->getCount() - 1) );
  this->result->setCount( this->input->getCount() );
}
