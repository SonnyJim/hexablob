File fsUploadFile;
char ip_addr[15];

#define FILE_EXT ".eseq"

int freespace;

String fnameProcessor(const String& var)
{
  if(var == "combobox")
    return String(getFileCombo());
  else if (var == "fname_curr")
    return String (fname_curr);
  else if (var == "brightness")
    return String (brightness);   
  else if (var =="led_delay")
    return String (led_delay);
  return String();
}

String getFileCombo()
{
  //  <option value="mercedes">Mercedes</option>
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
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });
  webServer.onNotFound(notFound);
  webServer.serveStatic("/index.htm", LittleFS, "/index.htm");
  //webServer.serveStatic("/config", LittleFS, "/config.htm");
  
  webServer.serveStatic("/upload", LittleFS, "/upload.htm");
  
  webServer.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {

        request->send(200);
        
      }, handleUpload);
  
  webServer.on("/success", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/success.htm", String(), false, reprocessor);
  });

  /*
  webServer.on("/config", HTTP_POST, [](AsyncWebServerRequest *request)
  {

  });
  */
  
  webServer.on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    
 
  if(request->hasParam("effects")) 
  {
    AsyncWebParameter* p = request->getParam("effects");
    if (request->hasParam("delete"))
      LittleFS.remove(p->value());
    else
      openeseq(p->value());
  }
    request->send(LittleFS, "/config.htm", String(), false, fnameProcessor);
  });


  webServer.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *request)
  {

      if(request->hasParam("brightness")) 
  {
    AsyncWebParameter* p = request->getParam("brightness");
    brightness = p->value().toInt();
    LEDS.setBrightness(brightness);
  }
  });

  webServer.on("/delay", HTTP_GET, [](AsyncWebServerRequest *request)
  {

      if(request->hasParam("delay")) 
  {
    AsyncWebParameter* p = request->getParam("delay");
    led_delay = p->value().toInt();
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
