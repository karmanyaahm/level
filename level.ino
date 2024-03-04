#include <Adafruit_LIS3DH.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 2
#define PIN2 3
#define BRIGHT 40
// brightness / 100

Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(10, PIN2, NEO_GRB + NEO_KHZ800);

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("LIS3DH test!");

  if (! lis.begin(0x19)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  // lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  lis.setDataRate(LIS3DH_DATARATE_200_HZ);
  Serial.print("Data rate set to: ");
  switch (lis.getDataRate()) {
    case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;

    case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  }

  strip.begin();
  strip.setBrightness(BRIGHT);
  strip.show(); // Initialize all pixels to 'off'3
  strip2.begin();
  strip2.setBrightness(BRIGHT);
  strip2.show(); // Initialize all pixels to 'off'
// not 9 cuz iirc its sck, might remember wrong todo
for (int i = 4; i < 9; i++)
  pinMode(i, INPUT_PULLUP);

}

float avg_angle = 0;
void loop() {
  if (digitalRead(4)) {
    delay(40);
    strip.clear();
    strip2.clear();
      strip.show();
  strip2.show();
    return;
  }

  sensors_event_t event;
  lis.getEvent(&event);


  // opposite/adjacent; x/y when finding the angle from the vertical; the chip's axes are flipped

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y);
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
  Serial.print(" \tdeg angle: "); Serial.print(avg_angle);
  Serial.println(" m/s^2 ");

  Serial.println();

if (digitalRead(5)) 
  fillColor(- avg_angle / 2);
else rainbowCycle(1);

  for (int i = 0; i < 4; i++) {
    float angle = atan(event.acceleration.y / event.acceleration.x) * 180. / 3.14;
    avg_angle = avg_angle * .8 + angle * .2;
    delay(10);
  }

}

void fillColor(float n) {

  float pos_n = fabs(n);
  float int_pos_n = int(pos_n);
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if (pos_n < .5) {
      strip.setPixelColor(0, strip.Color(0, 50, 200));
      strip2.setPixelColor(0, strip.Color(0, 50, 200));
          strip.setPixelColor(1, strip.Color(0, 0, 0));
      strip2.setPixelColor(1, strip.Color(0, 0, 0));
      break;
    }

    uint8_t r = 255;
    uint8_t g = 255;
    if (i > int_pos_n) r = g = 0;
    else if (i == int_pos_n) {
      r *= pos_n - int_pos_n;
      g *= pos_n - int_pos_n;
    };
    g = 0;
    if (n != pos_n) {
      uint8_t tmp = r;
      r = g;
      g = tmp;
    }

    strip.setPixelColor(i, strip.Color(r, g, 0));
    strip2.setPixelColor(i, strip.Color(g, r, 0));

  }
  strip.show();
  strip2.show();
}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip2.setPixelColor(i, c);
    strip.show();
    strip2.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
      strip2.setPixelColor(i, Wheel((i + j) & 255));

    }
    strip.show();
    strip2.show();

    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      strip2.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));

    }
    strip.show();
    strip2.show();

    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
        strip2.setPixelColor(i + q, c);  //turn every third pixel on

      }
      strip.show();
      strip2.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
        strip2.setPixelColor(i + q, 0);      //turn every third pixel off

      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
        strip2.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on

      }
      strip.show();
      strip2.show();


      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
        strip2.setPixelColor(i + q, 0);      //turn every third pixel off

      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
