#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()


#define DST_MN          60      // use 60mn for summer time in some countries
#define DST_SEC         ((DST_MN)*60)

time_t now;
bool time_valid = false;

void time_is_set(void) {
  time_valid = true;
  Serial.println("------------------ settimeofday() was called ------------------");
}

void ntp_setup() {
  settimeofday_cb(time_is_set);

  if (cfg.tz_offset < 25)
    configTime(cfg.tz_offset * 3600, DST_SEC, "pool.ntp.org");
  else
  {
    if (cfg.dst)//TODO WHHHHHHHY?
      configTime(cfg.tz_offset, DST_SEC, "pool.ntp.org");
    else
      configTime(cfg.tz_offset - DST_SEC, DST_SEC, "pool.ntp.org");
  }
}

int ntp_gethours ()
{
  now = time(nullptr);
  struct tm *ptm = localtime(&now);
  return ptm->tm_hour;
}

int ntp_getminutes ()
{
  now = time(nullptr);
  struct tm *ptm = localtime(&now);
  return ptm->tm_min;
}

int ntp_getseconds ()
{
  now = time(nullptr);
  struct tm *ptm = localtime(&now); 
  return ptm->tm_sec;
}

void ntp_loop() {

/*
  gettimeofday(&tv, nullptr);
//  clock_gettime(0, &tp);
  now = time(nullptr);
  */
  /*
  now_ms = millis();
  now_us = micros();

  // localtime / gmtime every second change
  static time_t lastv = 0;
  if (lastv != tv.tv_sec) {
    lastv = tv.tv_sec;
    Serial.println();
    printTm("localtime", localtime(&now));
    Serial.println();
    printTm("gmtime   ", gmtime(&now));
    Serial.println();
    Serial.println();
  }

  // time from boot
  Serial.print("clock:");
  Serial.print((uint32_t)tp.tv_sec);
  Serial.print("/");
  Serial.print((uint32_t)tp.tv_nsec);
  Serial.print("ns");

  // time from boot
  Serial.print(" millis:");
  Serial.print(now_ms);
  Serial.print(" micros:");
  Serial.print(now_us);

  // EPOCH+tz+dst
  Serial.print(" gtod:");
  Serial.print((uint32_t)tv.tv_sec);
  Serial.print("/");
  Serial.print((uint32_t)tv.tv_usec);
  Serial.print("us");

  // EPOCH+tz+dst
  Serial.print(" time:");
  Serial.print((uint32_t)now);

  // human readable
  Serial.print(" ctime:(UTC+");
  Serial.print((uint32_t)(TZ * 60 + DST_MN));
  Serial.print("mn)");
  Serial.print(ctime(&now));

  // simple drifting loop
  //delay(100);
  */
}
