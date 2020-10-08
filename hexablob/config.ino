#include <FS.h>
#include <LittleFS.h>

#define FSEQFS LittleFS
enum status_t { STOPPED, ESEQ, BUILTIN, PAINTING }; 
enum builtin_t {PRIDE, CYLON, CYLONSPIRAL, COLORPALETTE};

const char* builtin_names[2] = {"Pride", "Cylon"};

typedef struct
{
  char fname_curr[64];
  int led_delay;
  uint8_t brightness;
  bool show_time;
  bool show_ip; //Show IP on boot via LEDs
  int tz_offset; //Needs to be signed
  long dst_start;
  bool dst;
  char tzdbapikey[16]; //API key for http://api.timezonedb.com/
  char location[32];// IANA Location name
  status_t status;
  builtin_t builtin_status;
  
  uint8_t checksum;//always put at the end
} cfg_t;

cfg_t         cfg;

#define CFG_FILE "hexablob.cfg"

void cfg_print ()
{
  Serial.println("Filename: " + String(cfg.fname_curr));
  Serial.println("Led delay: " + String(cfg.led_delay));
  Serial.println("brightness: " + String(cfg.brightness));
  Serial.println("show_time: " + String(cfg.show_time));
  Serial.println("show_ip: " + String(cfg.show_ip));
  Serial.println("tz_offset: " + String(cfg.tz_offset));
  Serial.println("dst_start: " + String(cfg.dst_start));
  Serial.println("dst: " + String(cfg.dst));
  Serial.println("tzdbapikey: " + String(cfg.tzdbapikey));
  Serial.println("location: " + String(cfg.location));
  Serial.println("status: " + String(cfg.status));
  Serial.println("builtin_status: " + String(cfg.builtin_status));
  Serial.println("checksum: " + String(cfg.checksum));
}

void cfg_default ()
{
  Serial.println ("Defaulting configuration");
  strcpy (cfg.fname_curr, "CyberFade.eseq");
  cfg.led_delay = 25;
  cfg.brightness = 40;
  cfg.show_time = true;
  cfg.show_ip = false;
  cfg.tz_offset = 9;
  cfg.dst_start = 0;
  strcpy (cfg.tzdbapikey, "TZDBAPIKEY");
  strcpy (cfg.location, "Unknown/Unknown");
  cfg.status = STOPPED;
  cfg.builtin_status = PRIDE;
  cfg.checksum = cfg_checksum();
}

uint8_t cfg_checksum ()
{
  unsigned char *p = (unsigned char *)&cfg;
  uint8_t sum = 0;
  cfg_print();
  for (int i=0; i< sizeof(cfg) - 4; i++) //TODO Seriously shonky 4 byte deduction.
  {
    //Serial.print (p[i]);
    sum += p[i];
  }  
  Serial.println ("Checksum: " + String (sum));
  return sum;
}

bool cfg_load(void)
{
  Serial.println ("Config loading");
  File file = FileFS.open(CFG_FILE, "r");
  if (file)
  {
    file.readBytes((char *) &cfg, sizeof(cfg));
    cfg_print();
    file.close();
  }
  else
  {
    cfg_default();
    return cfg_save();
  }

  if (cfg.checksum != cfg_checksum())
  {
    cfg_default();
    return cfg_save();
  }
   
}
    
bool cfg_save(void)
{
  Serial.println ("Saving LED cfg");
  File file = FileFS.open(CFG_FILE, "w");
  cfg.checksum = cfg_checksum();
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
