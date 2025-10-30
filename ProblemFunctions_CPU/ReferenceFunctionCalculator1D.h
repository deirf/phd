#pragma once
#include "HeatConductionCalculator1DCPU.h"
#include "ProblemFunctionProcessor.h"


void calculate_reference_function(Context& _ctx, ProblemFunctionProcessor& processor, const std::string& _outputFile);
