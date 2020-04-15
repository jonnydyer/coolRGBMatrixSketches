#include <RGBmatrixPanel.h> // Hardware-specific library

//#define CLK  8   // USE THIS ON ARDUINO UNO, ADAFRUIT METRO M0, etc.
#define CLK A4 // USE THIS ON METRO M4 (not M0)
//#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

#define FPS 300         // Maximum frames-per-second
#define N_PARTICLES
uint32_t prevTime = 0; // For frame-to-frame interval timing

int hue[32][16];
float value[32][16];

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
typedef struct particle {
  float x,y,z,ux, uy, uz;
  float m;
}particle_t;

particle_t g_particles[N_PARTICLES];

void update(float dt);

void setup() {
  for(int i=0; i<32; i++){
    for(int j=0; j<16; j++){
      hue[i][j] = 0.0f;
      value[i][j] = 0.0f;
    }
  }

  for(int i=0; i<N_PARTICLES; i++){
    g_particles[i].x = random(0, 1e6)/1e5 - 5.0f;
    g_particles[i].y = random(0, 1e6)/1e5 - 5.0f;
    g_particles[i].z = random(0, 1e6)/1e5 - 5.0f;

    g_particles[i].ux = random(0, 1e6)/1e6 - 0.5f;
    g_particles[i].uy = random(0, 1e6)/1e5 - 0.5f;
    g_particles[i].uz = random(0, 1e6)/1e5 - 0.5f;
  }
  
  // put your setup code here, to run once:
  matrix.begin();
}

void update(float dt) {
  int i, j;

  float fx, fy, fz;

  for(n=0; n<N_PARTICLES;n++){
    fx = 0;
    fy = 0;
    fz = 0;
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
