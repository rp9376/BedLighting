#include <LEDStripDriver.h>
#include <IRremote.h>
#include <EEPROM.h>

//LedDriver
#define CIN 2
#define DIN 3
LEDStripDriver led = LEDStripDriver(DIN, CIN);

//IR receiver
#define RECV_PIN 5
IRrecv irrecv(RECV_PIN);
decode_results results;

//Debug settings //0 = normal operation //1 = debug w. serial //2 = only leds
#define DEBUG 0      //Will run seperate leds on pins 9 10 11
#define redLed 6
#define greenLed 9
#define blueLed 10

bool on = 0;
//Color variables
int rgb_colors[3] = {0, 0, 0};    // Color value storage
int hue = 0;
int saturation = 255;
int brightness = 0;
int preOffBrightness = 255;

//IR variables
unsigned int code = 0;            //IR read values
unsigned int last_code;
bool IRupdate = 0;
bool falseCode = 0;
bool itsDigit = 0;
int repeatedCode = 1;

//Rainbow variables
int rainbow = 0;
int rainbowDelayValue = 100;
int colorfulWorldDelayValue = 250;
bool colorfulWorld = 0;
int colorfulWorldStage = 1;
unsigned long rainbowButtonTimer;
#define buttonRePressTime 3000//ms
unsigned long rainbowTime;

int SelectedDigit = 0;
bool breathing = 0;

//Timer variables
bool TimerIsSet = 0;
unsigned long shutdownTime;
#define MillisForDefaultShutdown 18000000 //5 hours
bool resetedColorTo1stDigit = 1;
#define MillisForColorResetWhenOFF 1200000

bool update_ = 0;                 // If it updates colors at the end of each loop


void setup() {
#if DEBUG == 1
  Serial.begin(115200);
#endif
#if DEBUG > 2
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
#endif

  irrecv.enableIRIn();
  led.setColor(rgb_colors[0], rgb_colors[1], rgb_colors[2]);
  getRGB(hue, saturation, brightness, rgb_colors);
  blinkled();
  blinkled();
}

void loop() {

  readIR();
  if (IRupdate)
  {
#if DEBUG == 1
    Serial.println("Handeling code");
#endif
    handleCode();
  }

  if (rainbow)
  {
    handleRainbow();
  }


  if (on)
  {
    /*
      Serial.print("Timer: ");
      Serial.println(shutdownTime - millis());//*/
    TimerTurnOFF();
  }
  else
  {
    if (resetedColorTo1stDigit == 0 && millis() >= shutdownTime)
    {
      SelectedDigit = 0;
      hue = EEPROMReturnValFromAddr(0 * 6);
      saturation = EEPROMReturnValFromAddr(0 * 6 + 2);
      preOffBrightness = EEPROMReturnValFromAddr(0 * 6 + 4);
      resetedColorTo1stDigit = 1;
    }

  }

  if (update_)
  {
    setLight(hue, saturation, brightness);
    update_ = 0;
#if DEBUG == 1
    Serial.println("Updating Lights");
#endif
  }
#if DEBUG == 1
  delay(5);
#endif
}

void handleCode()
{
  falseCode = 0;
  itsDigit = 0;

  switch (code) {
    case 41565:         //ON Button //--------------FUNCTIONS----------
    case 9755:
      TurnOnOrOff();
      break;
    case 28031:           //Func/Stop button
    case 57885:
      if (on)
      {
        if (rainbow == 0)
        {
          blinkled(); blinkled();
          rainbowButtonTimer = millis() + buttonRePressTime;
          rainbow = 1;
#if DEBUG == 1
          Serial.println("Rainbow turnon");
#endif
        }
        else
        {
          if (rainbowButtonTimer > millis() && rainbow < 3)
          {
            blinkled();
            rainbow ++;
            rainbowButtonTimer = millis() + 5000;
          }
          else
          {
            blinkled();
            blinkled();
            rainbow = 0;
          }
          if (rainbow != 3)
          {
            colorfulWorld = 0;
          }
        }
      }
      break;
    case 5699:           //st/rept button
    case 45135:

      break;
    case 15355:         //EQ button
    case 39015:
      setTimer();
      break;    //----------ALTERS----------------------------
    case 25245:         //vol +
    case 7611:
      addToValueWithLimit(20, &brightness, 255);
      break;
    case 43095:         //vol -
    case 60891:
      addToValueWithLimit(-20, &brightness, 255);
      break;
    case 36975:         //up
    case 48511:
      switch (rainbow)
      {
        case 0:
          addToValueWithLimit(20, &saturation, 255);
          break;
        case 1:
        case 2:
          rainbowDelayValue = max(0, rainbowDelayValue -= 10);
          break;
        case 3:
          colorfulWorldDelayValue = max(50, colorfulWorldDelayValue -= 25);
          break;
      }
      /*if (rainbow)
        if (rainbow == 1 || rainbow == 2)
        {
          rainbowDelayValue = max(0, rainbowDelayValue -= 10);
        } else
        {

        }


        else
        addToValueWithLimit(20, &saturation, 255);*/
      break;
    case 57375:         //down
    case 49467:
      switch (rainbow)
      {
        case 0:
          addToValueWithLimit(-20, &saturation, 255);
          break;
        case 1:
        case 2:
          rainbowDelayValue = min(200, rainbowDelayValue += 10);
          break;
        case 3:
          rainbowDelayValue = min(2000, colorfulWorldDelayValue += 10);
          break;
      }


      /*
        if (rainbow)
          rainbowDelayValue = min(200, rainbowDelayValue += 10);
        //addToValueWithLimit(10, rainbowDelayValue, 500);
        else
          addToValueWithLimit(-20, &saturation, 255);*/
      break;
    case 19899:         //  >>|
    case 49725:
      addToValueWithRollover(10, &hue, 360);
      break;
    case 8925:         //  |<<
    case 54303:
      addToValueWithRollover(-10, &hue, 360);
      break;
    case 765:         //    >|| play/pause
    case 19227:
#if DEBUG == 1
      dumpValues();
#endif
#if DEBUG == 0 || DEBUG == 2
      addToValueWithLimit(255, &brightness, 255);
      addToValueWithLimit(255, &saturation, 255);
#endif
      break;  //------------NUMBER PAD-------------------
    case 58747:         //    0
    case 26775:
      updateValuesForDigits(0);
      break;
    case 48703:         //    1
    case 12495:
      updateValuesForDigits(1);
      break;
    case 58359:         //    2
    case 6375:
      updateValuesForDigits(2);
      break;
    case 539:         //    3
    case 31365:
      updateValuesForDigits(3);
      break;
    case 4335:         //    4
    case 25979:
      updateValuesForDigits(4);
      break;
    case 15547:         //    5
    case 14535:
      updateValuesForDigits(5);
      break;
    case 59295:         //    6
    case 23205:
      updateValuesForDigits(6);
      break;
    case 65015:         //    7
    case 17085:
      updateValuesForDigits(7);
      break;
    case 5499:         //    8
    case 19125:
      updateValuesForDigits(8);
      break;
    case 64539:        //    9
    case 21165:
      updateValuesForDigits(9);

      break;
    default:
      falseCode = 1;
      break;
  }

  if (TimerIsSet == 0 && falseCode == 0 && on)
  {
    shutdownTime = millis() + MillisForDefaultShutdown;
  }

  if (last_code == code)
    repeatedCode ++;
  else
    repeatedCode = 1;

  if (code == 5699 || code == 45135 && repeatedCode == 3)
  {
    int progDigit;
    unsigned long timeout = millis() + 10 * 1000;
    unsigned long blinktime = 0;
    bool savevalues = 0;
    code = 0;
    while (1)
    {
      if (blinktime < millis())
      {
        blinkled();
        blinktime = millis() + 100;
      }

      readIR();
      if (IRupdate)
      {
        progDigit = numpadPressedButton(code);
        IRupdate = 0;
        if (progDigit >= 0 && progDigit <= 9)
        {
          savevalues = 1;
          break;
        }
      }

      if (timeout < millis() || code == 5699 || code == 45135)
      {
        break;
      }
    }
    if (savevalues)
    {
      blinkled();
      blinkled();
      blinkled();
      EEPROMsaveValues(progDigit);
    }
    repeatedCode = 0;
  }

  IRupdate = 0;
  update_ = 1;
}



void TurnOnOrOff()
{
#if DEBUG == 1
  Serial.println("Turning ON or OFF");
#endif
  rainbow = 0;
  breathing = 0;
  TimerIsSet = 0;

  if (on)
  {
    resetedColorTo1stDigit = 0;
    shutdownTime = millis() + MillisForColorResetWhenOFF;
  }

  if (on == 1 && brightness < 100)
  {
    preOffBrightness = 255;
  }

  if (on) //going down
  {
    preOffBrightness = brightness;
    while (1)
    {
      brightness = brightness -= 2;
      delay(1);
      if (brightness < 0)
        break;
      setLight(hue, saturation, brightness);
    }
    brightness = 0;
    setLight(hue, saturation, brightness);
  }
  else //going up
  {
    while (1)
    {
      brightness = brightness += 2;
      delay(1);
      if (brightness > preOffBrightness)
        break;
      setLight(hue, saturation, brightness);
    }
    brightness = preOffBrightness;
    setLight(hue, saturation, brightness);
  }

  on = !on; //Because when its off setLight doesnt set fucking lights
  //update_ = 1;
  irrecv.resume(); //Quick fix for rapid on off //to prevent it
}

void updateValuesForDigits(int digit)
{
  if (on)
  {
    SelectedDigit = digit;
    rainbow = 0;
    hue = EEPROMReturnValFromAddr(digit * 6);
    saturation = EEPROMReturnValFromAddr(digit * 6 + 2);
    brightness = EEPROMReturnValFromAddr(digit * 6 + 4);
    itsDigit = 1;
  }
}

#if DEBUG == 1
void dumpValues()
{
  Serial.println("DUMPING VALUES");
  Serial.print("Power ON: "); Serial.println(on);
  Serial.print("HUE: "); Serial.println(hue);
  Serial.print("Saturation: "); Serial.println(saturation);
  Serial.print("Brightness: "); Serial.println(brightness);
  Serial.print("Red: "); Serial.print(rgb_colors[0]);
  Serial.print("    Green: "); Serial.print(rgb_colors[1]);
  Serial.print("    Blue: "); Serial.println(rgb_colors[2]);
  Serial.print("Brightness: "); Serial.println(brightness);
  Serial.print("Rainbow: "); Serial.println(rainbow);
  Serial.print("Rainbow delay: "); Serial.println(rainbowDelayValue);
  Serial.print("Millis: "); Serial.println(millis());

  Serial.println("Dumping EEPROM values for programmed values");
  for (int i = 0; i < 10 * 3 * 2; i += 6)
  {
    int hue;
    int sat;
    int bri;
    byte stbyte, ndbyte; // first read is actually second in combining

    stbyte = int(EEPROM.read(i));
    ndbyte = int(EEPROM.read(i + 1));
    hue = ndbyte << 8 | stbyte;

    stbyte = int(EEPROM.read(i + 2));
    ndbyte = int(EEPROM.read(i + 2 + 1));
    sat = ndbyte << 8 | stbyte;

    stbyte = int(EEPROM.read(i + 4));
    ndbyte = int(EEPROM.read(i + 4 + 1));
    bri = ndbyte << 8 | stbyte;

    Serial.print(i / 6);
    Serial.print(") ");
    Serial.print("Hue: ");
    Serial.print(hue);
    Serial.print("  Sat: ");
    Serial.print(sat);
    Serial.print("  Bri: ");
    Serial.println(bri);
  }
}
#endif
