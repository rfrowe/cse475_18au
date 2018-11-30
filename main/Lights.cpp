#include "Neopixel.h"

uint32_t rainbowColors[NEOPIXEL_COUNT] = {
  16711680, 13578240, 10444800, 7311360, 4177920, 1044480, 56865, 44625, 32385,
  20145, 7905, 1179885, 4325565, 7471245, 10616925, 13762605
};

bool Neopixel::rainbow(uint32_t dt) {
  static uint8_t offset = 0;

  if (dt > 50) {
    for (uint8_t pix = 0; pix < NEOPIXEL_COUNT; pix++) {
      _strip.setPixelColor((pix + offset) % NEOPIXEL_COUNT, rainbowColors[pix]);
    }
    _strip.show();
    offset++;
    return true;
  }
  return false;
}

bool Neopixel::bell(uint32_t dt) {
  static uint8_t offset = 0;

  if (!dt || dt > 150) {
    uint8_t color;

    for (uint8_t pix = 0; pix < NEOPIXEL_COUNT; pix++) {
      color = (sin(pix + offset) * 127 + 128) / 255;
      _strip.setPixelColor(pix, color, color, 0);
    }

    _strip.show();
    offset++;
    return true;
  }
  return false;
}

bool Neopixel::strobe(uint32_t dt) {
  static uint8_t strobes_remaining = 0;
  static bool strobing = false;

  if (!dt) {
    strobes_remaining = 10;
  }

  if (strobes_remaining > 0 && !dt || dt > 50) {
    if (strobing) {
      _strip.clear();
      strobes_remaining--;
    } else {
      _fill(_strip.Color(0, 0xFF, 0xFF));
    }
    strobing = !strobing;
    _strip.show();
    return true;
  }

  return false;
}

bool Neopixel::melodic(uint32_t dt) {
  static bool on = false;
  static uint8_t offset = 0;

  if (dt >= 200) {
    if (on) {
      _strip.clear();
      offset = (offset + 1) % 3;
    } else {
      for (uint8_t pix = 0; pix < NEOPIXEL_COUNT - offset; pix += 3) {
        _strip.setPixelColor(pix + offset, rainbowColors[pix + offset]);
      }
    }
    on = !on;
    _strip.show();

    return true;
  }

  return false;
}

bool Neopixel::insects(uint32_t dt) {
  static bool fading = false;
  static bool in = false;

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
    _strip.clear();
    for (uint8_t i = 0; i < 6; i++) {
      _strip.setPixelColor(rand() % 16, 0, 239 + rand() % 32 - 16, 31 + rand() % 32 - 16);
    }
    fading = in = true;
    _strip.show();
  }

  return false;
}

bool Neopixel::wavez(uint32_t dt) {
  static bool fading = false;
  static bool in = false;
  static uint32_t ocean_breeze_blue = _strip.Color(0, 239, 227);

  if (fading) {
    if (dt >= 200) {
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
    // _fill(ocean_breeze_blue);
    for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++) {
      _strip.setPixelColor(i, ocean_breeze_blue);
    }
    fading = in = true;
    _strip.show();
  }

  return false;
}


bool Neopixel::lightning(uint32_t dt)
{
	if (dt >= 75) // Original Delay 500
	{
		uint8_t brightness = 255;
		for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++)
		{
			_strip.setPixelColor(i, brightness, brightness, brightness, brightness);
		}
		_strip.show();
		delay(50);
		brightness = 0;
		for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++)
		{
			_strip.setPixelColor(i, brightness, brightness, brightness, brightness);
		}
		_strip.show();
    return true;
	}
  return false;
}

// Custom Ambient 3
bool Neopixel::rotating(uint32_t dt)
{
	static uint8_t loopCounter = 0;
	if (dt >= 200) // Original Delay 200
	{
		for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++)
		{
			uint8_t brightness = 0;
			if ((i + loopCounter) % 4 == 0)
			{
				brightness = 255;
			}
			_strip.setPixelColor(i, brightness, brightness, brightness, brightness);
		}
		_strip.show();

		loopCounter++;
		if (loopCounter == NEOPIXEL_COUNT)
		{
			loopCounter = 0;
		}
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
    // _strip.setPixelColor(i%num,Wheel(((i * 256 / _strip.numPixels()) + i) & 255));
    // _strip.setPixelColor((i+1)%num,Wheel(((i * 256 / _strip.numPixels()) + i) & 255));
    // _strip.setPixelColor((i+2)%num,Wheel(((i * 256 / _strip.numPixels()) + i) & 255));
    // _strip.setPixelColor((i+3)%num,Wheel(((i * 256 / _strip.numPixels()) + i) & 255));
    // _strip.setPixelColor((i+4)%num,Wheel(((i * 256 / _strip.numPixels()) + i) & 255));
    _strip.show();
    offset++;
    rand_val = random(25, 200);
    return true;
  }

  return false;
}

// bird lights
bool Neopixel::blinkDot(uint32_t dt) {
  if (dt >= 50) {
    for (int i = 0; i < _strip.numPixels(); i++) {
      _strip.setPixelColor(i, _strip.Color(0, 0, 0));
    }
    for (int i = 0; i < _strip.numPixels(); i++){
      if (random(10) <= 1) _strip.setPixelColor(i, _strip.Color(0, 0, 255));
    }
    _strip.setBrightness(5);
    _strip.show();
    return true;
  }
  return false;
}


// seashore lights
bool Neopixel::breathCycle(uint32_t dt){
  static uint8_t offset = 0;
  static boolean increasingFlag = true;
  if (dt >= 25) {
    for (int i = 0; i < _strip.numPixels(); i++) {
      _strip.setPixelColor(i, _strip.Color(0, 0, 255));
    }
    _strip.setBrightness(offset / 10);
    _strip.show();
    if (increasingFlag) {
      offset ++;
    } else{
      offset --;
    }
    if (offset == 50 || offset == 0){
      increasingFlag = !increasingFlag;
    }
    return true;
  }
  return false;
}

bool Neopixel::twinkle_lights(uint32_t dt) {
  //Set brightness to 15?
  _strip.setBrightness(15);
  //Delay between changes?
  if(dt >= 200) {
    // First turn off all lights
    for (uint16_t i = 0; i < NEOPIXEL_COUNT; i++) { _strip.setPixelColor(i, 0,0,0,0); _strip.show();}
    // Choose random number of pixels to be on at the same time between 1-4
    int numLightsOn = rand() % 4 + 1;

    for (int i = 0; i<numLightsOn; i++) {
      //For each pixel that's on, choose which pixel it is from 0-15
      uint16_t pixelNum = rand() % NEOPIXEL_COUNT;
      //Set the color of the pixel to blue
      _strip.setPixelColor(pixelNum, 0, 0, 255, 0);
    }
    _strip.show();
    return true;
  }
  return false;
}

bool Neopixel::lightning2(uint32_t dt) {
  _strip.setBrightness(15);
  Serial.println(dt);
  static bool on = false;
  int light_on_interval = rand() % 50 + 50; //randon number between 50 to 99
  int light_off_interval = rand() % 350 + 50; //randon number between 50 to 399
  if (on) {
    if (dt >= light_on_interval) {
          _fill(_strip.Color(0, 0, 0, 127));
          _strip.show();
          on = !on;
          return true;
    }
  } else {
    if (dt > light_off_interval) {
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

bool Neopixel::quadRand(uint32_t dt){
  uint16_t i;
  uint8_t R, G, B;



  if(dt >= 250){
    //tdt = 0;
    uint8_t rand = random(4);
    B = 0;
    for(i = 0; i < 16; i++) {
      switch (rand) {
        case 0:
          if (i >= 0 && i < 4) {
            R = 255;
            G = 165;
          } else {
            R = 0;
            G = 0;
          }
          break;
        case 1:
          if (i >= 4 && i < 8) {
            R = 255;
            G = 165;
          } else {
            R = 0;
            G = 0;
          }
          break;
        case 2:
          if (i >= 8 && i < 12) {
            R = 255;
            G = 165;
          } else {
            R = 0;
            G = 0;
          }
          break;
        case 3:
          if (i >= 12 && i < 16) {
            R = 255;
            G = 165;
          } else {
            R = 0;
            G = 0;
          }
          break;
        default:
          break;
      }
      _strip.setPixelColor(i, _strip.Color(R, G, B));
    }
    _strip.show();
    return true;
  }
  return false;
}

bool Neopixel::fire(uint32_t dt){
  uint16_t i;
  byte R, G;

  //tdt += dt;

  if(dt >= 100){
    //tdt = 0;
    for(i = 0; i < 16; i++){
      if(random(4) == 0){
        R = 255;
        G = random(155);
      }else{
        R = 0;
        G = 0;
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

  if(*rds == 0){
    if(random(16) == 1){
      *rds = 1;
    }
  }

  if(*rds == 1){
    i = random(120);
    if((i + *rdv) >= 250){
      *rdv = 250;
      *rds = 2;
      *rss = 1;
    }else{
      *rdv = *rdv + i;
    }
  }else if(*rds == 2){
    i = random(20);
    if((*rdv - i) <= 0){
      *rdv = 0;
      *rds = 0;
    }else{
      *rdv = *rdv - i;
    }

    if(*rss == 1){
      i = random(150);
      if((i + *rsv) >= 250){
        *rsv = 250;
        *rss = 2;
      }else{
        *rsv = *rsv + i;
      }
    }else if(*rss == 2){
      i = random(80);
      if((*rsv - i) <= 0){
        *rsv = 0;
        *rss = 0;
      }else{
        *rsv = *rsv - i;
      }
    }
  }
}

bool Neopixel::rainCycle(uint32_t dt) {
  uint16_t i;

  //tdt += dt;

  if(dt >= 150){
    //tdt = 0;
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

// phone visual
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

//
// // Input a value 0 to 255 to get a color value.
// // The colours are a transition r - g - b - back to r.
// uint32_t Neopixel::Wheel(byte WheelPos) {
//   WheelPos = 255 - WheelPos;
//   if (WheelPos < 85) {
//     return _strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//   }
//   if (WheelPos < 170) {
//     WheelPos -= 85;
//     return _strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//   }
//   WheelPos -= 170;
//   return _strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
// }
//
// // Slightly different, this makes the rainbow equally distributed throughout
// void Neopixel::rainbowCycle(uint32_t wait) {
//   uint16_t i, j;
//
//   for (j = 0; j < 256 * 4; j++) { // n cycles of all colors on wheel
//     for (i = 0; i < _strip.numPixels(); i++) {
//       _strip.setPixelColor(i, Wheel(((i * 256 / _strip.numPixels()) + j) & 255));
//     }
//     _strip.show();
//     delay(wait);
//   }
// }
