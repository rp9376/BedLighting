void readIR()
{
  if (irrecv.decode(&results))
  {
    last_code = code;
    code = results.value;
#if DEBUG == 1
    Serial.println(code);
#endif
    IRupdate = 1;
    irrecv.resume();
  }
}

int numpadPressedButton(int val)
{
  switch (val)
  {
    case 58747:         //    0
    case 26775:
      return 0;
      break;
    case 48703:         //    1
    case 12495:
      return 1;
      break;
    case 58359:         //    2
    case 6375:
      return 2;
      break;
    case 539:         //    3
    case 31365:
      return 3;
      break;
    case 4335:         //    4
    case 25979:
      return 4;
      break;
    case 15547:         //    5
    case 14535:
      return 5;
      break;
    case 59295:         //    6
    case 23205:
      return 6;
      break;
    case 65015:         //    7
    case 17085:
      return 7;
      break;
    case 5499:         //    8
    case 19125:
      return 8;
      break;
    case 64539:         //    9
    case 21165:
      return 9;
    default:
      return -1;
  }
}
