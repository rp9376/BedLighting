void setLight(unsigned int h, unsigned int s, unsigned int b)
{
  getRGB(h, s, b, rgb_colors);
  SetLeds(rgb_colors[0], rgb_colors[1], rgb_colors[2]);
}

void SetLeds(int Rcolor, int Gcolor, int Bcolor)
{
  led.setColor(Rcolor, Gcolor, Bcolor);

#if DEBUG == 1 || DEBUG == 2
  analogWrite(redLed, Rcolor);
  analogWrite(greenLed, Gcolor);
  analogWrite(blueLed, Bcolor);
  
#endif//*/
}

void blinkled()
{
  setLight(hue, saturation, 255);
  delay(20);
  setLight(hue, saturation, 0);
  delay(100);
  setLight(hue, saturation, brightness);
}

void handleRainbow()
{
  switch (rainbow)
    {
      case 1:
        if (millis() > rainbowTime)
        {
          rainbowTime = millis() + rainbowDelayValue;
          addToValueWithRollover(1, &hue, 360);
          update_ = 1;
        }
        break;
      case 2:
        if (millis() > rainbowTime)
        {
          rainbowTime = millis() + rainbowDelayValue;
          addToValueWithRollover(-1, &hue, 360);
          update_ = 1;
        }
        break;
      case 3:
        if (colorfulWorld == 0) //setup
        {
          //rainbowTime = millis() + colorfulWorldDelayValue;
          colorfulWorld = 1;
          colorfulWorldStage = 0;
        }
        if (millis() > rainbowTime) //cycle
        {
          rainbowTime = millis() + colorfulWorldDelayValue;
          colorfulWorldStage ++;
          if (colorfulWorldStage == 5)
          {
            colorfulWorldStage = 1;
          }

          switch (colorfulWorldStage)
          {
            case 1:
              hue = 0;
              saturation = 155;
              brightness = 255;
              break;
            case 2:
              hue = 180;
              saturation = 75;
              brightness = 215;
              break;
            case 3:
              hue = 295;
              saturation = 255;
              brightness = 255;
              break;
            case 4:
              hue = 30;
              saturation = 255;
              brightness = 255;
              break;
          }
          update_ = 1;
        } //colorful world cycle switch
        break;
    } //rainbow type switch
}
