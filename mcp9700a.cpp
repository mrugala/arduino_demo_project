#include "mcp9700a.h"
#include "project_defines.h"

#define V_REF   5.
#define V_PER_C  .01
#define V_0C     .5

Mcp9700A::Mcp9700A(const unsigned v_out_pin)
    : v_out_pin(v_out_pin)
{
}

Mcp9700A::~Mcp9700A()
{
}

int Mcp9700A::init()
{
    return 0;
}

double Mcp9700A::getTemperature_C()
{
    return ((read() * V_REF / 1024) - V_0C) / V_PER_C ;
}

unsigned Mcp9700A::read()
{
    return analogRead(v_out_pin);
}

