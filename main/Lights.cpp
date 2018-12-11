#include "Neopixel.h"

// A simpler version of wheel. These are just 16 colors, in rainbow order.
uint32_t rainbowColors[NEOPIXEL_COUNT] = {
  16711680, 13578240, 10444800, 7311360, 4177920, 1044480, 56865, 44625, 32385,
  20145, 7905, 1179885, 4325565, 7471245, 10616925, 13762605
};

uint32_t Neopixel::wheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return _strip.Color(255 - pos * 3, 0, pos * 3);
  }
  if (pos < 170) {
    pos -= 85;
    return _strip.Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return _strip.Color(pos * 3, 255 - pos * 3, 0);
}

bool Neopixel::rainbow(uint32_t dt) {
  static uint8_t offset;

  if (dt >= 50) {
    for (uint8_t pix = 0; pix < NEOPIXEL_COUNT; pix++) {
      _strip.setPixelColor((pix + offset) % NEOPIXEL_COUNT, rainbowColors[pix]);
    }
    _strip.show();

    offset++;
    return true;
  }

  return false;
}

bool Neopixel::smoothRainbow(uint32_t dt) {
  static uint16_t offset;

  if (!dt || dt >= 25) {
    for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++) {
      _strip.setPixelColor(i, wheel(((i * 256 / NEOPIXEL_COUNT) + offset) & 255));
    }
    _strip.show();
    offset = (offset + 5) % 1024;
    return true;
  }

  return false;
}

bool Neopixel::strobe(uint32_t dt) {
  static uint8_t strobes_remaining;
  static bool on;

  if (!dt) {
    strobes_remaining = 26;
    on = false;
  }

  if (strobes_remaining && dt >= 50) {
    if (on) {
      _strip.clear();
      strobes_remaining--;
    } else {
      _fill(0xFFFFFFFF);
    }
    on = !on;
    _strip.show();
    return true;
  }

  return false;
}

bool Neopixel::bell(uint32_t dt) {
  static uint8_t offset;

  if (!dt || dt >= 150) {
    uint8_t color;

    for (uint8_t pix = 0; pix < NEOPIXEL_COUNT; pix++) {
      color = (int) (sin(pix + offset) * 127) + 128;
      _strip.setPixelColor(pix, color, color, 0);
    }

    _strip.show();
    offset++;
    return true;
  }

  return false;
}

bool Neopixel::melodic(uint32_t dt) {
  static uint8_t color_offset, pix_offset;
  static bool on;

  if (!dt) {
    color_offset = pix_offset = 0;
    on = false;
  }

  if (dt >= 150) {
    if (on) {
      _strip.clear();
      if (pix_offset > 2) {
        pix_offset = 0;
        color_offset += 16;
      } else {
        pix_offset = (pix_offset + 1) % 1024;
      }
      on = false;
    } else {
      for (uint8_t pix = 0; pix < NEOPIXEL_COUNT - pix_offset; pix += 4) {
        _strip.setPixelColor(pix + pix_offset, wheel((pix + color_offset) % 255));
      }
      on = true;
    }
    _strip.show();
    return true;
  }

  return false;
}

bool Neopixel::insects(uint32_t dt) {
  static bool fading;
  static bool in;

  if (!dt) {
    fading = in = false;
  }

  if (fading) {
    if (dt >= 25) {
      uint8_t brightness = _strip.getBrightness();

      if (in) {
        if (brightness >= NEOPIXEL_BRIGHTNESS) {
          in = false;
        } else {
          _strip.setBrightness(brightness + 3);
        }
      } else {
        if (brightness == 0) {
          fading = false;
        } else {
          _strip.setBrightness(brightness - 3);
        }
      }
      _strip.show();

      return true;
    }
  } else {
    _strip.setBrightness(0);
    _strip.clear();
    for (uint8_t i = 0; i < 6; i++) {
      _strip.setPixelColor(rand() % 16, 0, 239 + rand() % 128 - 112, 31 + rand() % 128 - 112);
    }
    fading = in = true;
    _strip.show();
  }

  return false;
}

bool Neopixel::wavez(uint32_t dt) {
  static bool fading;
  static bool in;

  if (!dt) {
    fading = in = false;
  }

  if (fading) {
    if (dt >= 50) {
      uint8_t brightness = _strip.getBrightness();

      if (in) {
        if (brightness >= NEOPIXEL_BRIGHTNESS) {
          in = false;
        } else {
          _strip.setBrightness(brightness + 1);
        }
      } else {
        if (brightness == 0) {
          fading = false;
        } else {
          _strip.setBrightness(brightness - 1);
        }
      }
      _strip.show();

      return true;
    }
  } else {
    _strip.setBrightness(0);
    for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++) {
      uint32_t color = _strip.Color(0, rand() % 128 - 112 + 239, rand() % 128 - 112 + 227);
      _strip.setPixelColor(i, color);
    }
    fading = in = true;
    _strip.show();
  }

  return false;
}

bool Neopixel::rotating(uint32_t dt) {
  static uint8_t loopCounter = 0;
  if (dt >= 200) {
    for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++){
      uint8_t brightness = 0;
      if ((i + loopCounter) % 4 == 0) {
        brightness = 255;
      }
      _strip.setPixelColor(i, (i + loopCounter) % 4 ? 0 : 0xFFFFFFFF);
    }
    _strip.show();

    loopCounter = (loopCounter + 1) % NEOPIXEL_COUNT;
    return true;
  }
  return false;
}

// Strobes exactly 10 times then stops.
// Note: strobe will not reset until another light has been used.
bool Neopixel::lightning(uint32_t dt) {
  static uint8_t strobes_remaining;
  static bool on;

  if (!dt) {
    strobes_remaining = 10;
    on = false;
  }

  if (strobes_remaining && dt >= 50) {
    if (on) {
      _strip.clear();
      strobes_remaining--;
    } else {
      _fill(0xFFFFFFFF);
    }
    on = !on;
    _strip.show();
    return true;
  }

  return false;
}

bool Neopixel::wind(uint32_t dt) {
  static uint8_t offset = 0;
  static uint8_t rand_val = random(25, 200);

  if (dt > rand_val) {
    _strip.clear();
    _fill(rainbowColors[0], offset % NEOPIXEL_COUNT, 5);
    for (uint8_t i = 0; i < 5; i++) {
      _strip.setPixelColor((offset + i) % NEOPIXEL_COUNT, wheel(((offset * 256 / NEOPIXEL_COUNT) + offset) & 255));
    }
    _strip.show();
    offset++;
    rand_val = random(25, 200);
    return true;
  }

  return false;
}

bool Neopixel::blinkDot(uint32_t dt) {
  if (!dt) {
    _strip.setBrightness(15);
  }

  if (dt >= 100) {
    _strip.clear();
    for (int i = 0; i < _strip.numPixels(); i++){
      if (random(10) <= 1) _strip.setPixelColor(i, 0xFF);
    }
    _strip.show();
    return true;
  }
  return false;
}

bool Neopixel::twinkleLights(uint32_t dt) {
  if(dt >= 150) {
    _strip.clear();
    // Choose random number of pixels to be on at the same time between 1-4
    uint8_t numLightsOn = rand() % 4 + 1;

    for (int i = 0; i < numLightsOn; i++) {
      _strip.setPixelColor(rand() % NEOPIXEL_COUNT, 0xFF);
    }
    _strip.show();
    return true;
  }
  return false;
}

bool Neopixel::strobeRand(uint32_t dt) {
  static bool on;
  uint8_t light_on_interval;
  uint16_t light_off_interval;

  if (!dt) {
    on = false;
  }

  if (on) {
    light_on_interval = rand() % 50 + 50;
    light_off_interval = rand() % 350 + 50;

    if (dt >= light_off_interval) {
          _fill(0xFFFFFFFF);
          _strip.show();
          on = !on;
          return true;
    }
  } else {
    if (dt >= light_on_interval) {
          _strip.clear();
          _strip.show();
          on = !on;
          return true;
    }
  }

  return false;
}

bool Neopixel::rotatingSlowly(uint32_t dt) {
  static uint8_t offset = 0;

  if (dt >= 100) {
    for (uint8_t pix = 0; pix < NEOPIXEL_COUNT; pix++) {
      _strip.setPixelColor((pix + offset) % NEOPIXEL_COUNT, 0, 0, 0, pix < NEOPIXEL_COUNT / 2 ? 0xFF : 0);
    }
    _strip.show();
    offset++;
    return true;
  }
  return false;
}

bool Neopixel::quadRand(uint32_t dt) {
  uint16_t i;
  uint8_t R, G;

  if(dt >= 250){
    uint8_t rand = random(4);
    for(i = 0; i < 16; i++) {
      R = (i / 4 == rand) ? 255 : 0;
      G = (i / 4 == rand) ? 165 : 0;
      _strip.setPixelColor(i, R, G, 0);
    }
    _strip.show();
    return true;
  }
  return false;
}

bool Neopixel::fire(uint32_t dt) {
  uint8_t R, G;

  if(dt >= 600){
    for(uint8_t i = 0; i < 16; i++){
      if(!random(4)){
        R = 255;
        G = random(155);
      }else{
        R = G = 0;
      }
      _strip.setPixelColor(i, _strip.Color(R, G, 0));
    }
    _strip.show();
    return true;
  }
  return false;
}

//requires the rain function to run
byte rainDropState[16];
byte rainShineState[16];
byte rainDropVolume[16];
byte rainShineVolume[16];

void Rain(byte* rds, byte* rss, byte* rdv, byte* rsv) {
  uint16_t i;

  if (*rds == 0) {
    if (random(16) == 1) {
      *rds = 1;
    }
  }

  if (*rds == 1){
    i = random(120);
    if ((i + *rdv) >= 250) {
      *rdv = 250;
      *rds = 2;
      *rss = 1;
    } else {
      *rdv = *rdv + i;
    }
  } else if (*rds == 2) {
    i = random(20);
    if ((*rdv - i) <= 0) {
      *rdv = 0;
      *rds = 0;
    } else {
      *rdv = *rdv - i;
    }

    if (*rss == 1) {
      i = random(150);
      if ((i + *rsv) >= 250) {
        *rsv = 250;
        *rss = 2;
      } else {
        *rsv = *rsv + i;
      }
    } else if (*rss == 2) {
      i = random(80);
      if ((*rsv - i) <= 0) {
        *rsv = 0;
        *rss = 0;
      } else {
        *rsv = *rsv - i;
      }
    }
  }
}

bool Neopixel::rainCycle(uint32_t dt) {
  uint16_t i;


  if(dt >= 150){
    for(i = 0; i < 16; i++){
      Rain((rainDropState + i), (rainShineState + i), (rainDropVolume + i), (rainShineVolume + i));
    }

    for(i = 0; i < 16; i++){
      _strip.setPixelColor(i, _strip.Color(0, 0, *(rainDropVolume + i), *(rainShineVolume + i)));
    }

    _strip.show();
    return true;
  }
  return false;
}

bool Neopixel::flashing(uint32_t dt) {
  static bool white = false;

  if (dt >= 150) {
    if (white) {
      _fill(_strip.Color(rand() % 256, rand() % 256, rand() % 256));
    } else {
      _fill(_strip.Color(0, 0, 0, 255));
    }
    white = !white;
    _strip.show();
    return true;
  }
  return false;
}

bool Neopixel::bell2(uint32_t dt) {
  static bool yellow = false;

  if (dt >= 400) {
    if (yellow) {
      _fill(_strip.Color(255, 255, 0));
    } else {
      _strip.clear();
    }
    yellow = !yellow;
    _strip.show();
    return true;
  }

  return false;
}
