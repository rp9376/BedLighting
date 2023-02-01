int EEPROMReturnValFromAddr(int addr) //int takes 2 bytes so addr goes 0,2,4
{
  byte stbyte, ndbyte; // first read is actually second in combining
  stbyte = int(EEPROM.read(addr));
  ndbyte = int(EEPROM.read(addr + 1));
  return (ndbyte << 8 | stbyte);
}

void EEPROMsaveValues(int digit) // 2 bytes for hue, 2 bytes for sat, 2 bytes for bri
{
  if (digit >= 0 && digit <= 9)
  {
    EEPROM.put(digit * 6, hue);
    EEPROM.put(digit * 6 + 2, saturation);
    EEPROM.put(digit * 6 + 4, brightness);
  }
}
