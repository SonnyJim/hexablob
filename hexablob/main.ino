//TODO Scan local subnet for your phone to see if you are at home to turn the lights on
//Have an 'error connection' color.

#include <ESP8266mDNS.h> 

long loop_time;
#define LOOP_DELAY 5000


void setup ()
{
  Serial.begin(115200);
  while (!Serial);
  led_setup();
  wifi_setup();
  Serial.println("Setting up MDNS");
  if (!MDNS.begin("hexablob"))
  {
    Serial.println("Error setting up MDNS responder!");
  }
  led_drawip();
  ntp_setup ();
  webserver_setup ();
  
  /*
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) 
  {
    Serial.print(dir.fileName());
    if(dir.fileSize()) 
    {
        File f = dir.openFile("r");
        Serial.println(f.size());
    }
  }
  */
}

void loop ()
{
  /*Service the important loops, but only service the other ones every 5 seconds */
  wifi_loop ();
  led_loop ();

  if (millis() > loop_time + LOOP_DELAY)
  {
    loop_time = millis();
  }
  else
    return;
    
  /* Stuff under here only runs every 5 seconds */
  ntp_loop ();

  
}
