File fsUploadFile;
char ip_addr[15];

void notFound(AsyncWebServerRequest *request) {
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

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    Serial.printf("UploadStart: %s\n", filename.c_str());
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
  webServer.serveStatic("/config", LittleFS, "/config.htm");
  
  webServer.serveStatic("/upload", LittleFS, "/upload.htm");
  webServer.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
      }, handleUpload);
  
  webServer.on("/success", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/success.htm", String(), false, reprocessor);
  });
  webServer.on("/replace", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/replace.htm", String(), false, reprocessor);
  });

   webServer.on("/IDidAPoo", HTTP_GET, [](AsyncWebServerRequest *request){
   Serial.println ("I DONE A STEEEEENKY POO");
  });

   webServer.on("/brightness", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    int args = request->args();
for(int i=0;i<args;i++){
  Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
}
  });
  
  webServer.begin();
}
