/* Written by JelleWho https://github.com/jellewie */
void UpdateAndShowClock() {
  //  if (AnimationCounter > 0) {     //Animation needs to be shown
  //    ShowAnimation(false);
  //  } else
  {
    //==============================
    //Update the internal time clock
    //==============================
    while (TimeCurrent.Ticks + 1000 <= millis()) {
#ifdef TimeExtra_SerialEnabled
      Serial.println("TM: Time= " + String(TimeCurrent.HH) + ":" + String(TimeCurrent.MM) + ":" + String(TimeCurrent.SS) + " " + String(TimeCurrent.Ticks) + " now=" + String(millis()));
#endif //TimeExtra_SerialEnabled
      TimeCurrent.Ticks += 1000;
      TimeCurrent.SS++;
      if (TimeCurrent.SS > 59) {
        TimeCurrent.SS = 0;
        TimeCurrent.MM++;
      }
      if (TimeCurrent.MM > 59) {
        TimeCurrent.MM = 0;
        TimeCurrent.HH++;
        if (DoHourlyAnimation) {
#ifdef Time_SerialEnabled
          Serial.println("TM: Start Hourly Animation");
#endif //Time_SerialEnabled
          StartAnimation(random(0, TotalAnimations), AnimationCounterTime); //Start a random Animation
          return;
        }
      }
      if (TimeCurrent.HH > 24)
        TimeCurrent.HH = 0;
      static bool TimeFlag = false;
      if (TimeCurrent.HH == 4 and !TimeFlag) {
        TimeFlag = true;
        if (!UpdateTime())                    //Get a new sync timestamp from the server
          WiFiManager_connected = false;
      } else
        TimeFlag = false;
    }
    //==============================
    //Show the time on the LEDs if needed
    //==============================
    static byte LastSec = TimeCurrent.SS;     //Store 'second' as an 'update already done' state. so if the seconds counter changes we update and else we skip updating
    if (LastSec != TimeCurrent.SS) {
      LastSec = TimeCurrent.SS;
      FastLED.clear();
      LEDs[LEDtoPosition(TimeCurrent.SS)] += CRGB(0, 0, 255);
      LEDs[LEDtoPosition(TimeCurrent.MM)] += CRGB(0, 0, 255);
      LEDs[LEDtoPosition(TimeCurrent.HH * 5)] += CRGB(0, 0, 255);
      UpdateLEDs = true;
    }
  }
}

byte LEDtoPosition(byte LEDID) {
  //Takes ClockOffset into account, so you can say turn LED 0 on (top of the CLOCK) and it will convert it to be the top LED
  //Basicly adding ClockOffset to the LED and wrapping LEDS around
  LEDID += ClockOffset;
  while (LEDID >= TotalLEDs)
    LEDID -= TotalLEDs;
  return LEDID;
}

bool UpdateTime() {
  if (!WiFiManager_connected) return false;   //If WIFI not connected, stop right away
#ifdef Time_SerialEnabled
  Serial.println("TM: Get server time");
#endif //Time_SerialEnabled
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
#ifdef Time_SerialEnabled
    Serial.println("TM: Failed to obtain time");
#endif //Time_SerialEnabled
    return false;
  } else {
#ifdef Time_SerialEnabled
    Serial.println(&timeinfo, "TM: %A, %B %d %Y %H:%M:%S");
#endif //Time_SerialEnabled
    TimeCurrent.Ticks = millis();
    TimeCurrent.HH = timeinfo.tm_hour;
    TimeCurrent.MM = timeinfo.tm_min;
    TimeCurrent.SS = timeinfo.tm_sec;
  }
  return true;
}