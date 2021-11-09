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
// tons of tweaking, it works better.  The biggest problem with the original
// implementation is that it doesn't work well in a loop.  You can't monitor
// button presses or potentiometer values (read brightness control) while
// it's running.  This version has been modified from the original to operate
// more like a finite-state machine so it doesn't remain in long loops to
// complete the animation.  Instead, it runs through the code once for each
// frame and then returns to the calling function.


#define NUM_SPARKS NUM_LEDS / 2  // max number of sparks
static float sparkPos[NUM_SPARKS];
static float sparkVel[NUM_SPARKS];
static float sparkCol[NUM_SPARKS];
static float flarePos;
static float flareVel;
static int   nSparks;

const  float gravity = -.004; // m/s^2
static float dying_gravity;
static float brightness;

static uint8_t boomState = 0;

/*
 * Initialize the flare
 */
void flareInit() {
  flarePos = 0;
  flareVel = float(random(110, 140)) / 100; // trial and error to get reasonable range
  brightness = 1.0f;

  // initialize launch sparks
  for (int i = 0; i < 5; i++) { 
    sparkPos[i] = 0;
    sparkVel[i] = (float(random(50,255)) / 255.0) * (flareVel / 5.0);  // random around 20% of flare velocity
    sparkCol[i] = sparkVel[i] * 1000;
    sparkCol[i] = constrain(sparkCol[i], 0, 255);
  } 
  boomState = 1;    // Advance to the next state after initialization
}
  
/*
 * Send up the flare
 */
void flareSend () {
  // sparks
  for (int i = 0; i < 5; i++) {
    sparkPos[i] += sparkVel[i];
    sparkPos[i] = constrain(sparkPos[i], 0, NUM_LEDS-1);
    sparkVel[i] += gravity;
    sparkCol[i] += -.8;
    sparkCol[i] = constrain(sparkCol[i], 200, 255);
//  leds[int(sparkPos[i])] = HeatColor(sparkCol[i]);
    DrawPixels(sparkPos[i], 1.0f, HeatColor(sparkCol[i]));
// TBD leds[int(sparkPos[i])] %= 50; // reduce brightness to 50/255 (TBD how to handle with DrawPixels)
  }
    
  // flare
//  leds[int(flarePos)] = CHSV(0, 0, int(brightness * 255));
    DrawPixels(flarePos, 1.5f, CHSV(0,0,int(brightness*255)));
//  FastLED.show();
//  FastLED.clear();
    // Fade the LEDs (TODO - write a function for this operation...)
    for (int j=0; j < NUM_LEDS; j++)
      if (random8(2) == 0)
        leds[j] = leds[j].fadeToBlackBy(32);

    flarePos += flareVel;
    flareVel += gravity;
    brightness *= .995;   // Previous value 0.985 dimmed flare too much

  // When the flare has started dropping slightly, proceed to the explosion states.
  if (flareVel < -0.05) boomState = 2;
}

/*
 * Explode!
 * 
 * Explosion happens where the flare ended.
 * Size is proportional to the height.
 */
void splosionInit() {
  nSparks = flarePos / 2; // works out to look about right

  // initialize sparks
  for (int i = 0; i < nSparks; i++) { 
    sparkPos[i] = flarePos;
    sparkVel[i] = (float(random(0, 20000)) / 10000.0) - 1.0; // from -1 to 1 
    sparkCol[i] = abs(sparkVel[i]) * 5000; // set colors before scaling velocity to keep them bright 
    sparkCol[i] = constrain(sparkCol[i], 150, 255); 
//  sparkCol[i] = random(200,255);
    sparkVel[i] *= flarePos * 4 / (NUM_LEDS-1); // proportional to height
  }
  sparkCol[0] = 255;  // this will be our known spark
  dying_gravity = gravity; 
  
  boomState = 3;      // Let's blow it up
}

void splosion () {
  float c1 = 120; 
  float c2 = 50; 
    // Fade the LEDs
//    for (int j=0; j < NUM_LEDS; j++)
//      if (random8(2) == 0)
//      leds[j] = leds[j].fadeToBlackBy(128);
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
//  FastLED.show();
    // Fade the LEDs
    for (int j=0; j < NUM_LEDS; j++)
      if (random8(1) == 0)
        leds[j] = leds[j].fadeToBlackBy(128);
//FastLED.clear();
//FastLED.show();
// When the known spark faded below a certain color, send up another flare.
if (sparkCol[0] < c2/128) boomState = 0; // as long as our known spark is lit, work with all the sparks
}

void firework() {
  switch (boomState) {
    case 0: flareInit();    break;  // Initialize the flare
    case 1: flareSend();    break;  // Send up the flare
    case 2: splosionInit(); break;  // Initialize the 'splosion'
    case 3: splosion();     break;  // Animate the 'splosion'
    default:                break;
  }
}
