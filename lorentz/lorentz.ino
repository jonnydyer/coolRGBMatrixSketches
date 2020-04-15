#include <RGBmatrixPanel.h> // Hardware-specific library

//#define CLK  8   // USE THIS ON ARDUINO UNO, ADAFRUIT METRO M0, etc.
#define CLK A4 // USE THIS ON METRO M4 (not M0)
//#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

#define RHO 28.0f
#define SIGMA 10.0f
#define BETA 8.0f / 3.0f

#define FPS 300         // Maximum frames-per-second
uint32_t prevTime = 0; // For frame-to-frame interval timing

int hue[32][16];
float value[32][16];

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
float x, y, z, t;

void update(float dt);

void setup() {
  x = 1.0f;
  y = 1.0f;
  z = 1.0f;
  t = 0.0f;
  for(int i=0; i<32; i++){
    for(int j=0; j<16; j++){
      hue[i][j] = 0.0f;
      value[i][j] = 0.0f;
    }
  }
  
  // put your setup code here, to run once:
  matrix.begin();
}

void update(float dt) {
  static float x1, y1, z1, dx, dy, dz;
  int i, j;
  dx = SIGMA * (y - x);
  dy = x * (RHO - z) - y;
  dz = x * y - BETA * z;

  x1 += dt * dx;
  y1 += dt * dy;
  z1 += dt * dz;
  t += dt;

  i = (int)(0.9*x1 + 15.0);
  if(i > 31) {
    i = 31;
  }
  if(i < 0) {
    i = 0;
  }
  j = (int)(z * 0.33f);
  if(j > 15) {
    j = 15;
  }
  if(j < 0) {
    j = 0;
  }
  
  hue[i][j] = (int)((y1 + 30.0f) * 7.0f + 5.0f*t);
  value[i][j] = 255;

  for(i=0; i<32; i++){
    for(j=0; j<16; j++){
      value[i][j] *= 0.9995;
      matrix.drawPixel(i, j, matrix.ColorHSV(hue[i][j], 255, (int)value[i][j], true));
    }
  }
  
  //matrix.drawLine((uint8_t)(0.9*x + 15.0), (uint8_t)(z * 0.33f), 
  //               (uint8_t)(0.9*x1 + 15.0), (uint8_t)(z1 * 0.33f), 
  //                matrix.ColorHSV((int)((y1 + 30.0f) * t), 255, 255, true));
  x = x1;
  y = y1;
  z = z1;
}

void loop() {
  update(0.005);
  
  uint32_t t;
  while(((t = millis()) - prevTime) < (1000 / FPS));
  prevTime = t;

  matrix.swapBuffers(false);
}
