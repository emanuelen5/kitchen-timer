#include "button.h"
#include "avr/io.h"
#include "avr/interrupt.h"

class AvrButton : public Button
{
    using Button::Button;

public:
    void service();
};
