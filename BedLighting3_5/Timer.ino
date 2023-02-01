void TimerTurnOFF()
{
  if (millis() >= shutdownTime && SelectedDigit != 0)
  {
    TurnOnOrOff();
    TimerIsSet = 0;
  }
}

void setTimer()
{
#if DEBUG == 1
  Serial.println("Timer");
#endif
  if (on)
  {
    blinkled();
    blinkled();

#define breathingdelay 2
    bool dimming = brightness > 120;
    int preOffBrightness = brightness;
    code = 0;
    unsigned long minutes = 0;
    unsigned long breathingCountDown = millis() + breathingdelay;
    while (true)
    {
      if (dimming)
      {
        brightness -= 1;
        setLight(hue, saturation, brightness);
        if (brightness < 50)
          dimming = !dimming;
      }
      else
      {
        brightness += 1;
        setLight(hue, saturation, brightness);
        if (brightness == 255)
          dimming = !dimming;
      }

      readIR();
      if (IRupdate)
      {
        CreateNumberWdigit(numpadPressedButton(code), &minutes);
        IRupdate = 0;
      }
      if (code == 15355 || code == 39015 || minutes >= 100)  //    EQ //to break out or turn off timer if no number was pressed
      {
        break;
      }
    }
    blinkled();
    blinkled();
    TimerIsSet = minutes > 0;
    if (!TimerIsSet)
    {
      delay(250);
      blinkled();
    }
    brightness = preOffBrightness;
    shutdownTime = millis() + minutes * 60 * 1000;
#if DEBUG > 0
    Serial.print("Exiting timer with timer set for: ");
    Serial.println(minutes);
#endif
  }
}
