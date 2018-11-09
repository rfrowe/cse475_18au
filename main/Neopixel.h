#ifndef _NEOPIXEL_H_
#define _NEOPIXEL_H_

#include <Adafruit_NeoPixel_ZeroDMA.h>

#define NEOPIXEL_PIN 19
#define NEOPIXEL_COUNT 16

class Neopixel {
 public:
  /** List of all neopixel functions in this class. Each takes dt, the difference in time since the last call. */
  static void rainbow(uint32_t dt);

  /** Array of all light effects, in a fixed order, used to assign an index to each. Add your effects to this! */
  static constexpr void (*LIGHTS[2])(uint32_t) = {nullptr, &rainbow};

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
  /** Current light effect being played. Please do not change this value directly. Use Neopixel::setLight(uint8_t) */
  static uint8_t _currentIdx;

  /** Time since loop was last called. */
  static uint32_t _lastLoop;

  static Adafruit_NeoPixel_ZeroDMA _strip;
};

#endif  // _NEOPIXEL_H_
