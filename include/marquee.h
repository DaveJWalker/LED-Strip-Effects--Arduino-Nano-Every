#include <Arduino.h>
#include <FastLED.h>
#include <ledgfx.h>


extern CRGBArray<NUM_LEDS> leds;

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

    // Now create a counter to keep track of the walking black LEDs within the marquee. 
    static unsigned int scroll = 100;   // 'Phase' of black LEDs
    static bool       walk_fwd = true;  // Walking forward (true) or backward?
    if (walk_fwd)  scroll++;
    else           scroll--;

    // Every 10 seconds, reverse the scroll direction
    EVERY_N_SECONDS(10) {walk_fwd = not walk_fwd;}

    // Apply the walking black LEDs in the following loop.
    for (int i=scroll % WALKING_LED_SPACES; i < NUM_LEDS/(MIRRORED_STRIP+1)-1; i += WALKING_LED_SPACES) {
      leds[i] = CRGB::Black;
      if (MIRRORED_STRIP)
        leds[NUM_LEDS - 1 - i] = CRGB::Black;
    }
  }
}


// FIXME: Not really working properly... need to look into why.
void DrawMarqueeSmooth() {
  // Only update the frame if more than MIN_FRAME milliseonds has passed
  EVERY_N_MILLISECONDS(1) { //MIN_MARQUEE_FRAME_MS) {
    static byte j = HUE_BLUE;
    j += 4;
    byte k = j;

    // The following code is roughly equivalent to fill_rainbow(leds, NUM_LEDS, j, 8)
    CRGB c;
    for (int i = 0; i < NUM_LEDS; i++)
      leds[i] = c.setHue(k+=8);

    static float scroll = 0.0f;
    scroll += 0.3f;
    if (scroll > 5.0f)
        scroll -= 5.0f;

    for (float i = scroll; i < NUM_LEDS/2 - 1; i += 5) {
        DrawPixels(i, 3, CRGB::Black);
        DrawPixels(NUM_LEDS-1-(int)i, 3, CRGB::Black);
    }
  }
}
