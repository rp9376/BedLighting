void addToValueWithLimit(int value, int *ptr, int limit) //call addToValueWithLimit(10, &hue, 255) //for sat&bri
{
  if (on)
  {
    *ptr += value;
    *ptr = min(limit, *ptr);
    *ptr = max(0, *ptr);
  }

}

void addToValueWithRollover(int value, int *ptr, int limit) //call setValue(10, &hue, 255) //for sat&bri
{
  if (on)
  {
    *ptr += value;
    if (*ptr >= limit)
      *ptr -= limit;
    else if (hue < 0)
      *ptr += limit;
  }
}

void CreateNumberWdigit(int value, unsigned long *ptr)
{
  if (value >= 0)
  {
    *ptr = *ptr * 10 + value;
    blinkled();
  }
}
