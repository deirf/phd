#include "AFwa.h"
#include "oSpark.h"

void
AFwa::GenerateAmplitude()
{
  PREC amplitude = this->max_param - this->min_param;

  const COUNT_T fwaIndex = 0;
  for (COUNT_T i = 0; i < this->spark_number; i++)
  {
    const PREC bestDistance = this->CalculateDistance( this->sparks[i]->GetParams(), this->GetBestParams() );
    if (
         ( bestDistance > amplitude ) &&
         ( this->sparks[ i ]->GetFitness() > this->sparks[ fwaIndex ]->GetFitness() )
       )
    {
      amplitude = bestDistance;
    }

    amplitude = 0.5f * ( this->sparks[i]->GetAmplitude() + lambda * amplitude );
    assume(this->finite(amplitude), "Ampliture is infinite");
    this->sparks[i]->SetAmplitude(amplitude);
  }
}
