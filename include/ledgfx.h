#include <Arduino.h>
#include <FastLED.h>

// The following functions are 'borrowed' from https://github.com/davepl/DavesGarageLEDSeries.

// FractionalColor
//
// Returns a fraction of a color; abstracts the fadeToBlack away so that we can later
// do better color correction as needed
CRGB ColorFraction(const CRGB colorIn, float fraction)
{
  fraction = min(1.0f, fraction);
  return CRGB(colorIn).fadeToBlackBy(255 * (1.0f - fraction));
}


// DrawPixels
//
// Draw a sub-pixel precise amount of pixels starting at a floating point offset;
// for example, you can draw 2.75 pixels starting a 5.5, and it will end at 8.25.
void DrawPixels(float fPos, float count, CRGB color)
{
    // Figure out how much the first pixel will hold
    float availFirstPixel = 1.0f - (fPos - (long)(fPos));  // If we are starting at 2.25, there would be 0.75 avail here
    float amtFirstPixel = min(availFirstPixel, count);     // But of course we never draw more than we need
    float remaining = min(count, FastLED.size()-fPos);     // How many pixels remain after we draw the front header pixel
    int iPos = fPos;

    // Blend (add) in the color value of this first partial pixel ...and decrement the remaining pixel count by that same amount
    if (remaining > 0.0f) {
      FastLED.leds()[iPos++] += ColorFraction(color, amtFirstPixel); 
      remaining -= amtFirstPixel;                          
    }

    // Draw any full pixels and stop when we have a full pixel or less remainining
    while (remaining > 1.0f) {                             // Final pixel can 'handle' up to 1.0 full pixels, so we draw anything more here
      FastLED.leds()[iPos++] += color;                     // Draw them in one at aa time and update the remaining counts
      remaining--;
    }  

    // Draw tail pixel, up to a single full pixel
    if (remaining > 0.0f) {
        FastLED.leds()[iPos] += ColorFraction(color, remaining);     
    }
}
