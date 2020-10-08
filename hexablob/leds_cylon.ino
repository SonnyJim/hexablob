
void cylon_fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(180); } }

int cylon_count = 0;

int cylon_spiral[61] = {2,1,0,10,11,25,26,42,43,55,56,57,58,59,60,50,49,35,34,18,17,5,4,3,7,8,9,12,24,27,41,44,54,53,52,51,48,36,33,19,16,6,15,14,13,23,28,40,45,46,47,37,32,20,21,22,29,39,38,31,30};

void cylon_loop() { 
  if (cylon_count++ > NUM_LEDS)
    cylon_count = 0;
  leds[cylon_count] += CRGB(255,0,0);
  cylon_fadeall();
}

void cylonspiral_loop()
{
  if (cylon_count++ > NUM_LEDS)
    cylon_count = 0;
  leds[cylon_spiral[cylon_count]] += CRGB(255,0,0);
  cylon_fadeall();
}
