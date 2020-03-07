#include <Arduino.h>

#include <fix_fft.h>
#include <stdio.h>
#include <math.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64

#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define M 7

#define SAMPLES 128

char vReal[SAMPLES];
char vImag[SAMPLES];
const double samplingFrequency = 10000; 
unsigned int sampling_period_us;
unsigned long microseconds, timeFFT;

void writeInfo();

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //display.display();

  analogReference(DEFAULT);
  sampling_period_us = round(1000000*(1.0/samplingFrequency));
  
  delay(1000);
}

void loop() {
  microseconds = micros();
  for(int i=0; i<SAMPLES; i++)
  {
      vReal[i] = analogRead(A0)*128/1024;
      vImag[i] = 0;
      while(micros() - microseconds < sampling_period_us){
        //empty loop
      }
      microseconds += sampling_period_us;
  }

  timeFFT = micros();
  fix_fft(vReal, vImag, M, false);
  fix_fft(vReal, vImag, M, true);
  timeFFT = micros() - timeFFT;

  display.clearDisplay();

  writeInfo();

  display.display();
}

void writeInfo()
{
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.print("FFT/FIR size=");
  display.println(SAMPLES);
  display.print("BW=");
  display.print((int)(samplingFrequency/2));
  display.print("kHz");

  display.setCursor(0,24);
  display.println("Filter calc time:");
  display.println(" ");
  display.print("FFT :");
  display.print(timeFFT);
  display.println("uS");
}

