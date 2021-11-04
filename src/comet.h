#include <Arduino.h>
#include <FastLED.h>

extern CRGBArray<NUM_LEDS> leds;

// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// Draw Comet (borrowed from https://github.com/davepl/DavesGarageLEDSeries)
//
void DrawComet () {
  const byte   fadeAmt      = 128;        // Fraction of 256 to fade a pixel each frame
  const int    cometSize    = 5;          // Size of comet in pixels
  const int    deltaHue     = 1;          // How far to step the cycling hue each draw cycle
  const double cometSpeed   = 4.0;        // How far to advance the comet each frame

  static byte   hue         = HUE_RED;    // Current color
  static int    iDirection  = 1;          // Current comet direction (-1 or +1)
  static double iPos        = 0;          // Current comet position

  hue   += deltaHue;                      // Update the comet color
  iPos  += iDirection * cometSpeed;       // Update the comet position

  // Flip the comet direction when it hits either end
  if (iPos > (NUM_LEDS - cometSize) || iPos == 0)
    iDirection *= -1;

  // Draw the comet at its current position
  for (int i=0; i < cometSize; i++)
    leds[(int)iPos + i].setHue(hue);

  // Fade the LEDs
  for (int j=0; j < NUM_LEDS; j++)
    if (random(2) == 0)
      leds[j] = leds[j].fadeToBlackBy(fadeAmt);
}