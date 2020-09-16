#include <FastLED.h>
#include <FS.h>
#include <LittleFS.h>

#define FSEQFS LittleFS

#define NUM_LEDS 183 
#define DATA_PIN 4

// Define the array of leds
CRGB leds[NUM_LEDS];
File dataFile;



int test_number;
long test_wait;



#define LEDNUMLEN 12 //Max length of the number LED array, or rather the max amount of pixels to use per number
//List LEDs to light up by number, 255 terminates the array
const uint8_t numbers[10][LEDNUMLEN] = {
{7, 8, 13, 15, 20, 23, 29, 31, 38, 39 , 255}, //0
{8, 14, 22, 30, 39, 255},  //1
{7, 8, 13, 15, 22, 30, 38, 39,40, 255}, //2
{7, 8, 13, 15, 22, 29, 31, 38, 39 , 255}, //3
{15,13,21,22,29,39, 255}, //4 eek
{7,8, 15,21,22,29,38,39,255}, //5
{7,8,15,21,22,31,29,38,39,255},  //6
{7,8,9,13,22,30,38,255}, //7
{7,8,15,13,21,22,31,29,38,39,255}, //8
{7,8,15,13,21,22,29,38,39,255}, //9
};

const uint8_t dot[4] = {14,21,22,30};

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
bool running;

void openeseq (String filename)
{
  fname_curr = "Nothing";
  running = false;
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
    running = false;
    return;
   }
   
   Serial.println("Header: "+ String(headerData.magic)); 
   Serial.println("Model offset: " + String(headerData.model_offset));
   Serial.println("Step Size: "+ String(headerData.stepsize));
   Serial.println("Model start: "+ String(headerData.model_start));
   Serial.println("Model size: " + String(headerData.model_size));
   //Rewind to the start of the data
   dataFile.seek (sizeof(headerData));
   running = true;  
   fname_curr = filename;
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
    running = false;
    return; 
  }
  
  memcpy (&leds, ledData, headerData.stepsize);
  
  //led_drawtime (test_number);

  
  FastLED.show();
  delay(led_delay);
}


/*
void led_drawtime (int num)
{
  if (millis() > test_wait + 1000)
  {
    test_number++;
    if (test_number > 9)
      test_number = 0;
    test_wait = millis();
  }
  int i;

  for (i = 0; i < LEDNUMLEN; i++)
  {
    if (numbers[num][i] == 255)
      return; //All finished
    else
      leds[numbers[num][i]] = CRGB(255,255,255);
  }
}
*/
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
  for (i = 0; i < LEDNUMLEN; i++)
  {
    if (numbers[num][i] == 255)
      return; //All finished
    else
      leds[numbers[num][i]] = CRGB(255,255,255);
  }
}

void led_loop() { 
  if (running)
    playeseq();
}

void led_setup() { 
  Serial.println ("Setting up LEDs");
  LEDS.addLeds<WS2812,DATA_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(40);
  brightness = 40;
  //Clear the strip
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
  delay(500);
  FastLED.clear();
  FastLED.show();
    if (!FSEQFS.begin()) {
    Serial.printf("Unable to open FS, aborting\n");
    //TODO Do an error
    }

  openeseq("Custom-2.eseq");
  test_wait = millis();
  test_number = 0;
  show_ip = true;
}
