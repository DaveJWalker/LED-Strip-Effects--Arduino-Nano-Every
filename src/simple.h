// -'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-
//
//                                PATTERNS
//
// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
// 
// Various simple LED patterns are included in this file.  Many of
// of these functions require the following get defined (which is done
// in the main program):
//
//    #define NUM_LEDS 300
//    CRGBArray<NUM_LEDS> leds;
//    uint8_t value = 100;
//    uint8_t gHue = 0;



#include <Arduino.h>
#include <FastLED.h>

// Other defines
#define MIRRORED_STRIP        1   // Set to 0 or 1... '1' to make the LED strip 'mirrored'


// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// Moving rainbow with random white twinkles
//
void rainbowSparkles ()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + i*5, 150, value);

    if (random(255) > 252)
      leds[i] = CHSV(0,0,200);
  }

  EVERY_N_MILLISECONDS(1) {
    gHue++;
  }
}


// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// A colored dot sweeping back and forth, with fading trails
//
void sinelon()
{
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(20, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);

  EVERY_N_MILLISECONDS(5) {
    gHue++;
  }

}


// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// A colored dot sweeping back and forth, with fading trails
// AND without gaps between pixels
//
void sinelon_no_gap()
{
  static int last_pos = 0;
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 30, 0, NUM_LEDS-1 );
  if (abs(pos - last_pos) > 20) {
    // this is a really big gap:  some other effect has been running
    // rather than fill many LEDs suddenly with one colour
    // let's just pretend this frame never happened
    last_pos = pos;
  }
  while (last_pos != pos) {
    if (last_pos < pos) {
      last_pos++;
    } else {
      last_pos--;
    }
    leds[last_pos] += CHSV( gHue, 255, 192);
  }

  EVERY_N_MILLISECONDS(5) {
    gHue++;
  }

}


// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// Eight colored dots, weaving in and out of sync with each other
//
void juggle() {
  static int last_pos = 0;
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i*2 + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// draws a line that fades between 2 random colors
// TODO:  Add logic to rotate the starting point
/* FIXME - not currently working
void gradient_fill() {

  //  uint8_t hue1 = 60;
  //  uint8_t hue2 = random8(255);
  uint8_t hue1 = random8(255);
  uint8_t hue2 = hue1 + random8(30, 61);

  for ( int i = 0; i < NUM_LEDS; i++) {
    //fill_gradient (leds, 0, CHSV(0, 255, 255), i, CHSV(96, 255, 255), SHORTEST_HUES);
    fill_gradient (leds, 0, CHSV(hue1, 255, 255), i, CHSV(hue2, 255, 255), SHORTEST_HUES);
    delay(15);
    FastLED.show();
    //  FastLED.clear();
  }
}*/


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

/*
// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// Bouncing Balls (borrowed from https://github.com/davepl/DavesGarageLEDSeries)
//
using namespace std;
#include <vector>

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))   // Count elements of static array

static const CRGB ballColors [] =
{
    CRGB::Green,
    CRGB::Red,
    CRGB::Blue,
    CRGB::Orange,
    CRGB::Indigo
};

class BouncingBallEffect
{
  private:

    double InitialBallSpeed(double height) const
    {
      return sqrt(-2 * Gravity * height);   // Math
    }

    size_t  _cLength;
    size_t  _cBalls;
    byte    _fadeRate;
    bool    _bMirrored;

    const double Gravity = -9.81;   // Physics!
    const double StartHeight = 1;   // Drop balls from max height initially
    const double ImpactVelocity = InitialBHallSpeed(StartHeight);
    const double SpeedKnob = 4.0;   // Higher values will slow the effect

    vector<double> ClockTimeAtLastBounce, Height, BallSpeed, Dampening;
    vector<CRGB>   Colors;

  public:
    // BouncingBallEffect
    //
    // Caller specs strip length, number of balls, persistence level (255 is least),
    // and whether the balls should be drawn mirrored from each side.
    BouncingBallEffect(size_t cLength, size_t ballCount = 3, byte fade = 0, bool bMirrored = false)
      : _cLength(cLength - 1),

};*/
