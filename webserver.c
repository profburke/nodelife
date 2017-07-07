static const char PROGMEM rootPage[] = R"rawliteral(
<h2>NodeLife</h2>
<h3>Conway's Life on an ESP8266 and SSD1106</h3>

Commands: </br>
<ul>
<li><a href="/reset">reset</a></li>
<li><a href="/start">start</a></li>
<li><a href="/stop">stop</a></li>
</ul>

)rawliteral";

//void handleRoot() {
//  Serial.println("received HTTP request");
//  server.send(200, "text/html", "<h1>NodeLife</h1>\
//  \
//  Commands:</br>\
//  reset, start, stop, ....\
//  ");
//}

void handleRoot() {
  Serial.println("received HTTP request");
  server.send(200, "text/html", rootPage);
}

void handleReset() {
  _resetCA = 1;
  _runCA = 1;
  handleRoot();
}

void handleStop() {
  _runCA = 0;
  togglePaused();
  handleRoot();
}

void handleStart() {
  _runCA = 1;
  togglePaused();
  handleRoot();
}

void handleConfigure() {
  for (int i = 0; i < server.args(); i++) {
    // p = probability of living cell in random setup
    if (server.argName(i) == "p") {
      p = server.arg(i).toInt();
    }
  }
  handleRoot();
}

void initWebServer() {
  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  server.on("/restart", handleReset);
  server.on("/stop", handleStop);
  server.on("/start", handleStart);
  server.on("/configure", handleConfigure);
  server.begin();
  Serial.println("HTTP server started");
}


