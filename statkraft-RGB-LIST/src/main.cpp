#include <Arduino.h>
#include "led_handler.h"

led_handler led_left;
led_handler led_right;
int last_millis;
bool full_on_timer_start = false;
int full_on_timer = 0;
void setup() {
  Serial.begin(9600);
  analogReadResolution(12);
  led_left.begin(270, 0.1, 50, 1760, 11);
  led_right.begin(270, 0.1, 50, 1760, 10);
  last_millis = millis();
  full_on_timer = millis();
}

void loop() {
  int a0_val = analogRead(A0);
  int a1_val = analogRead(A1);
  //Serial.print("A0: ");
  //Serial.print(a0_val);
  //Serial.print(", A1: ");
  //Serial.println(a1_val);
  if(a1_val < 50 && a0_val < 50) {
    led_left.enable_idle();
    led_right.enable_idle();
    full_on_timer_start = false;
  }//else if(full_on_timer_start && full_on_timer - millis() > 10000) {
  //  led_left.enable_full_on_mode();
  //  led_right.enable_full_on_mode();
  //}
  else {
    if(full_on_timer_start == false) {
        full_on_timer_start = true;
        full_on_timer = millis();
    }
    led_left.disable_idle();
    led_right.disable_idle();
  }
  led_left.progress(analogRead(A0));
  led_right.progress(analogRead(A1));
  
}
