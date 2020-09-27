//TODO Scan local subnet for your phone to see if you are at home to turn the lights on
//Have an 'error connection' color.
//Store currently playing to file, along with other config data
//Analogue clock face
//Pimpin' ain't easy, hours in characters, minutes in LEDs - 1
//Put filesize free on upload page and don't allow client upload if filesize too big
//Use Geoip to set clock??

#include <ESP8266mDNS.h> 

long loop_time;
#define LOOP_DELAY 5000


void setup ()
{
  Serial.begin(115200);
  while (!Serial);
  cfg_setup();
  cfg.status = BOOTING;
  led_setup();
  wifi_setup();
  Serial.println("Setting up MDNS");
  if (!MDNS.begin("hexablob"))
  {
    Serial.println("Error setting up MDNS responder!");
  }

  if (strcmp (cfg.location, "Unknown/Unknown") == 0)
    geoip_get();
  else
    geoip_tzget(String(cfg.location));
  
  ntp_setup ();
  fs_setup ();
  if (cfg.show_ip)
    led_drawip();
  openeseq (cfg.fname_curr);
  webserver_setup ();
  
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
  /*
  Serial.println ("Hour " + String(ntp_gethours()));
  Serial.println ("Minutes " + String(ntp_getminutes()));
  */

  
}
