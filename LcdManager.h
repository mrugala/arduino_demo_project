#pragma once

class LcdManager
{
public:
    struct LcdPins
    {
        const unsigned rs;
        const unsigned enable;
        const unsigned d4;
        const unsigned d5;
        const unsigned d6;
        const unsigned d7;
    };

    LcdManager(const LcdPins pins);
    virtual ~LcdManager();

    int init(const unsigned cols, const unsigned rows, const unsigned screens);
    
    template<typename... Args>
    void print(const unsigned screen, Args... args)
    {
         
    }
    void goToScreen(const unsigned screen);

private:

};





