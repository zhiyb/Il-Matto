#include "dir.h"

DIR _dir_[MAX_DIR_OPEN];
struct dirent _dirent_[MAX_DIR_OPEN];
uint8_t _dir_use_ = 0;
