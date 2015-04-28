int analogPin = 0; // read from multiplexer using analog input 0
int strobePin = 5; // strobe is attached to digital pin 2
int resetPin = A1; // reset is attached to digital pin 3
int spectrumValue[7]; // to hold a2d values

#include "SPI.h"
#include "Cube.h"

Cube cube;
const int frames = 50;
int frame = 0;
int bass[frames];
int snare[frames];
int hat[frames];
int test[frames];
long previousMillis = 0;
long p1 = 0;
long p2 = 0;
long p3 = 0;
long interval = 150, timer = 10000;
boolean change = false, change2 = false;

void setup()
{
  //Serial.begin(9600);
  cube.begin(0, 115200);
  pinMode(analogPin, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  analogReference(DEFAULT);
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
}

void loop()
{
  unsigned long currentMillis = millis();
  readEq();
  int new_bass = spectrumValue[0] + spectrumValue[1];
  int new_snare = spectrumValue[3] + spectrumValue[2];
  int new_hat = spectrumValue[6] + spectrumValue[5];

  if (currentMillis - p1 > timer) {
    change = !change;
    if(change)
      change2 = !change2;
    p1 = currentMillis;
  }
  if ((new_snare / find_average(snare, frames)) > 1) {
    if (change) {
      cube.setplane(Y, 3, RED);
    } else if(change2){
      cube.setplane(X, 3, PINK);
    } else {
      cube.setplane(Z, 3, RED);
    }
  }
  if ((new_hat / find_average(hat, frames)) > 1) {
    if (change) {
      cube.setplane(Y, 2, YELLOW);
      cube.setplane(Y, 1, YELLOW);
    } else if(change2){
      cube.setplane(X, 2, ORANGE);
      cube.setplane(X, 1, ORANGE);
    } else {
      cube.setplane(Z, 2, YELLOW);
      cube.setplane(Z, 1, YELLOW);
    }
  }
  if ((new_bass / find_average(bass, frames)) > 1) {
    if (change) {
      cube.setplane(Y, 0, GREEN);
    } else if(change2){
      cube.setplane(X, 0, PURPLE);
    } else {
      cube.setplane(Z, 0, GREEN);
    }
  }
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    cube.all(BLACK);
  }
  hat[frame] = new_hat;
  snare[frame] = new_snare;
  bass[frame] = new_bass;
  frame++;
  if (frame >= frames) frame = 0;
}

int find_average(int ary[], int siz) {
  double sum = 0;
  for (int i = 0; i < siz; i++) {
    sum += ary[i];
  }
  return sum / siz;
}

void readEq() {
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    spectrumValue[i] = analogRead(analogPin);
    digitalWrite(strobePin, HIGH);
  }
}
