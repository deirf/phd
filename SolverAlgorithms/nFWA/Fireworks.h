#pragma once

#include "../../common/Types.h"
#include "../../common/Record.h"

class Firework;


class Fireworks
{
  private:
    TFirework* fireworks = NULL;
    COUNT_T fw_pop_index = 0;

  public:
    Fireworks(TFirework* _fireworks, COUNT_T _fw_pop_index) : fireworks(_fireworks), fw_pop_index(_fw_pop_index) {};
    ~Fireworks() {}

    TFirework* GetFireworks() { return this->fireworks; }
    Firework* GetFirework(COUNT_T _fw_index) { return this->GetFireworks()->getData( _fw_index ); }
    void SetFirework(Firework* _firework, COUNT_T _fw_index) { this->GetFireworks()->setData( _fw_index, _firework ); }

    COUNT_T GetFwPopIndex() { return this->fw_pop_index; }
};
