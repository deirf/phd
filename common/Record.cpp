#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iterator>
#include "Record.h"


template class Record<PREC>;
template class Record<COUNT_T>;
template class Record<Firework*>;
template class Record<Spark*>;
template class Record<RandomSpark*>;
template class Record<GaussianSpark*>;
template class Record<QuantumSpark*>;
template class Record<FireworkSpark*>;
template class Record<BestSpark*>;
template class Record<_sparkrecorddata*>;


template <typename RecordType> bool
Record<RecordType>::isRecordReal(COUNT_T _recordindex)
{
  return (_recordindex < this->getSize());
}

template <typename RecordType> void
Record<RecordType>::checkDataExistance( COUNT_T _recordindex )
{
  assume( this->isDataExist( _recordindex ), "Data does not exist; index='%ld'", _recordindex );
}

template <typename RecordType> void
Record<RecordType>::checkRecordReal( COUNT_T _recordindex )
{
  assume( this->isRecordReal( _recordindex ), "Record does not real; index='%ld'", _recordindex );
}

template <typename RecordType> void
Record<RecordType>::checkCountLessThanSize()
{
  assume( this->getCount() < this->getSize(), "Count >= size; count='%ld', size='%ld'", this->getCount(), this->getSize() );
}

template <typename RecordType> void
Record<RecordType>::checkCountIsSize()
{
  assume( this->getCount() == this->getSize(), "Count != size; count='%ld', size='%ld'", this->getCount(), this->getSize() );
}

template <typename RecordType> uint
Record<RecordType>::getType()
{
  return typeID.at( std::type_index( typeid( RecordType ) ) );
}

template <typename RecordType>
Record<RecordType>::Record(COUNT_T _size, RecordType _defaultvalue)
{
  this->create(_size);
  for (COUNT_T i = 0; i < _size; i++)
  {
    this->record->at(i) = _defaultvalue;
  }
}

template <typename RecordType>
Record<RecordType>::Record(COUNT_T _size)
{
  this->create(_size);
}

template <typename RecordType>
Record<RecordType>::Record()
{
  this->create(0);
}

template <typename RecordType> void
Record<RecordType>::create(COUNT_T _size)
{
  this->record = std::unique_ptr<TVector(RecordType)>( new TVector(RecordType) );

  this->setSize(_size);
  this->setCount(0);
  this->oneDataSize = sizeof(RecordType);
}

template <typename RecordType>
Record<RecordType>::~Record()
{
   this->clear();
   this->record.reset();
}

template <typename RecordType> void
Record<RecordType>::free()
{
    delete this;
}

template <typename RecordType> bool
Record<RecordType>::isDataExist(COUNT_T _index)
{
  return (_index < this->getSize());
}

template <typename RecordType> void
Record<RecordType>::clear()
{
  this->record->clear();
  this->setCount(0);
}

template <typename RecordType> void
Record<RecordType>::setSize(COUNT_T _size)
{
  this->record->resize(_size);
  if (_size < this->getCount())
  {
    this->setCount(_size);
  }
}

template <typename RecordType> COUNT_T
Record<RecordType>::getSize()
{
  return this->record->size();
}

template <typename RecordType> void
Record<RecordType>::setCount(COUNT_T _count)
{
  this->count = _count;
}

template <typename RecordType> void
Record<RecordType>::incCount()
{
  this->checkCountLessThanSize();
  this->count++;
}

template <typename RecordType> void
Record<RecordType>::decCount()
{
  assume(this->getCount() > 0, "Count is 0");
  this->count--;
}

template <typename RecordType> void
Record<RecordType>::clearCount()
{
  this->count = 0;
}

template <typename RecordType> COUNT_T
Record<RecordType>::getCount()
{
  return this->count;
}

template <typename RecordType> COUNT_T
Record<RecordType>::getLastIndex()
{
  assume(this->getCount() > 0, "Count is 0");
  return this->getCount() - 1;
}

template <typename RecordType> void
Record<RecordType>::append(RecordType _data)
{
  this->checkCountIsSize();
  this->record->push_back( _data );
  this->setCount(this->getSize());
}

template <typename RecordType> RecordType
Record<RecordType>::getData(COUNT_T _index)
{
  this->checkDataExistance( _index );
  return this->record->at(_index);
}

template <typename RecordType> COUNT_T
Record<RecordType>::addData(RecordType _data)
{
  if (this->getCount() < this->getSize())
  {
    this->setData(this->getCount(), _data);
    this->incCount();
  }
  else //(this->getCount() == this->getSize())
  {
    this->append(_data);
  }

  return this->getLastIndex();
}

template <typename RecordType> bool
Record<RecordType>::delData(COUNT_T _index, RecordType _zero)
{
  this->checkDataExistance( _index );
  bool result = FALSE;

  COUNT_T last_index = this->getLastIndex();
  if (last_index > _index)
  {
    result = TRUE;
    this->setData(_index, this->getData(last_index));
  }
  this->setData(last_index, _zero);
  this->decCount();

  return result;
}

template <typename RecordType> void
Record<RecordType>::setData(COUNT_T _index, RecordType _data)
{
  this->checkDataExistance( _index );
  this->record->at(_index) = _data;
}

template <typename RecordType> void
Record<RecordType>::fill(RecordType _data)
{
  FILL(this->getRecordDataPtr(), this->getCount(), _data);
}

template <typename RecordType> RecordType
Record<RecordType>::getMax()
{
  return *std::max_element(this->record->begin(), this->record->end());
}

template <typename RecordType> RecordType*
Record<RecordType>::getRecordDataPtr()
{
  return this->record->data();
}

template <typename RecordType> void
Record<RecordType>::copy(RecordType* _dataptr, RecordCopyDirection _direction)
{
  this->copyPart( _dataptr, this->getSize(), _direction);
}

template <typename RecordType> TRecord(RecordType)*
Record<RecordType>::copy()
{
  TRecord(RecordType)* newrecord = new TRecord(RecordType)(this->getSize());
  this->copy(newrecord->getRecordDataPtr(), RCD_FROMRECORD);
  newrecord->setCount(this->getCount());
  return newrecord;
}

template <typename RecordType> void
Record<RecordType>::copyPart(RecordType* _dataptr, COUNT_T _count, RecordCopyDirection _direction)
{
  assume(!!_dataptr, "data is NULL");
  switch (_direction)
  {
    case RCD_TORECORD:
    {
      memcpy(this->getRecordDataPtr(),
             _dataptr,
             _count * this->oneDataSize);
      break;
    }

    case RCD_FROMRECORD:
    {
      memcpy(_dataptr,
             this->getRecordDataPtr(),
             _count * this->oneDataSize);
      break;
    }

    default:
    {
      DOES_NOT_REACH();
    }
  }
}

template <typename RecordType> void
Record<RecordType>::copyTo(TRecord(RecordType)* _data)
{
  assume(!!_data, "data is NULL");
  this->copy(_data->getRecordDataPtr(), RCD_FROMRECORD);
  _data->setCount(this->getCount());
}

template <typename RecordType> void
Record<RecordType>::copyFrom(TRecord(RecordType)* _data)
{
  assume(!!_data, "data is NULL");
  this->copy(_data->getRecordDataPtr(), RCD_TORECORD);
  this->setCount(_data->getCount());
}

template <typename RecordType> void
Record<RecordType>::copyTo(RecordType* _data, COUNT_T _N)
{
  assume(this->getCount() < _N, "Count > N; count='%ld', N='%ld'", this->getCount(), _N );
  assume(!!_data, "data is NULL");
  this->copyPart( _data, _N, RCD_FROMRECORD);
}

template <typename RecordType> void
Record<RecordType>::copyFrom(RecordType* _data, COUNT_T _N)
{
  assume(this->getSize() >= _N, "Size < N; size='%ld', N='%ld'", this->getSize(), _N );
  assume(!!_data, "data is NULL");
  this->copyPart( _data, _N, RCD_TORECORD);
  this->setCount(_N);
}
