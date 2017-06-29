void initAccessPoint() {  
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void setup() {
  randomSeed(analogRead(0));

  delay(1000);
  Serial.begin(115200);
  
  initDisplay();
  initAccessPoint();
  initWebServer();  
  
  clearGrids();
  resetCA();

  delay(150);
}

void loop() {
  if (_runCA) {
    if (_resetCA) {
      resetCA();
    } else {
     updateBuffer();
     displayStep();
     generation += 1;
     showGrid();
   }
  }
  
  server.handleClient();
  delay(250);
}

