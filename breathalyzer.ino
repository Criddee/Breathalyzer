#include <LiquidCrystal_I2C.h> // Driver Library for the LCD Module
#include "pitch.h"
//#include <Arduino.h>
//#include "AirQuality.h"

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); // Adjust to (0x27,20,4) for 20x4 LCD
//AirQuality sens;
#define buzz_pin 11
#define btn_pin 2
#define sens_pin A0
float avg = 0;
volatile bool exe = false;

void setup(){

  Serial.begin(9600);
  pinMode(sens_pin, INPUT);
  pinMode(btn_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(btn_pin), execute, LOW);
  //sens.init(14);

  // Initiate the LCD and turn on the backlight
  lcd.clear();
  lcd.init();      
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Warmup");
  int in = analogRead(sens_pin);
  int test_sum = 0;
  int min_time = 10;
  for(int time=0; time<60;time++){
    delay(1000);
    int temp = analogRead(sens_pin);
    if(abs(temp-in)<1 && time > min_time)test_sum++;
    if(test_sum > 5) break;
    in = temp;
  }
  calibrate();
  lcd.clear();
}

void execute()
{
  exe = true;
}

void calibrate()
{
  float sum = 0;
  int n = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Calibration");
  for (n; n <= 20; n++)
  {
    lcd.setCursor(0,1);
    float input = analogRead(sens_pin);
    lcd.print(input);
    sum += input;
    delay(200);
  }
  avg = sum / n;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("baseline");
  lcd.setCursor(0,1);
  lcd.print(avg);
  delay(2000);
}

void loop()
{
  lcd.setCursor(0,0);
  lcd.print("ready");
  //lcd.print(sens.slope());
  if (exe){
    Serial.println("execute");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Start");
    tone(buzz_pin,250);
    delay(1000);
    float alc_sum = 0;
    int times = 0;
    int ns = 20;
    float top = 0.0;
    for (times; times < ns; times++)
    {
      lcd.clear();
      float alc = (float)map(analogRead(sens_pin),avg,1023,0,500);
      if(alc < 0)alc = 0.0;
      alc_sum += alc;
      lcd.print(alc);
      if(alc>top) top = alc;
      delay(300);
    }
    noTone(buzz_pin);
    lcd.clear();
    lcd.print("Average");
    lcd.setCursor(10,0);
    lcd.print("Peak");
    lcd.setCursor(0, 1);
    float out_avg = alc_sum / times;
    lcd.print(out_avg);
    lcd.setCursor(10,1);
    lcd.print(top);

    if(out_avg>200){
      melody(buzz_pin);
    }
    while (digitalRead(btn_pin)==HIGH)
    {
      delay(10);
    }
    
    int limit = 5;
    int curr = limit + 1;
    while(curr > limit){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Sensor clearing");
      lcd.setCursor(0,1);
      curr = map(analogRead(sens_pin),avg,1023,0,500);
      lcd.print(curr);
      delay(500);
    }
    exe = false;
    lcd.clear();
  }
  delay(200);
}

void melody(int play_pin){
    for(int n=0; n<5;n++){
        delay(500);
        tone(play_pin, NOTE_FS4);
        delay(500);
        tone(play_pin, NOTE_C4);
    }
    noTone(play_pin);
}