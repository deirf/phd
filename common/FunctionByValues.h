#pragma once
#include "Types.h"

class FunctionByValues
{
	COUNT_T N;
	const PREC *x_values;
public:
	PREC ValueAt(PREC x, const PREC *_fx_values);
	FunctionByValues(COUNT_T _N, const PREC *_x_values);
	~FunctionByValues();
};
