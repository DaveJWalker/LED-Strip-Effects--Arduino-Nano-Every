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