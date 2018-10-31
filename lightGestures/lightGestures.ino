/*
 * Light gestures for startle, ambient, and active states.
 * Author: Akhil Avula
 * EE 475 Autumn 2018
 */
#include <Adafruit_NeoPixel_ZeroDMA.h>

// DMA NeoPixels work ONLY on SPECIFIC PINS.
// On Circuit Playground Express: 8, A2 and A7 (TX) are valid.
// On Feather M0, Arduino Zero, etc.: 5, 11, A5 and 23 (SPI MOSI).
// On GEMMA M0: pin 0.
// On Trinket M0: pin 4.
// On Metro M4: 3, 6, 8, 11, A3 and MOSI
#define PIN         A5
#define NUM_PIXELS 16
#define GESTURE 4 // 0: startle gesture, 1 2 3: ambient gestures,4 5 6: active gestures
/****CHANGE THE GESTURE NUMBER TO TEST OUT EACH GESTURE****/

Adafruit_NeoPixel_ZeroDMA strip(NUM_PIXELS, PIN, NEO_GRBW);

void setup() {
  strip.begin();
  strip.setBrightness(32);
  strip.show();
}

void loop() {
  uint16_t i;
  uint32_t temp;
  if (0 == GESTURE) {
    
    // 'Color wipe' across all pixels
    for(uint32_t c = 0xFF0000; c; c >>= 16) { // Red, blue
      for(i=0; i<strip.numPixels(); i++) {
        if (i%2 == 0) {
          strip.setPixelColor(i, c);
        } else {
          temp = c >> 16;
          strip.setPixelColor(i, temp);
        }
      }
      strip.show();
      delay(500);
    }
    
  } else if (1 == GESTURE) { // ambient
//    uint16_t i;
//    uint32_t temp;
    // 'Color wipe' across all pixels
    for(uint32_t c = 0xFF0000; c; c >>=8) {
      for(i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
      }
      for (int j = 0; j < 33; j++) {
        strip.setBrightness(j);
        strip.show();
        delay(50);
      }
      for (int k = 32; k >= 0; k--) {
        strip.setBrightness(k);
        strip.show();
        delay(50);
      }
      
    }
  } else if (2 == GESTURE) { // ambient
    uint32_t elapsed, t, startTime = micros();
    for(;;) {
    t       = micros();
    elapsed = t - startTime;
    if(elapsed > 5000000) break; // Run for 5 seconds
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((uint8_t)(
        (elapsed * 256 / 1000000) + i * 256 / strip.numPixels())));
    }
    for (int j = 8; j < 33; j++) {
        strip.setBrightness(j);
        strip.show();
        delay(50);
      }
      for (int k = 32; k >= 8; k--) {
        strip.setBrightness(k);
        strip.show();
        delay(50);
      }
  }
  } else if (3 == GESTURE) { // ambient
    
  } else if (4 == GESTURE) { // active
    for(uint32_t c = 0xFF0000; c; c >>= 8) { // Red, green, blue
      for(i=0; i<strip.numPixels()/2; i++) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i + strip.numPixels()/2, c >> 8);
        strip.show();
        delay(50);
      }
    }

  } else if (5 == GESTURE) { // active
    
  } else if (6 == GESTURE) { // active
    
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
