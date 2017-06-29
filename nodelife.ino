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

#define STATUSHEIGHT (16)
#define LCDWIDTH (DISPLAY_WIDTH)
#define LCDHEIGHT (DISPLAY_HEIGHT - STATUSHEIGHT)

static uint16_t generation = 0;

uint8_t _runCA = 1;
uint8_t _resetCA = 0;

// TODO: web methods to set pixels in grid
// TODO: jazz up web display to have a running/paused indicator (and a step count ???)
// TODO: allow optional AP versus Client (versus both ???)
// TODO: start up with a Logo first ....
// TODO: auto reset at 10000 (or ???) steps?
// TODO: add host name using mDNS
// TODO: use raw string or file system for better HTML pages


