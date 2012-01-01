#include <xs1.h>
#include "led_control.h"

void toggle_leds(chanend led_chan, int toggle)
{
	led_chan <: toggle;
}