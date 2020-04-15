#include <RGBmatrixPanel.h> // Hardware-specific library

//#define CLK  8   // USE THIS ON ARDUINO UNO, ADAFRUIT METRO M0, etc.
#define CLK A4 // USE THIS ON METRO M4 (not M0)
//#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

#define FPS 60         // Maximum frames-per-second
//#define DEBUG 1
#define N_ITER 255
uint32_t prevTime = 0; // For frame-to-frame interval timing

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);
float g_ox, g_oy, g_sx, g_sy;

void update(float ox, float oy, float sx, float sy);
void update_origin(void);

void setup() {
  // put your setup code here, to run once:
  matrix.begin();
  Serial.begin(115200);
  randomSeed(analogRead(8));
  update_origin();
}

void update_origin(void){
  float r, theta;
//  theta = (float)random(0,1e6) / 1e6 * TWO_PI;
//  r = (1.0f - cos(theta)) / 2.0f;
//  g_ox = r * cos(theta) + 0.25;
//  g_oy = r * sin(theta);
  g_ox = (float)random(0,1e6)/1e6 + 1.0f;
  g_oy = 0.0f;
  g_sx = 3.0f; g_sy = 2.5f;
}

void update(float ox, float oy, float sx, float sy) {
  float zr, zc, cr, cc;
  float zr2, zc2;
  int i, j, n;
  int max_cnt, one_cnt;

  max_cnt = 0;
  one_cnt = 0;
  for(i=0; i<32; i++){
    for(j=0; j<16; j++){
      cr = ((float)(i-16) * sx / 32.0f) - ox;
      cc = ((float)(j-8) * sy / 16.0f) - oy;
#ifdef DEBUG
      Serial.print("C=");
      Serial.print(cr, 4);
      Serial.print(" + ");
      Serial.print(cc, 4);
      Serial.print("i\n");
#endif
      zr = 0.0;
      zc = 0.0;
      zr2 = 0.0;
      zc2 = 0.0;
      for(n=0; n<N_ITER; n++) {
        zr += zr2 - zc2 + cr;
        zc += 2 * zr * zc + cc;
        zr2 = zr * zr;
        zc2 = zc * zc;
#ifdef DEBUG
        Serial.print("N=");
        Serial.print(n);
        Serial.print(" z2 = ");
        Serial.print(zr2 + zc2);
        Serial.print("\n");
#endif
        if ((zr2 + zc2) > 4.0f){
          break;
        }
      }
      if(n == N_ITER){
        max_cnt++;
        matrix.drawPixel(i, j, matrix.ColorHSV(0, 255, 0, true));
      }
      else{
        if(n < 8 ){
          one_cnt++;
        }
        matrix.drawPixel(i, j, matrix.ColorHSV(n, 255, 255, true));
      }
    }
  }

  if((max_cnt >= (16*32)) || (one_cnt >= (16*32)) || (g_sx < 1e-3)){
    update_origin();
  }
}

void loop() {
  //update(g_ox, g_oy, 3.0f, 2.5f);
  g_sx *= 0.995;
  g_sy *= 0.995;
  update(g_ox, g_oy, g_sx, g_sy);
  uint32_t t;
  while(((t = millis()) - prevTime) < (1000 / FPS));
  prevTime = t;

  matrix.swapBuffers(false);
}
