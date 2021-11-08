#include <FastLED.h>
#include <ledgfx.h>

// Fire2012 with programmable Color Palette
//
// This code is the same fire simulation as the original "Fire2012",
// but each heat cell's temperature is translated to color through a FastLED
// programmable color palette, instead of through the "HeatColor(...)" function.
//
// Four different static color palettes are provided here, plus one dynamic one.
// 
// The three static ones are: 
//   1. the FastLED built-in HeatColors_p -- this is the default, and it looks
//      pretty much exactly like the original Fire2012.
//
//  To use any of the other palettes below, just "uncomment" the corresponding code.
//
//   2. a gradient from black to red to yellow to white, which is
//      visually similar to the HeatColors_p, and helps to illustrate
//      what the 'heat colors' palette is actually doing,
//   3. a similar gradient, but in blue colors rather than red ones,
//      i.e. from black to blue to aqua to white, which results in
//      an "icy blue" fire effect,
//   4. a simplified three-step gradient, from black to red to white, just to show
//      that these gradients need not have four components; two or
//      three are possible, too, even if they don't look quite as nice for fire.
//
// The dynamic palette shows how you can change the basic 'hue' of the
// color palette every time through the loop, producing "rainbow fire".

  // This first palette is the basic 'black body radiation' colors,
  // which run from black to red to bright yellow to white.
CRGBPalette16 gPal = HeatColors_p;
  
  // These are other ways to set up the color palette for the 'fire'.
  // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  
  // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  
  // Third, here's a simpler, three-step gradient, from black to red to white
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);

  // Fourth, the most sophisticated: this one sets up a new palette every
  // time through the loop, based on a hue that changes every time. To use
  // this hue, it needs to be added to the loop.
  // The palette is a gradient from black, to a dark color based on the hue,
  // to a light color based on the hue, to white.
  //
  //   static uint8_t hue = 0;
  //   hue++;
  //   CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
  //   CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
  //   gPal = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// DJW: This routine has been tweaked a little based partially on Dave
// Plummer's modifications mostly for mirroring and reversing (located
// here https://github.com/davepl/DavesGarageLEDSeries) and also based on
// my own tweaks.
//
// When diffusing the fire upwards, these control how much to blend in from
// the cells below (ie: downward neighbors). You can tune these coefficients
// to control how quickly and smoothly the fire spreads.
static const byte BlendSelf      = 2;
static const byte BlendNeighbor1 = 3;
static const byte BlendNeighbor2 = 2;
static const byte BlendNeighbor3 = 1;
static const byte BlendTotal     = (BlendSelf + BlendNeighbor1 + BlendNeighbor2 + BlendNeighbor3);


void Fire2012WithPalette(int  size        = NUM_LEDS, // Total number of pixels for the flame effect (max NUM_LEDS)
                         int  cooling     = 90,       // Cooling rate (less cooling = taller flames) (suggested range 20-100)
                         int  sparks      = 2,        // Number of spark attempts per frame
                         int  sparkHeight = 8,        // Max height for each spark
                         int  sparking    = 20,       // Probability of a spark for each attempt (out of 255)
                         bool bReversed   = false,     // Flames can be reversed
                         bool bMirrored   = false     // If mirrored, the LED array is split and mirrored
) {
// Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Cut the size in half when mirroring
  if (bMirrored) size /= 2;

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < size; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / size) + 2));
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int i= size - 1; i >= 2; i--) {
//    heat[i] = (heat[i - 1] + heat[i - 2] + heat[i - 2] ) / 3;
    heat[i] = (heat[i] * BlendSelf +
               heat[(i - 1) % size] * BlendNeighbor1 +
               heat[(i - 2) % size] * BlendNeighbor2 +
               heat[(i - 3) % size] * BlendNeighbor3)
               / BlendTotal;
  }
    
  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  // For sparking, I use the Arduino random() function instead of
  // random8.  It's a tad slower (the diff is 1-2FPS), but it gives
  // better random looking fire IMO.
  for (int i = 0; i < sparks; i++) {
    if( random(0,255) < sparking ) {
      int y = random8(sparkHeight);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }
  }

  // Step 4.  Map from heat cells to LED colors
  for( int i = 0; i < size; i++) {

    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    uint8_t colorindex = scale8( heat[i], 240);
    CRGB color = ColorFromPalette( gPal, colorindex);

    // Now draw the colors to the LED strip
    int j = bReversed ? (size - 1 - i) : i;
    leds[j] = color;
    if (bMirrored)
      leds[!bReversed ? (2*size - 1 - i) : size + i] = color;
  }
}