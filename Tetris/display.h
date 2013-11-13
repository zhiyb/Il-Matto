#ifndef _DISPLAY_H
#define _DISPLAY_H

#define INDEX_INIT 255

void Print_unit(uint8_t x0, uint16_t y0, uint8_t x, uint8_t y, uint16_t c);
void Print_next(void);
void Print_board(void);
void Clean_object(void);
void Print_object(void);
void Print_gameover(void);
void Print_pause(void);
void Print_start(void);
void Print_frame(void);
void Print_score(void);
void Print_Speed(void);
void Print_highest(void);
void Print_saveload(uint8_t index);
void Print_failed(uint8_t index);
void Print_saving(uint8_t index);

#endif
