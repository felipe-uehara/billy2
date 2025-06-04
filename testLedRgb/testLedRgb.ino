/*
VSS/GND - amarelo
DIN - vermelho
VDD - preto
DOUT - azul
*/

/*#include <FastLED.h>
int led = 23;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //analogWrite(led, 255);
  /*for (int i=50;; i++){
    analogWrite(led, i);
    delay(500);
  }*/

 /* analogWrite(led, 0);
  delay(1000);
  analogWrite(led, 100);
  delay(1000);
  analogWrite(led, 150);
  delay(1000);
  analogWrite(led, 200);
  delay(1000);
  analogWrite(led, 255);
  delay(1000);
}*/

#include <Adafruit_NeoPixel.h>

#define LED_PIN 27

#define NUM_LEDS 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {

strip.begin();

strip.show();

}

void loop() {

// Cycle through Red, Green, Blue colors

setPixelColor(255, 0, 0); // Red

delay(1000); // Wait for 1 second

setPixelColor(255,255,255);

delay(1000);

}

void setPixelColor(int red, int green, int blue) {

strip.setPixelColor(0, strip.Color(red, green, blue));

strip.show();

}
