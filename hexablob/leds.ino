#include <FastLED.h>


#define NUM_LEDS 183 
#define DATA_PIN 4

CRGB leds[NUM_LEDS];
File dataFile; //ESEQ data file

extern bool time_valid;
long paint_time;
#define PAINT_DELAY 10000

#define LEDNUMLEN 12 //Max length of the number LED array, or rather the max amount of pixels to use per number
//List LEDs to light up by number, 254 terminates the array
const uint8_t numbers[10][LEDNUMLEN] = {
{7, 8, 13, 15, 20, 23, 29, 31, 38, 39 , 254}, //0
{8, 14, 22, 30, 39, 254},  //1
{7, 8, 13, 15, 22, 30, 38, 39,40, 254}, //2
{7, 8, 13, 15, 22, 29, 31, 38, 39 , 254}, //3
{15,13,21,22,29,39, 254}, //4 eek
{7,8, 15,21,22,29,38,39,254}, //5
{7,8,15,21,22,31,29,38,39,254},  //6
{7,8,9,13,22,30,38,254}, //7
{7,8,15,13,21,22,31,29,38,39,254}, //8
{7,8,15,13,21,22,29,38,39,254}, //9
};

#define MAXLEDLEN 14
const uint8_t timenum[MAXLEDLEN][14] = {
{14,21,30,29,38,39,46,254},//0
{14,21,22,30,38,39,46,254},//1
{15,14,20,22,30,38,47,46,45,254},//2
{15,14,20,22,30,37,39,47,46,254},//3
{15,13,21,22,29,39, 254},//4
{15,14,20,31,30,39,47,46,254},//5
{15,14,20,31,30,37,39,47,46,254},//6
{15,14,13,22,30,38,47,254}, //7
{15,14,20,22,31,30,37,39,47,46, 254},//8
{15,14,20,22,31,30,39,47,46, 254},//9
{15,13,20,22,23,31,30,28,37,39,40,47,45,254},//10
{15,13,20,22,31,29,37,39,47,45,254}, //11
{15,13,12,20,22,24,31,28,37,40,47,45,44,254},//12
};

const uint8_t dot[4] = {14,21,22,30};

const uint8_t outside_min[24] = {2,1,0,10,11,25,26,42,43,55,56,57,58,59,60,50,49,35,34,18,17,5,4,3};

typedef struct _headerData_t
{
  char      magic[4];
  uint32_t   model_offset; 
  uint32_t   stepsize;
  uint32_t   model_start;
  uint32_t   model_size;
} headerData_t;

headerData_t headerData;
char rawData[20]; //Raw header data, probably don't actually need this
char ledData[256]; //needs to be at least as big as the stepsize?

void led_save (String filename)
{
  /*
   * Header: ESEQ
Model offset: 1
Step Size: 184
Model start: 4
Model size: 183

   */
  File file;
  headerData_t header;
  strncpy (header.magic, "ESEQ", 4);
  header.model_offset = 1;
  header.stepsize = 184;
  header.model_start = 4;
  header.model_size = 183;
  file = FSEQFS.open(filename, "w");
  if (file)
  {
    file.write((uint8_t*) &header, sizeof(header));
    file.write((uint8_t*) &leds, 184);
    file.close();
  }
}
void openeseq (String filename)
{
  Serial.println ("Opening: " + filename);
  strcpy (cfg.fname_curr, "Nothing");
  cfg.status = STOPPED;
  dataFile = FSEQFS.open(filename, "r");
  Serial.println("File size: " + String(dataFile.size()));
  if (dataFile.size() > 0)
  {
    dataFile.readBytes (rawData, 20);
    memcpy (&headerData, rawData, 20);
   if (strncmp (headerData.magic, "ESEQ", 4))
   {
    Serial.print ("Error: Header data incorrect ");
    Serial.println (headerData.magic);
    return;
   }
   
   Serial.println("Header: "+ String(headerData.magic)); 
   Serial.println("Model offset: " + String(headerData.model_offset));
   Serial.println("Step Size: "+ String(headerData.stepsize));
   Serial.println("Model start: "+ String(headerData.model_start));
   Serial.println("Model size: " + String(headerData.model_size));
   //Rewind to the start of the data
   dataFile.seek (sizeof(headerData));
   cfg.status = PLAYING;  
   filename.toCharArray(cfg.fname_curr, filename.length() + 1);
  }
}

void playeseq ()
{
  if (dataFile.available() < headerData.stepsize)
  {
    dataFile.seek (sizeof(headerData));
  }
  
  if (dataFile.readBytes (ledData, headerData.stepsize) != headerData.stepsize)
  {
    Serial.println ("Error reading bytes into buffer");
    cfg.status = STOPPED;
    return; 
  }
  
  memcpy (&leds, ledData, headerData.stepsize);
  
}

//Pimp display
void led_drawmin (int minutes)
{
  int i;
  for (i = 60; i > (60 - minutes); i--)
  {
    leds[i] = CRGB(0,0,255);
  }

}

void led_drawoutsidemin (int minutes)
{
  int remap = map (minutes, 0, 59, 0, 24);
  int i;
  for (i = 0;i < remap; i++)
  {
    leds[outside_min[i]] -= CRGB(225,225,225);
    int blue = map (i, 0, remap, 25, 255);
    leds[outside_min[i]]  += CRGB(0,0,blue);
  }
}



int seconds_fade;
int seconds_old;

void led_drawsecs (int seconds)
{ 
  if (seconds_old != seconds)
  {
    seconds_fade = 230;
    seconds_old = seconds;
  }
  //int remap = map (seconds, 0, 59, 0, 24);
  int remap;
  if (seconds <24)
    remap = seconds;
  else if (seconds <48)
    remap = seconds - 24;
  else if (seconds < 60)
    remap = map (seconds-48, 0, 12, 0, 24);
    
  leds[outside_min[remap]] += CRGB(seconds_fade,seconds_fade,seconds_fade);
  if (seconds_fade > 10)
    seconds_fade -= 11;
  else
    seconds_fade = 0;
    
}

void led_drawtime ()
{
  int hour;
  int minutes;

  hour = ntp_gethours();
  minutes = ntp_getminutes();
  /*
  Serial.println ("Hour " + String(ntp_gethours()));
  Serial.println ("Minutes " + String(ntp_getminutes()));
  */
  if (hour > 12)
    hour -= 12;
    

  led_drawoutsidemin(minutes);
  led_drawsecs (ntp_getseconds());
  led_drawtimenum(hour);
}

void led_drawip ()
{
  //Probably want to check to see if we are in AP mode, if so, skip this and signal the user
  String ip = WiFi.localIP().toString(); 
  char buf[15];

  ip.toCharArray(buf, sizeof(buf));
  int ipnum;
  int i;
  for (i=0; i < strlen(buf);i++)
  {
    FastLED.clear();
    ipnum = buf[i] - 48; //Magic 48 woo
    if (ipnum != -2) //-2 is a dot
    {
      led_drawnum(ipnum);
    }
    else
    {
      led_drawdot();
    }

    FastLED.show();
    delay(1000);
    FastLED.clear();
    FastLED.show();
    delay(200);

 
  }
}
void led_drawdot ()
{
  int i;
  for (i = 0; i < 3; i++)
  {
      leds[dot[i]] = CRGB(255,255,255);
  }
}

void led_drawnum (int num)
{
  int i;
  for (i = 0; i < MAXLEDLEN; i++)
  {
    if (numbers[num][i] == 254)
      return; //All finished
    else
      leds[numbers[num][i]] = CRGB(255,255,255);
  }
}

void led_drawtimenum (int num)
{
  int i;
  for (i = 0; i < MAXLEDLEN; i++)
  {
    if (timenum[num][i] == 254)
      return; //All finished
      else
        leds[timenum[num][i]] = CRGB(255,255,255);
    }
}

void led_paint_from_string (String data)
{
  //253,191,0,253,191,
  int i;
  int j = 0;
  int color_step = 1;
  int start_pos = 0;
  int value;
  int red,green,blue;
  
  paint_time = millis();
  for (i=0;i<=data.length();i++)
  {
    if (data.charAt(i) == ',')
    {
      //Serial.println ("Found needle at " + String(i));
      value = data.substring (start_pos, i).toInt();
      //Serial.println (value);
      start_pos = i + 1;
      if (color_step == 1)
        red = value;
      else if (color_step == 2)
        green = value;
      else if (color_step == 3)
        blue = value;
      color_step++;
      if (color_step > 3)
      {
        //Serial.println ("Painting " + String(j));
        leds[j] = CRGB(red,green,blue);
        color_step = 1;
        j++;
      }
    }
      
  }
  
}
void led_loop() { 
  
  if (cfg.status == PLAYING && paint_time == 0)
  {
    playeseq();
    if (time_valid && cfg.show_time)
      led_drawtime();
  }
  
  if (millis() > paint_time + PAINT_DELAY)
  {
    paint_time = 0;
  }
  FastLED.show();
  delay(cfg.led_delay);
}

void led_setup() { 
  Serial.println ("Setting up LEDs");
  LEDS.addLeds<WS2812,DATA_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(cfg.brightness);
  //Clear the strip
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
  delay(500);
  FastLED.clear();
  FastLED.show();


  
}
