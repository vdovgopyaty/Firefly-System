#include <Adafruit_NeoPixel.h>
#include <AsyncStream.h>
#include <GParser.h>

#define PIN 10        // пин для подключения ленты
#define NUMPIXELS 42  // количество программируемых пикселей
#define BUNDLESIZE 6  // количество пикселей в пачке (всего пикселей NUMPIXELS * BUNDLESIZE)

AsyncStream<NUMPIXELS * 12> serial(&Serial, ';'); // по 12 символов на каждый пиксель
Adafruit_NeoPixel strip(NUMPIXELS * BUNDLESIZE, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(1000000); // Baud Rate
  Serial.setTimeout(1000);
  
  while(!Serial) {
    // ждем подключения на Serial-порту
  }
  
  strip.begin();            // инициализируем ленту
  strip.setBrightness(200); // яркость светодиодов (максимум 255)
  
  Serial.println("Waiting for RGB-data...");
}

// Протокол - RGB (0..255, 0..255, 0..255)
// Формат данных - R1 G1  B1  R2  G2  B2  ..  RN  GN  BN;

void loop() {
  if (serial.available()) {
    char* str = serial.buf;
    
    GParser parser(str, '\t');  // разделитель - tab
    int rgbData[NUMPIXELS * 3]; // 3 светодиода на пиксель
    int len = parser.parseInts(rgbData);
    
    if (len == NUMPIXELS * 3) { // 3 светодиода на пиксель
      for (int i = 0; i < NUMPIXELS; i++) {
        if (BUNDLESIZE > 1) {
          for (int j = 0; j < BUNDLESIZE; j++) {
            strip.setPixelColor(i*BUNDLESIZE+j, strip.Color(rgbData[i*3], rgbData[i*3+1], rgbData[i*3+2]));
          }
        } else {
          strip.setPixelColor(i, strip.Color(rgbData[i*3], rgbData[i*3+1], rgbData[i*3+2]));
        }
      }
      
      strip.show();
    } else {
      Serial.println("ERROR");
    }
  }
}
