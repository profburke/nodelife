#ifndef CA_H
#define CA_H

#include <stdint.h>

#define LIVE (1)
#define DEAD (0)

void clearGrids();

void set(uint8_t gridIndex, uint8_t x, uint8_t y);
void clear(uint8_t gridIndex, uint8_t x, uint8_t y);
uint8_t get(uint8_t gridIndex, uint8_t x, uint8_t y);

void grid2screen(uint8_t gridIndex);
void randomFill(uint8_t gridIndex, uint8_t p);

void swapGrids();
void showGrid();

uint8_t calculate(uint8_t currentValue, uint8_t nNeighbors);
uint8_t countNeighbors(uint8_t gridIndex, uint8_t x, uint8_t y);

void updateBuffer();

void resetCA();

#endif

