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

#define SAMPLES 128

char vReal[SAMPLES];
char vImag[SAMPLES];
const double samplingFrequency = 10000;
unsigned int sampling_period_us;
unsigned long microseconds, timeFFT, timeFIR;
//int m;

const char PROGMEM window[SAMPLES] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1};

void writeInfo(int n);
void multWindow(int n);
void FFTfilter(int n);
void convolve(int n);
void runAnalysis();

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //display.display();

  analogReference(DEFAULT);
  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));

  delay(1000);
}

void loop()
{
  microseconds = micros();
  for (int i = 0; i < SAMPLES; i++)
  {
    vReal[i] = analogRead(A0) * 128 / 1024;
    vImag[i] = 0;
    while (micros() - microseconds < sampling_period_us)
    {
      //empty loop
    }
    microseconds += sampling_period_us;
  }

  runAnalysis();
}

void writeInfo(int n)
{
  const int val[7] = {2, 4, 8, 16, 32, 64, 128};

  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.print("FFT/FIR size=");
  display.println(val[n]);
  display.print("BW=");
  display.print((int)(samplingFrequency / 2));
  display.print("kHz");
  display.print(" ");
  display.print("uC:UNO");

  display.setCursor(0, 24);
  display.println("Filter calc time:");
  display.println(" ");
  display.print("FFT:");
  display.print(timeFFT);
  display.println("uS");
  display.print("FIR:");
  display.print(timeFIR);
  display.println("uS");

  display.print("WINNER:");
  display.print(timeFFT < timeFIR ? "FFT " : "FIR ");
  display.print("d:");
  display.print(timeFFT > timeFIR ? (timeFFT - timeFIR) : (timeFIR - timeFFT));
  display.print("uS");
}

void multWindow(int n)
{
  for (int i = 0; i < (int)(pow(2, n)); i++)
  {
    vReal[i] *= window[i];
    vImag[i] *= window[i];
  }
}

void FFTfilter(int n)
{
  fix_fft(vReal, vImag, n, false);
  multWindow(n);
  fix_fft(vReal, vImag, n, true);
}

void convolve(int n)
{
  int tempR = 0, tempI = 0;

  for (int sig = 0; sig < (int)(pow(2, n)); sig++)
  {
    for (int win = 0; win < (int)(pow(2, n)); win++)
    {
      tempR += (vReal[sig] * window[win]);
      tempI += (vImag[sig] * window[win]);
    }

    vReal[sig] = tempR;
    vImag[sig] = tempI;
  }
}

void runAnalysis()
{
  for (int i = 0; i < 7; i++)
  {
    timeFFT = micros();
    FFTfilter(i+1);
    timeFFT = micros() - timeFFT;

    timeFIR = micros();
    convolve(i+1);
    timeFIR = micros() - timeFIR;

    display.clearDisplay();
    writeInfo(i);
    display.display();

    delay(500);
  }
}
