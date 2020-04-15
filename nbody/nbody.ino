#include <RGBmatrixPanel.h> // Hardware-specific library

//#define CLK  8   // USE THIS ON ARDUINO UNO, ADAFRUIT METRO M0, etc.
#define CLK A4 // USE THIS ON METRO M4 (not M0)
//#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

#define FPS 100         // Maximum frames-per-second
#define N_PARTICLES 10
#define ORIGIN_X 16
#define ORIGIN_Y 8
#define G 100.0f
uint32_t prevTime = 0; // For frame-to-frame interval timing

long hue[32][16];
float value[32][16];

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);
typedef struct particle {
  float x,y,z,ux, uy, uz, ax, ay, az;
  float m;
}particle_t;

particle_t g_particles[N_PARTICLES];

void update(float dt);
void seed_particle(particle_t &p);
bool on_screen(int i, int j);

void setup() {
  for(int i=0; i<32; i++){
    for(int j=0; j<16; j++){
      value[i][j] = 0.0f;
      hue[i][j] = 0;
    }
  }

  for(int i=0; i<N_PARTICLES; i++){
    seed_particle(g_particles[i]);
  }
  
  // put your setup code here, to run once:
  matrix.begin();
}

void seed_particle(particle_t &p){
    p.x = random(0, 3e6)/1e5 - 15.0f;
    p.y = random(0, 1.4e6)/1e5 - 7.0f;
    p.z = random(0, 1e6)/1e5 - 5.0f;

    p.ux = random(0, 1e6)/1e4 - 50.0f;
    p.uy = random(0, 1e6)/1e4 - 50.0f;
    p.uz = random(0, 1e6)/1e4 - 50.0f;

    p.ax = 0.0f;
    p.ay = 0.0f;
    p.az = 0.0f;

    p.m = random(1, 1e4)/10;
}

bool on_screen(int i, int j)
{
  if((i < 0) || (i > 31)){
    return false;
  }else if((j < 0) || (j > 15)){
    return false;
  }
  return true;
}

void update(float dt) {
  int i, j, k;
  float x, y, z, r2, rx, ry, rz, m1_i, m2, rmag;
  particle_t *p;
  
  for(i=0; i<N_PARTICLES;i++){
    p = &g_particles[i];
    p->ax = 0.0f;
    p->ay = 0.0f;
    p->az = 0.0f;
    x = p->x;
    y = p->y;
    z = p->z;
    m1_i = 1.0f / p->m;

    // TODO: Big optimization possible here -
    // Instead of N^2 iteration, we can compute pair-wise
    // force components and then compute accelerations based on sum
    // of forces. This will result in n*(n-1)/2 evals instead
    // of n^2
    for(j=0; j<N_PARTICLES; j++){
      if(i == j){
        continue;
      }
      rx = (g_particles[j].x - x);
      ry = (g_particles[j].y - y);
      rz = (g_particles[j].z - z);
      m2 = g_particles[j].m;
      r2 = rx * rx + ry * ry + rz * rz;
      //rmag = sqrt(r2);
      
      // G * m1 * m2 * rx / |r| / r^2
      p->ax += G * m2 * rx / (r2 + 1);
      p->ay += G * m2 * ry / (r2 + 1);
      p->az += G * m2 * rz / (r2 + 1);
    }
    p->ax *= m1_i;
    p->ay *= m1_i;
    p->az *= m1_i;
    p->ux += p->ax * dt;
    p->uy += p->ay * dt;
    p->uz += p->az * dt;

    p->x += p->ux * dt;
    p->y += p->uy * dt;
    p->z += p->uz * dt;
  }

  for(i=0; i<32; i++){
    for(j=0; j<16; j++){
      value[i][j] *= 0.99;
      matrix.drawPixel(i, j, matrix.ColorHSV(hue[i][j], 100, (int)value[i][j], true));
      //matrix.drawPixel(i, j, matrix.ColorHSV(hue[i][j], 255, 0, true));
    }
  }
  
  for(k=0; k<N_PARTICLES; k++){
    p = &g_particles[k];
    i = (int)p->x + ORIGIN_X;
    j = (int)p->y + ORIGIN_Y;
    if(on_screen(i, j)){
      value[i][j] = 255;
      m2 = p->uz;
      if(m2 > 0){
        hue[i][j] = 0;
        matrix.drawPixel(i, j, matrix.ColorHSV(0, 0.01*m2*m2, 255, true));
      }else {
        hue[i][j] = 800;
        matrix.drawPixel(i, j, matrix.ColorHSV(800, 0.01*m2*m2, 255, true));
      }
    } 
  }
  
}

void loop() {
  update(0.001);
  
  uint32_t t;
  while(((t = millis()) - prevTime) < (1000 / FPS));
  prevTime = t;

  matrix.swapBuffers(false);
}
