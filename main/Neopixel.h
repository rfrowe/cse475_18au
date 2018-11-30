#ifndef _NEOPIXEL_H_
#define _NEOPIXEL_H_

#include <Adafruit_NeoPixel_ZeroDMA.h>

#define NEOPIXEL_PIN 19
#define NEOPIXEL_COUNT 16
#define NEOPIXEL_BRIGHTNESS 5

class Neopixel {
 public:
  /** List of all neopixel functions in this class. Each takes dt, the difference in time since the last call. */
  static bool rainbow(uint32_t dt);
  static bool smoothRainbow(uint32_t dt);
  static bool strobe(uint32_t dt);
  static bool bell(uint32_t dt);
  static bool melodic(uint32_t dt);
  static bool insects(uint32_t dt);
  static bool wavez(uint32_t dt);
  static bool rotating(uint32_t dt);
  static bool lightning(uint32_t dt);
  static bool wind(uint32_t dt);
  static bool blinkDot(uint32_t dt);
  static bool breathCycle(uint32_t dt);
  static bool twinkle_lights(uint32_t dt);
  static bool lightning2(uint32_t dt);
  static bool rotatingSlowly(uint32_t dt);
  static bool rainCycle(uint32_t dt);
  static bool quadRand(uint32_t dt);
  static bool fire(uint32_t dt);
  static bool flashing(uint32_t dt);
  static bool bell2(uint32_t dt);

  /** Array of all light effects, in a fixed order, used to assign an index to each. Add your effects to this! */
  static constexpr bool (*LIGHTS[])(uint32_t) = {
    nullptr,            // 00
    &rainbow,           // 01
    &smoothRainbow,     // 02
    &strobe,            // 03
    &bell,              // 04
    &melodic,           // 05
    &insects,           // 06
    &wavez,             // 07
    &rotating,          // 08
    &lightning,         // 09
    &wind,              // 0A
    &blinkDot,          // 0B
    &breathCycle,       // 0C
    &twinkle_lights,    // 0D
    &lightning2,        // 0E
    &rotatingSlowly,    // 0F
    &rainCycle,         // 10
    &quadRand,          // 11
    &fire,              // 12
    &flashing,          // 13
    &bell2              // 14
  };

  /**
   * Use this to set the current light gesture.
   *
   * @param lightIdx Index of new light effect to play in Neopixel::LIGHTS.
   */
  static void setLight(uint8_t lightIdx);

  /**
   * @returns the index of the current light gesture.
   */
  static uint8_t getLight();

  /** Called to loop whatever the currently selected lighting effect is. */
  static void loop();

  /** Initialize Neopixel. */
  static void setup();
 private:
  /** Adafruit_NeoPixel_ZeroDMA SUCKS and doesn't have the _fill method by default.
   *
   * @param c Color to fill with.
   * @param first Index of LED to start filling with.
   * @param count Number of LEDs to fill starting from first.
   */
  static void _fill(uint32_t c, uint16_t first=0, uint16_t count=NEOPIXEL_COUNT);

  static uint32_t wheel(byte pos);

  /** Current light effect being played. Please do not change this value directly. Use Neopixel::setLight(uint8_t) */
  static uint8_t _currentIdx;

  /** Time since loop was last called. */
  static uint32_t _lastLoop;

  static Adafruit_NeoPixel_ZeroDMA _strip;
};

#endif  // _NEOPIXEL_H_
