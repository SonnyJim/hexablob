File fsUploadFile;
char ip_addr[15];

#define FILE_EXT ".eseq"

int freespace;

//Processor for the config page
String fnameProcessor(const String& var)
{
  if(var == "combobox")
    return String(getFileCombo());
  else if (var == "fname_curr")
    return String (cfg.fname_curr);
  else if (var == "brightness")
    return String (cfg.brightness);   
  else if (var =="led_delay")
    return String (cfg.led_delay);
  else if (var =="show_time")
  {
    if (cfg.show_time)
      return String("True");
    else
      return String("False");
  }
  else if (var =="show_ip")
  {
    if (cfg.show_ip)
      return String ("True");
    else
      return String ("False");
  }
  else if (var =="fs_free")
    return String (fs_free() / 1024);
  else if (var =="tz_offset")
    return String (cfg.tz_offset);
  else if (var =="tzdbapikey")
    return String (cfg.tzdbapikey);
  else if (var =="location")
    return String (cfg.location);
    
  return String();
}

String getFileCombo()
{
  const String front = "<option value=\"";
  const String middle = "\">";
  const String rear = "</option>";
  String result;
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) 
  {
    if (dir.fileName().indexOf(FILE_EXT, dir.fileName().length() - strlen(FILE_EXT)) != -1) //Only display files ending in .eseq
    {
      result += front;
      result += dir.fileName();
      result += middle;
      result += dir.fileName();
      result += rear;
    } 
  }
  return result;  
}

void notFound(AsyncWebServerRequest *request) {
  Serial.println ("Error 404 " + String(request->method()) + " Url: "+ String(request->url()));
    request->send(404, "text/plain", "Not found");
}

String reprocessor(const String& var)
{
  if(var == "foobar")
    return String(WiFi.localIP().toString());
  else if(var == "IP")
    return String(WiFi.localIP().toString());
    
  return String();
}
/*
 *   Serial.println ("Printing Headers");
    int i;
    int headers = request->headers();
    for(i=0;i<headers;i++){
  AsyncWebHeader* h = request->getHeader(i);
  Serial.printf("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
}
    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

 */
inline bool fs_enoughspace (int fsize)
{

  int fs_free = fs_info.totalBytes - fs_info.usedBytes;
  //Serial.println (String(fs_free) + " Bytes free");
  if (fs_free > fsize)
    return true;
  else
    return false;
}
void update_freespace ()
{
  freespace = fs_info.totalBytes - fs_info.usedBytes;
}
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){

  int content_length = 0; 
  if(!index){
    Serial.printf("UploadStart: %s\n", filename.c_str());
  
    Serial.println(String(request->contentLength()));
    /*
    if(request->hasHeader("Content-Length"))
    {
      AsyncWebHeader* h = request->getHeader("Content-Length");
      content_length = h->value().toInt();
    }
    */
    freespace = fs_info.totalBytes - fs_info.usedBytes;
    if (freespace < content_length)
    //if (!fs_enoughspace (request->contentLength()))
    {
       Serial.println("Not enough space");
           //request->redirect("/failed");
        request->send(413, "text/plain", "Upload aborted.  No space left.  Try deleting some stuff");
        //return;
    }     
  }
  if (!fsUploadFile)
    fsUploadFile = LittleFS.open(filename, "w"); 
  
  for(size_t i=0; i<len; i++){
    fsUploadFile.write(data[i]);
    
  }
  
  if(final){
    fsUploadFile.close();
    request->redirect("/success");
    request->send(303);
    Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index+len);
  }


  //  } else {
  //    server.send(500, "text/plain", "500: couldn't create file");
}

void webserver_setup ()
{
  //ip_addr = String(WiFi.localIP()); 
  /*
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });
    */
  webServer.onNotFound(notFound);
  webServer.serveStatic("/", LittleFS, "/index.htm");
  webServer.serveStatic("/index.htm", LittleFS, "/index.htm");
  webServer.serveStatic("/styles/style.css", LittleFS, "/styles/style.css");
  webServer.serveStatic("/styles/colpick.css", LittleFS, "/styles/colpick.css");
  webServer.serveStatic("/favicon.ico", LittleFS, "/favicon.ico");
  webServer.serveStatic("/colpick.js", LittleFS, "/colpick.js");
  webServer.serveStatic("/main.js", LittleFS, "/main.js");
  webServer.serveStatic("/bob-holness.jpg", LittleFS, "/bob-holness.jpg");

  webServer.on("/upload", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/upload.htm", String(), false, fnameProcessor);
  });

  webServer.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {

        request->send(200);
        
      }, handleUpload);

  webServer.on("/success", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/success.htm", String(), false, reprocessor);
  });

  webServer.on("/save", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println ("Saving Hexablob config");
    cfg_save ();
  });
  
  webServer.on("/led_save", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println ("Saving LED pattern");
    led_save ("autosave.eseq");
  });
  webServer.on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
  {
   AsyncWebParameter* p;
   Serial.println ("GET /CONFIG");

/*
int params = request->params();
for(int i=0;i<params;i++){
  AsyncWebParameter* p = request->getParam(i);
  if(p->isFile()){ //p->isPost() is also true
    Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
  } else if(p->isPost()){
    Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
  } else {
    Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
  }
}
 */
  if(request->hasParam("effects")) 
  {
    p = request->getParam("effects");
    if (request->hasParam("delete"))
      LittleFS.remove(p->value());
    else
      openeseq(p->value());

    
  }
  
  if(request->hasParam("show_ip")) 
  {
    p = request->getParam("show_ip");
      if (p->value() == "true")
        cfg.show_ip = true;
      else
        cfg.show_ip = false;
  }
  
  if (request->hasParam("showtime"))
    {
      
       p = request->getParam("showtime");
      if (p->value() == "true")
        cfg.show_time = true;
      else      
        cfg.show_time = false;
        
    }

    if(request->hasParam("tzdbapikey")) 
    {
      AsyncWebParameter* p = request->getParam("tzdbapikey");
      p->value().toCharArray(cfg.tzdbapikey, p->value().length() + 1);//Why??? pfft
      //geoip_tzget (String(cfg.location));
     }
    
  request->send(LittleFS, "/config.htm", String(), false, fnameProcessor);
  });


  webServer.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *request)
  {

      if(request->hasParam("brightness")) 
  {
    AsyncWebParameter* p = request->getParam("brightness");
    cfg.brightness = p->value().toInt();
    LEDS.setBrightness(cfg.brightness);
  }
  });
  
  webServer.on("/delay", HTTP_GET, [](AsyncWebServerRequest *request)
  {

      if(request->hasParam("delay")) 
  {
    AsyncWebParameter* p = request->getParam("delay");
    cfg.led_delay = p->value().toInt();
  }
  });
  
  webServer.on("/tz_offset", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("tz_offset");
    if(request->hasParam("tz_offset")) 
    {
      AsyncWebParameter* p = request->getParam("tz_offset");
      cfg.tz_offset= p->value().toInt();
      ntp_setup();
     }
  });
  webServer.on("/pixel", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("pixel");
    if(request->hasParam("data")) 
    {
      AsyncWebParameter* p = request->getParam("data");
      //Serial.println(p->value());
      led_paint_from_string (String(p->value()));

     }
  });
  webServer.begin();
}

    /*
         int args = request->args();
      for(int i=0;i<args;i++)
      {

      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
      }
    Serial.println("POSTYPOO");
    */
