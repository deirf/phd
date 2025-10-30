#include "FunctionByValues.h"

PREC
FunctionByValues::ValueAt(PREC _x, const PREC *_fx_values)
{
  assume( (_x >= this->x_values[0]) && (_x <= this->x_values[this->N - 1]), "x is out of bounds; x='%lf', min='%lf', max='%lf'", _x, this->x_values[0], this->x_values[this->N - 1] );

  if (_x == this->x_values[0])
  {
    return _fx_values[0];
  }

  if (_x == this->x_values[this->N - 1])
  {
    return _fx_values[this->N - 1];
  }

  COUNT_T i = 1;
  while (this->x_values[i] < _x) i++;
  assume(i < this->N, "Index out of bounds; i='%ld', N='%ld'", i, this->N);

  const PREC x0 = this->x_values[i-1];
  const PREC y0 = _fx_values[i-1];
  const PREC x1 = this->x_values[i];
  const PREC y1 = _fx_values[i];

  return y0 + ((y1 - y0) * ((_x - x0) / (x1 - x0)));
}

FunctionByValues::FunctionByValues(COUNT_T _N, const PREC* _x_values)
{
  this->N = _N;
  this->x_values = _x_values;
}

FunctionByValues::~FunctionByValues()
{
}
