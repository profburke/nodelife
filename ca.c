#include "ca.h"

#define ONBOARD(x, y) \
  ( ((x) >= 0) && ((x) < LCDWIDTH) && ((y) >= 0) && ((y) < LCDHEIGHT) )

#define VALID_GRID(idx) \
  ( (idx == 0) || (idx == 1) )

#define COORDINATES_TO_SCREENBYTE(x, y) \
  ((x) + ((y)/8) * LCDWIDTH)

static uint8_t grid[2][LCDWIDTH * (LCDHEIGHT / 8)];
static uint8_t CURRENT = 0;
static uint8_t NEXT = 1;
static uint8_t p = 20;

void clearGrids() {
  memset(grid, 0, sizeof(grid));
}

// TODO: clean up this function, clear(), and get()
// by making use of bitClear(), bitSet(), bitWrite(), and bitRead()
void set(uint8_t gridIndex, uint8_t x, uint8_t y) {
  if (!ONBOARD(x, y)) {
    return;
  }
  if (!VALID_GRID(gridIndex)) {
    return;
  }

  grid[gridIndex][COORDINATES_TO_SCREENBYTE(x, y)] |= _BV((y % 8));
}

void clear(uint8_t gridIndex, uint8_t x, uint8_t y) {
  if (!ONBOARD(x, y)) {
    return;
  }
  if (!VALID_GRID(gridIndex)) {
    return;
  }

  grid[gridIndex][COORDINATES_TO_SCREENBYTE(x, y)] &= ~_BV((y % 8));
}

uint8_t get(uint8_t gridIndex, uint8_t x, uint8_t y) {
  if (!ONBOARD(x, y)) {
    return DEAD;
  }
  if (!VALID_GRID(gridIndex)) {
    return DEAD;
  }

  uint8_t contents = grid[gridIndex][COORDINATES_TO_SCREENBYTE(x, y)];

  return ( (contents & _BV((y % 8))) > 0);
}

// TODO: replace with memcpy and getScreenBuffer
void grid2screen(uint8_t gridIndex) {
  if (!VALID_GRID(gridIndex)) {
    return;
  }

  for (uint8_t x = 0; x < LCDWIDTH; x++) {
    for (uint8_t y = 0; y < LCDHEIGHT; y++) {
      uint8_t value = get(gridIndex, x, y);
      display.setColor((value % 2 == 0) ? BLACK : WHITE);
      display.setPixel(x, y);
    }
  }
}

void randomFill(uint8_t gridIndex, uint8_t p) {
  if (!VALID_GRID(gridIndex)) {
    return;
  }

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
  if ( (currentValue == LIVE) && ((nNeighbors == 2) || (nNeighbors == 3)) ) {
    return LIVE;
  }

  if ( /* currentValue == DEAD && */ (nNeighbors == 3) ) {
    return LIVE;
  }

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

void resetCA() {
  generation = 0;
  _runCA = 1;
  _resetCA = 0;
  erase(4, LCDHEIGHT + 2, LCDWIDTH - 8, 12);
  randomFill(CURRENT, p);
  showGrid();
  displayStep();
}



