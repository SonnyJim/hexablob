#include <FS.h>
#include <LittleFS.h>

#define FSEQFS LittleFS

typedef struct
{
  char fname_curr[64];
  int led_delay;
  uint8_t brightness;
  bool show_time;
  bool show_ip; //Show IP on boot via LEDs
  int tz_offset; //Needs to be signed
  bool dst;
  char tzdbapikey[16]; //API key for http://api.timezonedb.com/
  char location[32];// IANA Location name
} cfg_t;

cfg_t         cfg;

#define CFG_FILE "hexablob.cfg"

void cfg_default ()
{
  strcpy (cfg.fname_curr, "CyberFade.eseq");
  cfg.led_delay = 25;
  cfg.brightness = 40;
  cfg.show_time = true;
  cfg.show_ip = false;
  cfg.tz_offset = 9;
  strcpy (cfg.tzdbapikey, "TZDBAPIKEY");
  strcpy (cfg.location, "Asia/Phnom Penh");
}

bool cfg_load(void)
{
  Serial.println ("Config loading");
  File file = FileFS.open(CFG_FILE, "r");
  if (file)
  {
    file.readBytes((char *) &cfg, sizeof(cfg));
    file.close();
    return true;
  }
  else
  {
    cfg_default();
    return cfg_save();
  }
}
    
bool cfg_save(void)
{
  File file = FileFS.open(CFG_FILE, "w");

  if (file)
  {
    file.write((uint8_t*) &cfg, sizeof(cfg));
    file.close();
    return true;
  }
  else
  {
    return false;
  }
}

void cfg_setup ()
{
      if (!FSEQFS.begin()) {
    Serial.printf("Unable to open FS, aborting\n");
    //TODO Do an error
    }
  cfg_load();//TODO Error checking
}
