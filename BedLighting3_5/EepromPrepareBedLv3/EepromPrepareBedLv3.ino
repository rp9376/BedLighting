#include <EEPROM.h>
unsigned int val = 0;
#define Setup 0

#if Setup == 0
void setup()
{
  Serial.begin(115200);
  for(int i = 0;i<10*3*2;i+=6)
  {
    int hue;
    int sat;
    int bri;
    byte stbyte, ndbyte; // first read is actually second in combining
    
    stbyte = int(EEPROM.read(i));
    ndbyte = int(EEPROM.read(i+1));
    hue = ndbyte << 8 | stbyte;

    stbyte = int(EEPROM.read(i+2));
    ndbyte = int(EEPROM.read(i+2+1));
    sat = ndbyte << 8 | stbyte;

    stbyte = int(EEPROM.read(i+4));
    ndbyte = int(EEPROM.read(i+4+1));
    bri = ndbyte << 8 | stbyte;

    Serial.print(i/6+1);
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



#if Setup == 1
void setup()
{
  Serial.begin(115200);
  int hue = 0;
  int sat = 255;
  int bri = 255;
  Serial.println("Before writing");
  for (int address = 0; address < 10 * 3 * 2; address += 6)
  {
    Serial.println("Writing...");
    EEPROM.put(address, hue);
    EEPROM.put(address + 2, sat);
    EEPROM.put(address + 4, bri);
    hue = hue + 360 / 10;
  }

  Serial.println("Write to eeprom done");
  Serial.println("");
  Serial.println("Dumping:");

  byte data[16];
  int lenghtI = EEPROM.length() / 16;
  for (int i = 0; i < lenghtI; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      data[j] = EEPROM.read(i * 16 + j);
    }
    char buf[80];
    sprintf(buf, "%03x:     %02x %02x %02x %02x %02x %02x %02x %02x       %02x %02x %02x %02x %02x %02x %02x %02x",
            i, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
    Serial.println(buf);
  }
}
#endif




/*
  void readEEPROMvalues()
  {
  for (int i = 0; i < 10; i++)
  {
    int st nd;
    HueValues[i*2] = st;
    HueValues[i*2 +1] = st;
  }
  }
*/

/*
  byte btis= B11001010;
  Serial.println(btis << 4);

*/


/*
  #if Setup == 0
  void setup() {
  //EEPROM.put(0, val);
  Serial.begin(115200);

  byte stbyte, ndbyte; // first read is actually second in combining
  stbyte = int(EEPROM.read(0 * 2));
  ndbyte = int(EEPROM.read(0 * 2 + 1));
  val = ndbyte << 8 | stbyte;

  Serial.print(val);
  }
  #endif//*/




void loop() {
  // put your main code here, to run repeatedly:

}
