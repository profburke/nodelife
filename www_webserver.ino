void handleRoot() {
  Serial.println("received HTTP request");
  server.send(200, "text/html", "<h1>NodeLife</h1>\
  \
  Commands:</br>\
  reset, start, stop, ....\
  ");
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


