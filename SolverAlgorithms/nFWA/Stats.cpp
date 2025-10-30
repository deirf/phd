#include <cmath>
#include <iterator>
#include "Spark.h"
#include "../../common/Types.h"
#include "Stats.h"


PREC
Stats::AlmostZero(PREC _value)
{
  return ( std::abs(_value) > EPSILON_ALMOST_ZERO_LIMIT ? _value : 0 );
}

COUNT_T
Stats::GetDominantSpeed(COUNT_T _velo1, COUNT_T _velo2, COUNT_T _accel)
{
  COUNT_T velo_size = this->GetSpark()->GetNumberOfCalculatedTelemetrics();

  if (velo_size > 1)
  {
    return this->dominant_speed[   SIGN(
                                      std::abs(this->GetSpark()->GetVELO(_velo1)) -
                                      std::abs(this->GetSpark()->GetVELO(_velo2))
                                       )
                                     *
                                 - SIGN(
                                      this->GetSpark()->GetACCEL(_accel)
                                       )
                                 + 1
                               ];
  }

  return SDOM_NOT_CALCULATED;
}

void
Stats::CalculateTelemetrics()
{
  COUNT_T history_size = this->GetSpark()->GetHistory()->GetUnnamedHistorySize();
  COUNT_T velo_index = SSIV_VELO_FIRST;

  //HS:  1 2 3 4 5 6
  //H: R 2 3 4 5 6 7
  //V:  0 1 2 3 4 5
  //A:   0 1 2 3 4

  if (history_size > 0)
  {
    PREC velo = this->GetSpark()->GetFitness() -
                this->GetSpark()->GetHistory()->GetFitness(SH_FIRST);
    this->GetSpark()->SetVELO(velo_index, this->AlmostZero(velo));
    velo_index++;
  }

  if (history_size > 1)
  {
    const COUNT_T MAX_HISTORY = SH_FIRST + history_size;
    for ( COUNT_T history_index = SH_FIRST; history_index < MAX_HISTORY - 1; ++history_index)
    {
      PREC velo = this->GetSpark()->GetHistory()->GetFitness(history_index) -
                  this->GetSpark()->GetHistory()->GetFitness(history_index + 1);
      this->GetSpark()->SetVELO(velo_index, this->AlmostZero(velo));
      velo_index++;
    }


    const COUNT_T MAX_VELO = history_size;
    COUNT_T accel_index = SSIA_ACCEL_FIRST;

    assume(SSIV_VELO_FIRST < MAX_VELO - 1, "VELO calculation error");
    for ( velo_index = SSIV_VELO_FIRST; velo_index < MAX_VELO - 1; ++velo_index)
    {
      PREC accel = this->GetSpark()->GetVELO(velo_index + 1) -
                   this->GetSpark()->GetVELO(velo_index);
      this->GetSpark()->SetACCEL(accel_index, this->AlmostZero(accel));
      accel_index++;
    }
  }

  this->GetSpark()->SetNumberOfCalculatedTelemetrics(velo_index);
}

COUNT_T
Stats::GetTelemetric(COUNT_T _velo1, COUNT_T _velo2, COUNT_T _accel)
{
  COUNT_T result = STMS_UNKNOWN;
  COUNT_T velo_size = this->GetSpark()->GetNumberOfCalculatedTelemetrics();
  if (velo_size > 1)
  {
    result = this->telemetrics[ SIGN(this->GetSpark()->GetVELO(_velo1)) + 1 ]
                              [ SIGN(this->GetSpark()->GetVELO(_velo2)) + 1 ]
                              [ SIGN(this->GetSpark()->GetACCEL(_accel)) + 1 ];
  }
  else if (velo_size == 1)
  {
    result = this->light_telemetrics[ SIGN(this->GetSpark()->GetVELO(_velo1)) + 1 ];
  }
  else
  {
    result = STMS_NOT_CALCULATED;
  }

  assume(result != STMS_UNKNOWN, "result is UNKNOWN");

  return result;
}

COUNT_T
Stats::GetTerrain(COUNT_T _velo1, COUNT_T _velo2, COUNT_T _accel)
{
  COUNT_T result = STMD_UNKNOWN;
  COUNT_T velo_size = this->GetSpark()->GetNumberOfCalculatedTelemetrics();
  if (velo_size > 1)
  {
    result = this->terrians[ SIGN(this->GetSpark()->GetVELO(_velo1)) + 1 ]
                           [ SIGN(this->GetSpark()->GetVELO(_velo2)) + 1 ]
                           [ SIGN(this->GetSpark()->GetACCEL(_accel)) + 1 ];
  }
  else if (velo_size == 1)
  {
    result = this->light_terrians[ SIGN(this->GetSpark()->GetVELO(_velo1)) + 1 ];
  }
  else
  {
    result = STMD_NOT_CALCULATED;
  }

  assume(result != STMD_UNKNOWN, "result is UNKNOWN");

  return result;
}

COUNT_T
Stats::EvaluateTerrainSET(COUNT_T _terrain, COUNT_T _dominant_speed)
{
  COUNT_T return_value = STMD_UNKNOWN;

  switch (_terrain)
  {
    case STMD_INC_NULL:
    {
      return_value = SET_INC_EQ;
      break;
    }

    case STMD_DEC_NULL:
    {
      return_value = SET_DEC_EQ;
      break;
    }

    case STMD_NULL:
    {
      return_value = SET_EQ;
      break;
    }

    case STMD_INC:
    {
      return_value = SET_INC;
      break;
    }

    case STMD_INC_SLOWER:
    {
      return_value = SET_INC_SLOWER;
      break;
    }

    case STMD_INC_FASTER:
    {
      return_value = SET_INC_FASTER;
      break;
    }

    case STMD_DEC:
    {
      return_value = SET_DEC;
      break;
    }

    case STMD_DEC_SLOWER:
    {
      return_value = SET_DEC_SLOWER;
      break;
    }

    case STMD_DEC_FASTER:
    {
      return_value = SET_DEC_FASTER;
      break;
    }

    case STMD_DOM_V1_DEC:
    {
      switch (_dominant_speed)
      {
        case SDOM_DEC:
        {
          return_value = SET_DOM_V1_DEC;
          break;
        }

        case SDOM_EQ:
        {
          return_value = SET_DOM_V1_EQ;
          break;
        }

        case SDOM_INC:
        {
          return_value = SET_DOM_V1_INC;
          break;
        }

        default:
        {
          DOES_NOT_REACH();
        }
      }
      break;
    }

    case STMD_DOM_V1_INC:
    {
      switch (_dominant_speed)
      {
        case SDOM_DEC:
        {
          return_value = SET_DOM_V2_DEC;
          break;
        }

        case SDOM_EQ:
        {
          return_value = SET_DOM_V2_EQ;
          break;
        }

        case SDOM_INC:
        {
          return_value = SET_DOM_V2_INC;
          break;
        }

        default:
        {
          DOES_NOT_REACH();
        }
      }
      break;
    }

    case SDOM_NOT_CALCULATED:
    {
      return_value = SET_NOT_CALCULATED;
      break;
    }

    default:
    {
      DOES_NOT_REACH();
    }
  }

  return return_value;
}

COUNT_T
Stats::EvaluateTerrain(COUNT_T _terrain, COUNT_T _dominant_speed)
{
  COUNT_T return_value = SET_UNKNOWN;

  switch (this->EvaluateTerrainSET(_terrain, _dominant_speed))
  {
    case SET_EQ:
    case SET_INC_EQ:
    case SET_DEC_EQ:
    case SET_DOM_V1_EQ:
    case SET_DOM_V2_EQ:
    {
      return_value = SCT_EQ;
      break;
    }

    case SET_INC:
    case SET_INC_SLOWER:
    case SET_INC_FASTER:
    case SET_DOM_V1_INC:
    case SET_DOM_V2_INC:
    {
      return_value = SCT_INC;
      break;
    }

    case SET_DEC:
    case SET_DEC_SLOWER:
    case SET_DEC_FASTER:
    case SET_DOM_V1_DEC:
    case SET_DOM_V2_DEC:
    {
      return_value = SCT_DEC;
      break;
    }

    case SET_NOT_CALCULATED:
    {
      return_value = SCT_NOT_CALCULATED;
      break;
    }

    default:
    {
      DOES_NOT_REACH();
    }
  }

  return return_value;
}
