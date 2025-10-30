#include "Array.h"

template class Array<PREC>;
template class Array<COUNT_T>;
template class Array<Firework*>;
template class Array<Spark*>;
template class Array<RandomSpark*>;
template class Array<GaussianSpark*>;
template class Array<QuantumSpark*>;
template class Array<FireworkSpark*>;
template class Array<BestSpark*>;
template class Array<_sparkrecorddata*>;

template <typename ArrayType> uint
Array<ArrayType>::getType()
{
  return typeID.at( std::type_index( typeid( ArrayType ) ) );
}

template <typename ArrayType>
Array<ArrayType>::Array(COUNT_T _recordsize, COUNT_T _numberOfRecords, ArrayType _defaultvalue)
{
  this->create(_numberOfRecords, _recordsize, _defaultvalue);
}

template <typename ArrayType>
Array<ArrayType>::Array(COUNT_T _recordsize, COUNT_T _numberOfRecords)
{
  this->create(_numberOfRecords, _recordsize, 0);
}

template <typename ArrayType>
Array<ArrayType>::Array(COUNT_T _numberOfRecords)
{
  this->create(_numberOfRecords, 0, 0);
}

template <typename ArrayType>
Array<ArrayType>::Array()
{
  this->create(0, 0, 0);
}

template <typename ArrayType> void
Array<ArrayType>::create(COUNT_T _numberOfRecords, COUNT_T _recordsize, ArrayType _defaultvalue)
{

  this->array = std::unique_ptr<TVArray(ArrayType)>( new TVArray(ArrayType)(_numberOfRecords) );
  this->setCount(_numberOfRecords);
  this->oneDataSize = sizeof(ArrayType);
  if (_recordsize > 0)
  {
    for (COUNT_T _recordindex = 0; _recordindex < _numberOfRecords; _recordindex++)
    {
      this->array->at(_recordindex) = this->createRecord(_recordsize, _defaultvalue);
    }
  }
}

template <typename ArrayType> bool
Array<ArrayType>::isRecordExist(COUNT_T _recordindex)
{
  return (_recordindex < this->getCount());
}

template <typename ArrayType> bool
Array<ArrayType>::isRecordValid(COUNT_T _recordindex)
{
  return (!!this->getRecordPtr(_recordindex));
}

template <typename ArrayType> bool
Array<ArrayType>::isRecordReal(COUNT_T _recordindex)
{
  return (_recordindex < this->getSize());
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::getSize()
{
  return this->array->size();
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::getCount()
{
  return this->count;
}

template <typename ArrayType> void
Array<ArrayType>::setCount(COUNT_T _count)
{
  this->count = _count;
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::getLastIndex()
{
  return this->getCount() - 1;
}

template <typename ArrayType> void
Array<ArrayType>::incCount()
{
  assume( this->getCount() < this->getSize(), "Count >= size" );
  this->count++;
}

template <typename ArrayType> void
Array<ArrayType>::decCount()
{
  assume( this->getCount() > 0, "Count = 0" );
  this->count--;
}

template <typename ArrayType> void
Array<ArrayType>::checkRecordExistance( COUNT_T _recordindex )
{
  assume( this->isRecordExist( _recordindex ), "Record does not exist; index='%ld'", _recordindex );
}

template <typename ArrayType> void
Array<ArrayType>::checkRecordReal( COUNT_T _recordindex )
{
  assume( this->isRecordReal( _recordindex ), "Record does not real; index='%ld'", _recordindex );
}

template <typename ArrayType> void
Array<ArrayType>::checkRecordValidity( COUNT_T _recordindex )
{
  assume( this->isRecordValid( _recordindex ), "Record does not valid; index='%ld'", _recordindex );
}

template <typename ArrayType> void
Array<ArrayType>::checkCountIsSize()
{
  assume( this->getCount() == this->getSize(), "Count != size; count='%ld', size='%ld'", this->getCount(), this->getSize() );
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::getRecordCount(COUNT_T _recordindex)
{
  this->checkRecordExistance( _recordindex );
  this->checkRecordValidity( _recordindex );
  return this->getRecordPtr(_recordindex)->getCount();
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::getRecordSize(COUNT_T _recordindex)
{
  this->checkRecordExistance( _recordindex );
  this->checkRecordValidity( _recordindex );
  return this->getRecordPtr(_recordindex)->getSize();
}

template <typename ArrayType> TRecord(ArrayType)*
Array<ArrayType>::getRecordPtr(COUNT_T _recordindex)
{
  this->checkRecordReal( _recordindex );
  return this->array->at(_recordindex);
}

template <typename ArrayType> ArrayType*
Array<ArrayType>::getRecordDataPtr(COUNT_T _recordindex)
{
  this->checkRecordExistance( _recordindex );
  this->checkRecordValidity( _recordindex );
  return this->getRecordPtr(_recordindex)->getRecordDataPtr();
}

template <typename ArrayType> void
Array<ArrayType>::setRecordPtr(TRecord(ArrayType)* _record, COUNT_T _recordindex)
{
  this->checkRecordReal( _recordindex );
  this->array->at(_recordindex) = _record;
}

template <typename ArrayType> void
Array<ArrayType>::setData(COUNT_T _recordindex, COUNT_T _index, ArrayType _newdata)
{
  this->checkRecordExistance( _recordindex );
  this->checkRecordValidity( _recordindex );
  this->getRecordPtr(_recordindex)->setData(_index, _newdata);
}

template <typename ArrayType> ArrayType
Array<ArrayType>::getData(COUNT_T _recordindex, COUNT_T _index)
{
  this->checkRecordExistance( _recordindex );
  this->checkRecordValidity( _recordindex );
  return this->getRecordPtr(_recordindex)->getData(_index);
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::addData(COUNT_T _recordindex, COUNT_T _index, ArrayType _data)
{
  UNUSED( _index );
  this->checkRecordExistance( _recordindex );
  this->checkRecordValidity( _recordindex );
  return this->getRecordPtr(_recordindex)->addData(_data);
}

template <typename ArrayType> bool
Array<ArrayType>::delData(COUNT_T _recordindex, COUNT_T _index, ArrayType _zero)
{
  this->checkRecordExistance( _recordindex );
  this->checkRecordValidity( _recordindex );
  return this->getRecordPtr(_recordindex)->delData(_index, _zero);
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::appendRecord(Record<ArrayType>* _record)
{
  this->checkCountIsSize();
  this->array->push_back( std::move( _record ) );
  this->setCount(this->getSize());
  return this->getLastIndex();
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::addRecordInternal(Record<ArrayType>* _record)
{
  if (this->getCount() < this->getSize())
  {
    this->setRecordPtr(_record, this->getCount());
    this->incCount();
  }
  else
  {
    this->checkCountIsSize();
    this->appendRecord(_record);
  }

  return this->getLastIndex();
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::addRecord(Record<ArrayType>* _record)
{
  return this->addRecordInternal(_record);
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::addRecord(COUNT_T _size)
{
  return this->addRecordInternal(this->createRecord(_size, 0));
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::addRecord()
{
  return this->addRecord((COUNT_T)0);
}

template <typename ArrayType> bool
Array<ArrayType>::delRecord(COUNT_T _recordindex)
{
  this->checkRecordExistance( _recordindex );

  COUNT_T last_index = this->getLastIndex();
  if (this->isRecordValid(_recordindex))
  {
    this->getRecordPtr(_recordindex)->free();
  }

  bool result = FALSE;
  if (last_index > _recordindex)
  {
    result = TRUE;
    this->setRecordPtr(this->getRecordPtr(last_index), _recordindex);
  }

  this->setRecordPtr(NULL, last_index);
  this->decCount();

  return result;
}

template <typename ArrayType> Record<ArrayType>*
Array<ArrayType>::createRecord(COUNT_T _size, ArrayType _value)
{
  return new Record<ArrayType>(_size, _value);
}

template <typename ArrayType> void
Array<ArrayType>::copyData(ArrayType* _dataptr, COUNT_T _recordindex, ArrayCopyDirection _direction)
{
  this->checkRecordExistance( _recordindex );
  this->checkRecordValidity( _recordindex );
  switch (_direction)
  {
    case ACD_TOARRAY:
    {
      this->getRecordPtr( _recordindex )->copyPart( _dataptr,
                                                    this->getRecordSize(_recordindex),
                                                    RCD_TORECORD
                                                  );
      break;
    }
    case ACD_FROMARRAY:
    {
      this->getRecordPtr( _recordindex )->copyPart( _dataptr,
                                                    this->getRecordSize(_recordindex),
                                                    RCD_FROMRECORD
                                                  );
      break;
    }
    default:
    {
      DOES_NOT_REACH();
    }
  }
}

template <typename ArrayType> COUNT_T
Array<ArrayType>::copyAllData(ArrayType* _dataptr, ArrayCopyDirection _direction)
{
  COUNT_T pos = 0;
  for(COUNT_T recordindex = 0; recordindex < this->getCount(); recordindex++ )
  {
    this->copyData(_dataptr + pos, recordindex, _direction);
    pos = pos + this->getRecordSize(recordindex);
  }
  return pos;
}

template <typename ArrayType> Array<ArrayType>*
Array<ArrayType>::copyArray()
{
  Array<ArrayType>* result = new Array<ArrayType>();
  for (COUNT_T recordindex = 0; recordindex < (COUNT_T)this->getCount(); ++recordindex)
  {
    Record<ArrayType>* record = new Record<ArrayType>(this->getRecordSize(recordindex));
    this->copyData(record->getRecordDataPtr(), recordindex, ACD_FROMARRAY);
    record->setCount(this->getRecordPtr(recordindex)->getCount());
    result->addRecordInternal(record);
  }

  result->setCount(this->getCount());
  return result;
}

template <typename ArrayType> void
Array<ArrayType>::clear()
{
  while ( this->getCount() != 0 )
  {
    this->delRecord(this->getCount() - 1);
  }

  this->array->clear();
  this->setCount(0);
}

template <typename ArrayType>
Array<ArrayType>::~Array()
{
  this->clear();
  this->array.reset();
}

template <typename ArrayType> void
Array<ArrayType>::free()
{
  delete this;
}
