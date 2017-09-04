#pragma once

class Mcp9700A
{
public:
    Mcp9700A(const unsigned v_out_pin);
    virtual ~Mcp9700A();

    int init();
    double getTemperature_C();

private:
    const unsigned v_out_pin;

    unsigned read();
};





