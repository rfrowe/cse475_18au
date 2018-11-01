/*
 * Light gestures for startle, ambient, and active states.
 * Author: Akhil Avula, Kheng Wei
 * EE 475 Autumn 2018
 */
#include <Adafruit_FeatherOLED.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>

// DMA NeoPixels work ONLY on SPECIFIC PINS.
// On Circuit Playground Express: 8, A2 and A7 (TX) are valid.
// On Feather M0, Arduino Zero, etc.: 5, 11, A5 and 23 (SPI MOSI).
// On GEMMA M0: pin 0.
// On Trinket M0: pin 4.
// On Metro M4: 3, 6, 8, 11, A3 and MOSI
#define PIN         A5
#define NUM_PIXELS 16
#define GESTURE 6 // 0: startle gesture, 1 2 3: ambient gestures,4 5 6: active gestures
/****CHANGE THE GESTURE NUMBER TO TEST OUT EACH GESTURE****/

Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();
Adafruit_NeoPixel_ZeroDMA strip(NUM_PIXELS, PIN, NEO_GRBW);

void setup() {
  oled.begin(); //Initialize the oled display
  oled.display();
  oled.clearDisplay();
  oled.init();
  oled.setCursor(0, 0);
  if (0 == GESTURE) {
    oled.print("Startle gesture");
  } else if (1 == GESTURE) { // ambient
    oled.print("Ambient Gesture 1");
  } else if (2 == GESTURE) { // ambient
    oled.print("Ambient Gesture 2");
  } else if (3 == GESTURE) { // ambient
    oled.print("Ambient Gesture 3");
  } else if (4 == GESTURE) { // active
    oled.print("Active Gesture 1");
  } else if (5 == GESTURE) { // active
    oled.print("Active Gesture 2");
  } else if (6 == GESTURE) { // active
    oled.print("Active Gesture 3");
  }
  oled.display();
  
  strip.begin(); // Initialize the LEDs
  strip.setBrightness(32);
  strip.show();
}

void loop() {

  if (0 == GESTURE) {
    startleLight();
  } else if (1 == GESTURE) { // ambient
    ambient1();
  } else if (2 == GESTURE) { // ambient
    ambient2();
  } else if (3 == GESTURE) { // ambient
    ambient3();
  } else if (4 == GESTURE) { // active
    active1();
  } else if (5 == GESTURE) { // active
    active2();
  } else if (6 == GESTURE) { // active
    active3();
  }
}

void startleLight(void) {
  uint16_t i;
  uint32_t temp;
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
}

void ambient1(void) {
  uint16_t i;
  uint32_t temp;
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
}

void ambient2(void) {
  uint16_t i;
  uint32_t temp;
  uint32_t elapsed, t, startTime = micros();
    for(;;) {
    t       = micros();
    elapsed = t - startTime;
    if(elapsed > 5000000) break; // Run for 5 seconds
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((uint8_t)(
        (elapsed * 256 / 1000000) + i * 256 / strip.numPixels())));
    }
    bounceBrightness(8,33,50);
  }
}

void ambient3(void) {
  uint16_t i;
  uint32_t temp;
  uint32_t elapsed, t, startTime = micros();
  for(;;) {
    t       = micros();
    elapsed = t - startTime;
    if(elapsed > 5000000) break; // Run for 5 seconds
    for(i=0; i<strip.numPixels() / 2; i++) {
      strip.setPixelColor(i, Wheel((uint8_t)(
        (elapsed * 256 / 1000000) + i * 256 / strip.numPixels())));
      bounceBrightness(0,33,50);
      strip.setPixelColor(i + strip.numPixels() / 2, Wheel((uint8_t)(
        (elapsed * 256 / 1000000) + i * 256 / strip.numPixels())));
      bounceBrightness(0,33, 50);
      strip.setBrightness(0);
      strip.show();
   }
   for(i=0; i<strip.numPixels(); i++) {
     strip.setPixelColor(i, 0);
   }
   strip.setBrightness(0);
   strip.show();
 }
}

void bounceBrightness(int low, int high, int delTime) {
  for (int j = low; j < high; j++) {
    strip.setBrightness(j);
    strip.show();
    delay(delTime);
  }
  for (int k = high - 1; k >= low; k--) {
    strip.setBrightness(k);
    strip.show();
    delay(delTime);
  }
}

void active1(void) {
  uint16_t i;
  uint32_t temp;
  for(uint32_t c = 0xFF0000; c; c >>= 8) { // Red, green, blue
    for(i=0; i<strip.numPixels()/2; i++) {
      strip.setPixelColor(i, c);
      strip.setPixelColor(i + strip.numPixels()/2, c >> 8);
      strip.show();
      delay(50);
    }
  }
}

void active2(void) {
  uint16_t i;
  uint32_t temp;
  for(uint32_t c = 0xFF0000; c; c >>= 8) { // Red, green, blue
    for(i=0; i<=strip.numPixels()/2; i++) {
      strip.setPixelColor(i, c);
      strip.setPixelColor(strip.numPixels() - i, c >> 8);
      strip.show();
      delay(50);
    }
  }
}

void active3(void) {
  uint16_t i;
  uint32_t temp;
  strip.setBrightness(13);
  for(;;) { // Red, green, blue
    for(i=0; i<=strip.numPixels(); i++) {
      strip.setPixelColor(i, 0xFF0000 << 8);
      strip.show();
      delay(50);
    }
    for(i=0; i<=strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
    }
    strip.show();
    delay(500);
    for(i=0; i<=strip.numPixels(); i++) {
      strip.setPixelColor(i,0xFF0000 << 8);
    }
    strip.show();
    delay(500);
    for(i=0; i<=strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
    }
    strip.show();
    delay(500);
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
