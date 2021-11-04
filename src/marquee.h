#include <Arduino.h>
#include <FastLED.h>

// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// Draw Marquee (borrowed from https://github.com/davepl/DavesGarageLEDSeries)
//
#define MIN_MARQUEE_FRAME_MS  100 // Minimum milliseconds per frame for Marquee
#define WALKING_LED_SPACES    5   // Space between black walking LEDs

void DrawMarquee () {
  // Only update the frame if more than MIN_FRAME milliseonds has passed
  EVERY_N_MILLISECONDS(MIN_MARQUEE_FRAME_MS) {
    static byte j = HUE_BLUE;
    j += 4;
    byte k = j;

    // The following code is roughly equivalent to fill_rainbow(leds, NUM_LEDS, j, 8)
    CRGB c;
    for (int i = 0; i < NUM_LEDS; i++)
      leds[i] = c.setHue(k+=8);

    // Now create a counter to keep track of the initial position of the walking black
    // LEDs within the marquee.
    static unsigned int scroll = 0;
    scroll++;

    // Apply the walking black LEDs in the following loop.
    for (int i=scroll % WALKING_LED_SPACES; i < NUM_LEDS/(MIRRORED_STRIP+1)-1; i += WALKING_LED_SPACES) {
      leds[i] = CRGB::Black;
      if (MIRRORED_STRIP)
        leds[NUM_LEDS - 1 - i] = CRGB::Black;
    }
  }
}