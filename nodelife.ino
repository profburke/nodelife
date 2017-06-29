/*****************************************************
 *
 * nodelife
 *
 * Copyright (c) 2017 Matthew M. Burke.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * 
 * Conway's Game of Life implemented for an SSD1106 and ESP8266
 *
 * See https://github.com/profburke/nodelife/issues for issues, suggestions, etc.
 *
 *****************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include "SSD1306.h"

const char *ssid = "nodelife";
const char *password = "itsybitsyspider";

ESP8266WebServer server(80);
SSD1306  display(0x3c, D3, D5);

#define LIVE (1)
#define DEAD (0)

#define STATUSHEIGHT (16)
#define LCDWIDTH (DISPLAY_WIDTH)
#define LCDHEIGHT (DISPLAY_HEIGHT - STATUSHEIGHT)

#define ONBOARD(x, y) \
  ( ((x) >= 0) && ((x) < LCDWIDTH) && ((y) >= 0) && ((y) < LCDHEIGHT) )

#define VALID_GRID(idx) \
  ( (idx == 0) || (idx == 1) )

#define COORDINATES_TO_SCREENBYTE(x, y) \
  ((x) + ((y)/8) * LCDWIDTH)

static uint8_t grid[2][LCDWIDTH * (LCDHEIGHT/8)];
static uint8_t CURRENT = 0;
static uint8_t NEXT = 1;
static uint8_t p = 20;

void clearGrids() {
  memset(grid, 0, sizeof(grid));
}

// TODO: replace with memcpy and getScreenBuffer
void grid2screen(uint8_t gridIndex) {
  if (!VALID_GRID(gridIndex)) { return; }
  
  for (uint8_t x = 0; x < LCDWIDTH; x++) {
    for (uint8_t y = 0; y < LCDHEIGHT; y++) {
      uint8_t value = get(gridIndex, x, y);
      display.setColor((value % 2 == 0) ? BLACK : WHITE);
      display.setPixel(x, y);
    }
  }
}

// TODO: clean up this function, clear(), and get()
// by making use of bitClear(), bitSet(), bitWrite(), and bitRead()
void set(uint8_t gridIndex, uint8_t x, uint8_t y) {
  if (!ONBOARD(x, y)) { return; }
  if (!VALID_GRID(gridIndex)) { return; }
  
  grid[gridIndex][COORDINATES_TO_SCREENBYTE(x, y)] |= _BV((y % 8));
}


void clear(uint8_t gridIndex, uint8_t x, uint8_t y) {
  if (!ONBOARD(x, y)) { return; }
  if (!VALID_GRID(gridIndex)) { return; }

  grid[gridIndex][COORDINATES_TO_SCREENBYTE(x, y)] &= ~_BV((y % 8));
}

uint8_t get(uint8_t gridIndex, uint8_t x, uint8_t y) {
  if (!ONBOARD(x, y)) { return DEAD; }
  if (!VALID_GRID(gridIndex)) { return DEAD; }

  uint8_t contents = grid[gridIndex][COORDINATES_TO_SCREENBYTE(x, y)];

  return ( (contents & _BV((y % 8))) > 0);
}

void randomFill(uint8_t gridIndex, uint8_t p) {
  if (!VALID_GRID(gridIndex)) { return; }

  for (uint8_t x = 0; x < LCDWIDTH; x++) {
    for (uint8_t y = 0; y < LCDHEIGHT; y++) {
      if (random(100) < p) {
        set(gridIndex, x, y);
      } else {
        clear(gridIndex, x, y);
      }
    }
  }
}

void swapGrids() {
  CURRENT = 1 - CURRENT; 
  NEXT = 1 - NEXT; 
}

void showGrid() {
  grid2screen(CURRENT);
  display.display();
}

uint8_t calculate(uint8_t currentValue, uint8_t nNeighbors) {
  if ( (currentValue == LIVE) && ((nNeighbors == 2) || (nNeighbors == 3)) ) { return LIVE; }

  if ( /* currentValue == DEAD && */ (nNeighbors == 3) ) { return LIVE; }

  return DEAD;
}

uint8_t countNeighbors(uint8_t gridIndex, uint8_t x, uint8_t y) {
  uint8_t count = 0;
  
  count += get(gridIndex, x - 1, y - 1);
  count += get(gridIndex, x - 1, y);
  count += get(gridIndex, x - 1, y + 1);
  count += get(gridIndex, x, y - 1);
  count += get(gridIndex, x, y + 1);
  count += get(gridIndex, x + 1, y - 1);
  count += get(gridIndex, x + 1, y);
  count += get(gridIndex, x + 1, y + 1);
  
  return count;
}

void updateBuffer() {
  for (uint8_t x = 0; x < LCDWIDTH; x++) {
    for (uint8_t y = 0; y < LCDHEIGHT; y++) {
      uint8_t currentValue = get(CURRENT, x, y);
      uint8_t nNeighbors = countNeighbors(CURRENT, x, y);
      uint8_t newValue = calculate(currentValue, nNeighbors);
      if (newValue == LIVE) {
        set(NEXT, x, y);
      } else {
        clear(NEXT, x, y);
      }
    }
  }
  swapGrids();
}

void erase(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  OLEDDISPLAY_COLOR currentColor = display.getColor();
  display.setColor(BLACK);
  display.fillRect(x, y, w, h);
  display.setColor(currentColor);  
}

static uint16_t generation = 0;

char msg[32];
void displayStep() {
  display.setColor(WHITE);
  sprintf(msg, "step: %d", generation);
  uint16_t textWidth = display.getStringWidth(msg);
  erase(4, LCDHEIGHT + 2, textWidth, 12);
  display.drawString(4, LCDHEIGHT + 2, msg);
}

uint8_t _runCA = 1;
uint8_t _resetCA = 0;

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

void togglePaused() {
  OLEDDISPLAY_COLOR currentColor = display.getColor();
  if (!_runCA) {
    display.setColor(WHITE);
  } else {
    display.setColor(BLACK);
  }
  sprintf(msg, "PAUSED");
  uint16_t textWidth = display.getStringWidth(msg);
  uint16_t x = LCDWIDTH - textWidth - 3;
  display.drawString(x, LCDHEIGHT + 2, msg);
  display.display();
  display.setColor(currentColor);
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

// TODO: web methods to set pixels in grid
// TODO: jazz up web display to have a running/paused indicator (and a step count ???)
// TODO: allow optional AP versus Client (versus both ???)
// TODO: start up with a Logo first ....
// TODO: auto reset at 10000 (or ???) steps?
// TODO: add host name using mDNS
// TODO: use raw string or file system for better HTML pages


void initStatusArea() {
  display.drawRect(0, LCDHEIGHT, LCDWIDTH - 1, STATUSHEIGHT);  
}

void initDisplay() {
  display.init();
  display.setContrast(256);
  display.clear();
  initStatusArea();
}

void initAP() {  
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
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

void resetCA() {
  generation = 0;
  _runCA = 1;
  _resetCA = 0;
  erase(4, LCDHEIGHT + 2, LCDWIDTH - 8, 12);
  randomFill(CURRENT, p);  
  showGrid();
  displayStep();
}

void setup() {
  delay(1000);
  randomSeed(analogRead(0));
  Serial.begin(115200);
  
  initDisplay();
  initAP();
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

