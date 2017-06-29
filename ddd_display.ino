
void erase(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  OLEDDISPLAY_COLOR currentColor = display.getColor();
  display.setColor(BLACK);
  display.fillRect(x, y, w, h);
  display.setColor(currentColor);  
}

char msg[32];
void displayStep() {
  display.setColor(WHITE);
  sprintf(msg, "step: %d", generation);
  uint16_t textWidth = display.getStringWidth(msg);
  erase(4, LCDHEIGHT + 2, textWidth, 12);
  display.drawString(4, LCDHEIGHT + 2, msg);
}

void initStatusArea() {
  display.drawRect(0, LCDHEIGHT, LCDWIDTH - 1, STATUSHEIGHT);  
}

void initDisplay() {
  display.init();
  display.setContrast(256);
  display.clear();
  initStatusArea();
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


