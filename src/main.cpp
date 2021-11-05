#include <Arduino.h>
#include <FastLED.h>
#include <ezButton.h>

// Set up debug defines
#define DEBUG 1

// By using #defines, we prevent the serial debug code from getting compiled when
// not needed, thus saving program space.
#if DEBUG == 1
#define debug(x)      Serial.print(x)
#define debugln(x)    Serial.println(x)
#define debug1(x,y)   Serial.print(x,y)
#define debugln1(x,y) Serial.println(x,y)
#else
#define debug(x)
#define debugln(x)
#define debug1(x,y)
#define debugln1(x,y)
#endif


// LED defines for number of LEDs, LED chipset, etc.
#define NUM_LEDS              300
#define LED_PIN               2
#define BRIGHT_PIN            21
#define COLOR_ORDER           GRB
#define LED_CHIPSET           WS2812B
#define INIT_BRIGHTNESS       50
#define VOLTS                 5
#define MAX_MILLIAMPS         5000

// FIXME: The following defines are temporary because I'm using ledgfx.h
// utility written by Dave Plumber.  That utility requires these defines...
// I'll look into removing it later... maybe.
#define FAN_SIZE              16  // Number of LEDs in each fan
#define NUM_FANS              3   // Number of Fans
#define LED_FAN_OFFSET        4   // How far from bottom first pixel is

// Button-related defines
#define PTRN_CHG_PIN          4   // Pin for the pattern change button
#define MaxPatterns           8   // Total number of patterns
#define PatternSwitchTime     30  // Time for automatically switching Patterns in seconds

// Other defines
#define MIRRORED_STRIP        1   // Set to 0 or 1... '1' to make the LED strip 'mirrored'

CRGBArray<NUM_LEDS> leds;


uint8_t value             = 100;
uint8_t white_pos         = 0;
uint8_t gHue              = 0;    // Rotating "base color" used by many of the patterns
uint8_t Brightness        = 0;    // LED brightness
int     potVal;                   // Potentiometer (used for LED brightness control)
boolean autoSwitchPattern = true; // Boolean to control pattern autoswitching
uint8_t currentPattern    = 0;

// Keep track of frames-per-second
double FrameStart         = 0;    // Frame start time
double FrameEnd           = 0;    // Frame end time
double fps                = 0;    // Average frames-per-second

// Include LED patterns
#include <twinkles.h>
#include <simple.h>
#include <marquee.h>
#include <comet.h>
#include <fire.h>


// Create ezButton object for pattern change button
ezButton button_ptrn_chg(PTRN_CHG_PIN);


// FramesPerSecond (borrowed from https://github.com/davepl/DavesGarageLEDSeries)
//
// Tracks a weighted average to smooth out the values that it calcs as the simple
// reciprocal of the amount of time taken specified by the caller.  So 1/3 of a
// second is 3fps. and it will take up to 10 frames or so to stabilize on that
// value.
double FramesPerSecond(double seconds) {
  static double framesPerSecond;
  framesPerSecond = (framesPerSecond * 0.9) + (1.0 / seconds * 0.1);
  return framesPerSecond;
}


// Initial setup
void setup() {
  Serial.begin(115200);

  // Add entropy to random number generator; we use a lot of it for fire.
  random16_add_entropy( random());

  // Set pattern change button debounce time to 50ms
  button_ptrn_chg.setDebounceTime(50);

  // Setup palette for twinkle show
  chooseNextColorPalette(gTargetPalette);

  // Set up the LED strip
  FastLED.addLeds<LED_CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MAX_MILLIAMPS);
  FastLED.setBrightness(INIT_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  // Wait a couple of seconds after initial setup
  delay(2000);
}


// Main Loop
void loop() {

  // Set the FrameStart time to the end of the last frame.
  FrameStart = FrameEnd;

  // Read the brightness pot and set LED brightness
  potVal = 1023 - analogRead(BRIGHT_PIN);
  Brightness = potVal/4;  // Convert value to u_int8
  FastLED.setBrightness(Brightness);

  // Check pattern change button status
  button_ptrn_chg.loop();

  // Whenever the button is pressed, increment the current pattern
  // and turn on/off the pattern autoswitch flag.
  if (button_ptrn_chg.isPressed()) {
    if (autoSwitchPattern) {
      currentPattern = 0;
      autoSwitchPattern = false;
    }
    else if (currentPattern < MaxPatterns-1) {
      currentPattern++;
    }
    else {
      currentPattern = 0;
      autoSwitchPattern = true;
    }
    debug("BUTTON PRESSED. Pattern: ");
    debug(currentPattern);
    debug("  AutoSwitchPattern: ");
    debugln(autoSwitchPattern);
  }

  // If autoswitch is enabled, cycles through the patterns periodically.
  // (The last pattern is always blank so skip that one.)
  if (autoSwitchPattern) {
    EVERY_N_SECONDS(PatternSwitchTime) {
      if (currentPattern < MaxPatterns-2)
        currentPattern++;
      else
        currentPattern = 0;
      debug("Pattern automatically changed. Pattern: ");
      debugln(currentPattern);
    }
  }

  // Display an LED pattern based on currentPattern value
  switch (currentPattern) {
  case 0:
    rainbowSparkles();
    break;
  case 1:
    twinkleMaster();
    break;
  case 2:
    juggle();
    break;
  case 3:
    sinelon_no_gap();
    break;
  case 4:
    DrawMarquee ();
    break;
  case 5:
    DrawComet ();
    break;
  case 6:
    Fire2012WithPalette ();
    break;
  default:
    FastLED.clear();
    break;
  }

  // And update the LEDs
  FastLED.show();

  // Calculate the time spent processing the frame and the corresponding
  // frames-per-second.
  FrameEnd = millis()/1000.0;
  fps = FramesPerSecond(FrameEnd - FrameStart);

  // Display the FPS
  EVERY_N_SECONDS(3) {
    Serial.print("FPS: ");
    Serial.println(fps,1);
  }

  unsigned long elapsedTime_ms  = millis();
  float         fsecondsRunning = elapsedTime_ms/1000.0;
//unsigned long elapsedTime_us  = micros();
//float         fmillis         = elapsedTime_us/1000.0;
  EVERY_N_SECONDS(5) {
    debug   ("Elapsed Time (s): ");
//  debug   (elapsedTime_ms);
//  debug   (", ");
    debugln1(fsecondsRunning,3);
//  debug   (", ");
//  debug   (elapsedTime_us);
//  debug   (", ");
//  debugln1(fmillis,1);
  }
}
