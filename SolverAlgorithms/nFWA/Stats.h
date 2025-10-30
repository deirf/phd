#pragma once
#include "../../common/Types.h"

class Spark;

class Stats
{
  private:
    Spark* spark = NULL;

    const uint telemetrics[3][3][3] = {
      { { STMS_V1mV2mAm, STMS_V1mV2mAn, STMS_V1mV2mAp }, { STMS_UNKNOWN,  STMS_UNKNOWN,  STMS_V1mV2nAp  }, { STMS_UNKNOWN,  STMS_UNKNOWN,  STMS_V1mV2pAp } },
      { { STMS_V1nV2mAm, STMS_UNKNOWN,  STMS_UNKNOWN  }, { STMS_UNKNOWN,  STMS_V1nV2nAn, STMS_UNKNOWN   }, { STMS_UNKNOWN,  STMS_UNKNOWN,  STMS_V1nV2pAp } },
      { { STMS_V1pV2mAm, STMS_UNKNOWN,  STMS_UNKNOWN  }, { STMS_V1pV2nAm, STMS_UNKNOWN,  STMS_UNKNOWN   }, { STMS_V1pV2pAm, STMS_V1pV2pAn, STMS_V1pV2pAp } },
     };

    const uint light_telemetrics[3] = { STMS_V1m, STMS_V1n, STMS_V1p };

    const uint terrians[3][3][3] = {
      { { STMD_DEC_SLOWER, STMD_DEC,     STMD_DEC_FASTER }, { STMD_UNKNOWN, STMD_UNKNOWN, STMD_DEC     }, { STMD_UNKNOWN,    STMD_UNKNOWN, STMD_DOM_V1_DEC } },
      { { STMD_DEC_NULL,   STMD_UNKNOWN, STMD_UNKNOWN    }, { STMD_UNKNOWN, STMD_NULL,    STMD_UNKNOWN }, { STMD_UNKNOWN,    STMD_UNKNOWN, STMD_INC_NULL   } },
      { { STMD_DOM_V1_INC, STMD_UNKNOWN, STMD_UNKNOWN    }, { STMD_INC,     STMD_UNKNOWN, STMD_UNKNOWN }, { STMD_INC_FASTER, STMD_INC,     STMD_INC_SLOWER } },
    };

    const uint light_terrians[3] = { STMD_DEC, STMD_NULL, STMD_INC };

    const uint directions[3][3][3] = {
      { { SET_DEC,        SET_DEC,     SET_DEC     }, { SET_UNKNOWN, SET_UNKNOWN, SET_DEC     }, { SET_UNKNOWN, SET_UNKNOWN, SET_DOM_V1_DEC } },
      { { SET_EQ,         SET_UNKNOWN, SET_UNKNOWN }, { SET_UNKNOWN, SET_EQ,      SET_UNKNOWN }, { SET_UNKNOWN, SET_UNKNOWN, SET_EQ         } },
      { { SET_DOM_V1_INC, SET_UNKNOWN, SET_UNKNOWN }, { SET_INC,     SET_UNKNOWN, SET_UNKNOWN }, { SET_INC,     SET_INC,     SET_INC        } },
    };

    const uint light_direction[3] = { SET_DEC, SET_EQ, SET_INC };

    const uint dominant_speed[3]    = { SDOM_DEC, SDOM_EQ, SDOM_INC };

    PREC AlmostZero(PREC _value);

  public:
    Stats() {};
    ~Stats() {};

    COUNT_T EvaluateTerrainSET(COUNT_T _terrain, COUNT_T _dominant_speed);
    COUNT_T EvaluateTerrain(COUNT_T _terrain, COUNT_T _dominant_speed);

    COUNT_T GetTerrain(COUNT_T _velo1, COUNT_T _velo2, COUNT_T _accel);
    COUNT_T GetLastTerrain() { return this->GetTerrain(SSIV_VELO_FIRST, SSIV_VELO_FIRST + 1, SSIA_ACCEL_FIRST); }

    COUNT_T GetTelemetric(COUNT_T _velo1, COUNT_T _velo2, COUNT_T _accel);
    COUNT_T GetLastTelemetric() { return this->GetTelemetric(SSIV_VELO_FIRST, SSIV_VELO_FIRST + 1, SSIA_ACCEL_FIRST); }

    COUNT_T GetDominantSpeed(COUNT_T _velo1, COUNT_T _velo2, COUNT_T _accel);
    COUNT_T GetLastDominantSpeed() { return this->GetDominantSpeed(SSIV_VELO_FIRST, SSIV_VELO_FIRST + 1, SSIA_ACCEL_FIRST); }

    void SetSpark(Spark* _spark) { this->spark = _spark; };
    Spark* GetSpark() { return this->spark; };

    void CalculateTelemetrics();
};
