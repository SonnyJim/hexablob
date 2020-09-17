#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()

#define TZ              9       // (utc+) TZ in hours
#define DST_MN          60      // use 60mn for summer time in some countries

////////////////////////////////////////////////////////

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

//timeval cbtime;      // time set in callback
//bool cbtime_set = false;

bool time_valid = false;

void time_is_set(void) {
  //gettimeofday(&cbtime, NULL);
  //cbtime_set = true;
  time_valid = true;
  Serial.println("------------------ settimeofday() was called ------------------");
}

void ntp_setup() {
  settimeofday_cb(time_is_set);

  configTime(TZ_SEC, DST_SEC, "au.pool.ntp.org");
  //TO
}

timeval tv;
timespec tp;
time_t now;
uint32_t now_ms, now_us;

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
