#include <Arduino.h>
#include <FastLED.h>
#include <ledgfx.h>

extern CRGBArray<NUM_LEDS> leds;

// -._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-'~`-._,-
//
// Boom! Fireworks Show!
//
// Borrowed from http://www.anirama.com/1000leds/1d-fireworks/ and then hacked
// to pieces.  The original code had tons of problems and barely worked.  Even after
// tons of tweaking, it still barely works.  The performance of the Arduino Nano
// Every microcontroller isn't nearly enough to get fluid animation.  Another huge
// problem with this animation is that it doesn't behave well in a loop.  You can't
// monitor button presses or potentiometer values (read brightness control) while
// it's running.


#define NUM_SPARKS NUM_LEDS / 2  // max number of sparks
float sparkPos[NUM_SPARKS];
float sparkVel[NUM_SPARKS];
float sparkCol[NUM_SPARKS];
float flarePos;

float gravity = -.004; // m/s^2

/*
 * Send up a flare
 * 
 */
void flare() {
  flarePos = 0;
  float flareVel = float(random(70, 120)) / 100; // trial and error to get reasonable range
  float brightness = 1;

  // initialize launch sparks
  for (int i = 0; i < 5; i++) { 
    sparkPos[i] = 0;
    sparkVel[i] = (float(random(50,255)) / 255.0) * (flareVel / 5.0);  // random around 20% of flare velocity
    sparkCol[i] = sparkVel[i] * 1000;
    sparkCol[i] = constrain(sparkCol[i], 0, 255);
  } 
  // launch 

  while (flareVel >= -.05) {
    delay(0);
    // sparks
    for (int i = 0; i < 5; i++) {
      sparkPos[i] += sparkVel[i];
      sparkPos[i] = constrain(sparkPos[i], 0, NUM_LEDS-1);
      sparkVel[i] += gravity;
      sparkCol[i] += -.8;
      sparkCol[i] = constrain(sparkCol[i], 200, 255);
//    leds[int(sparkPos[i])] = HeatColor(sparkCol[i]);
      DrawPixels(sparkPos[i], 1.0f, HeatColor(sparkCol[i]));
      leds[int(sparkPos[i])] %= 50; // reduce brightness to 50/255 (TBD how to handle with DrawPixels)
    }
    
    // flare
//  leds[int(flarePos)] = CHSV(0, 0, int(brightness * 255));
    DrawPixels(flarePos, 1.5f, CHSV(0,0,int(brightness*255)));
    FastLED.show();
//  FastLED.clear();
    // Fade the LEDs (TODO - write a function for this operation...)
    for (int j=0; j < NUM_LEDS; j++)
      if (random8(2) == 0)
        leds[j] = leds[j].fadeToBlackBy(32);

    flarePos += flareVel;
    flareVel += gravity;
    brightness *= .995;   // Previous value 0.985 dimmed flare too much
  }
}

/*
 * Explode!
 * 
 * Explosion happens where the flare ended.
 * Size is proportional to the height.
 */
void explodeLoop() {
  int nSparks = flarePos / 2; // works out to look about right

  // initialize sparks
  for (int i = 0; i < nSparks; i++) { 
    sparkPos[i] = flarePos;
    sparkVel[i] = (float(random(0, 20000)) / 10000.0) - 1.0; // from -1 to 1 
    sparkCol[i] = abs(sparkVel[i]) * 5000; // set colors before scaling velocity to keep them bright 
    sparkCol[i] = constrain(sparkCol[i], 150, 255); 
//  sparkCol[i] = random(200,255);
    sparkVel[i] *= flarePos * 4 / (NUM_LEDS-1); // proportional to height
//  Serial.print(sparkVel[i],1);
//  Serial.print(" ");
  }
//Serial.println("");
  sparkCol[0] = 255; // this will be our known spark 
  float dying_gravity = gravity; 
  float c1 = 120; 
  float c2 = 50; 
  while(sparkCol[0] > c2/128) { // as long as our known spark is lit, work with all the sparks
    // Fade the LEDs
    for (int j=0; j < NUM_LEDS; j++)
      if (random8(2) == 0)
        leds[j] = leds[j].fadeToBlackBy(128);
//  FastLED.clear();
    for (int i = 0; i < nSparks; i++) { 
      sparkPos[i] += sparkVel[i]; 
      sparkPos[i] = constrain(sparkPos[i], 0, NUM_LEDS-1); 
      sparkVel[i] += dying_gravity; 
//    sparkCol[i] *= .99; 
      sparkCol[i] *= .98; 
      sparkCol[i] = constrain(sparkCol[i], 0, 255); // red cross dissolve 
      if(sparkCol[i] > c1) { // fade white to yellow
        DrawPixels(sparkPos[i], 1.0f, CRGB(255, 255, (255 * (sparkCol[i] - c1)) / (255 - c1)));
//      leds[int(sparkPos[i])] = CRGB(255, 255, (255 * (sparkCol[i] - c1)) / (255 - c1));
      }
      else if (sparkCol[i] < c2) { // fade from red to black
        DrawPixels(sparkPos[i], 1.0f, CRGB((255 * sparkCol[i]) / c2, 0, 0));
//      leds[int(sparkPos[i])] = CRGB((255 * sparkCol[i]) / c2, 0, 0);
      }
      else { // fade from yellow to red
        DrawPixels(sparkPos[i], 1.0f, CRGB(255, (255 * (sparkCol[i] - c2)) / (c1 - c2), 0));
//      leds[int(sparkPos[i])] = CRGB(255, (255 * (sparkCol[i] - c2)) / (c1 - c2), 0);
      }
    }
    dying_gravity *= .995; // as sparks burn out they fall slower
    FastLED.show();
  }
    // Fade the LEDs
    for (int j=0; j < NUM_LEDS; j++)
      if (random8(1) == 0)
        leds[j] = leds[j].fadeToBlackBy(128);
//FastLED.clear();
  FastLED.show();
}



