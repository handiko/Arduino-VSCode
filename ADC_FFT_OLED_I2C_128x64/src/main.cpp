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
const double samplingFrequency = 5000; 
unsigned int sampling_period_us;           
unsigned long microseconds;

void drawSpectrum();

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
      vReal[i] = analogRead(A0); //*128/1024;
      vImag[i] = 0;
      while(micros() - microseconds < sampling_period_us){
        //empty loop
      }
      microseconds += sampling_period_us;
  }

  fix_fft(vReal, vImag, M, false);

  display.clearDisplay();

  drawSpectrum();
}

void drawSpectrum()
{
  display.clearDisplay();
  display.setCursor(0,0);

  if(SCREEN_HEIGHT == 64)
  {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
  
    display.print(F("FFT bins="));
    display.print(SAMPLES);

    display.setCursor(0,8);
    display.print(F("Start=0Hz Stop="));
    display.print((int)(samplingFrequency/2));
    display.print(F("Hz"));

    display.setCursor(40,24);
    display.print(F("Spectrum"));
    //display.print(micros() - microseconds);
    //display.print("uS");

    display.setCursor(0,16);
  }

  double dat;
  
  for(int i=0; i<SCREEN_WIDTH; i++)
  {
    if(i%2 == 0)
    {
      dat = sqrt(pow(vReal[i/(SCREEN_WIDTH/(SAMPLES/2))],2) + pow(vImag[i/(SCREEN_WIDTH/(SAMPLES/2))],2));
    }
    else
    {
      dat = ( sqrt(pow(vReal[(i-1)/(SCREEN_WIDTH/(SAMPLES/2))],2) + pow(vImag[(i-1)/(SCREEN_WIDTH/(SAMPLES/2))],2))
              + sqrt(pow(vReal[(i+1)/(SCREEN_WIDTH/(SAMPLES/2))],2) + pow(vImag[(i+1)/(SCREEN_WIDTH/(SAMPLES/2))],2)))
              / 2;
    }

    display.drawLine( i, SCREEN_HEIGHT-1,
                      i, SCREEN_HEIGHT-1 - (int)(20*log10(dat)),
                      WHITE);
  }

  display.display();
}