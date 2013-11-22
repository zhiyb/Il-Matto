#include <string.h>
#include <stdio.h>
#include "fat32.h"
#include "conv.h"

#define DELIM "/\\"
#define CLUS2OFF(c) (_cluster + (c - 2) * _secPerClus)

using namespace conv;

#include "tft.h"
void fat32::test(void)
{
	char path[] = "";
	DIR *d = opendir(path);
	struct dirent *dir;
	while (strcmp((dir = readdir(d))->d_name, "TFT     BMP") != 0);
	uint8_t block[512];
	sd.readBlock(CLUS2OFF(dir->d_off), block);
	uint16_t offset = uint32(&block[10]);
	uint16_t x, y, b = 1;
	tft.bmp(true);
	tft.all();
	tft.start();
	for (y = 240; y > 0; y--)
		for (x = 0; x < 320; x++) {
			uint32_t c = 0;
			if (offset + 3 > 512) {
				uint8_t ext = offset + 3 - 512;
				while (ext != 3) {
					c >>= 8;
					c |= block[512 - 3 + ext++] * \
					     0x00010000;
				}
				sd.readBlock(CLUS2OFF(dir->d_off) + b++, \
						block);
				offset = offset + 3 - 512;
				for (ext = 0; ext < offset; ext++) {
					c >>= 8;
					c |= block[ext] * 0x00010000;
				}
			} else {
				c = uint24(&block[offset]);
				offset += 3;
			}
			//tft.point(x, y - 1, c32to16(c));
			tft.write(c32to16(c));
		}
	tft.bmp(false);
}

FILE *fat32::fopen_read(char *path)
{
	return NULL;
}

uint32_t fat32::fatLookup(uint32_t cluster)
{
	uint8_t block[512];
	uint32_t off = cluster * 4 / 512;
	sd.readBlock(_fat + off, block);
	off = cluster * 4 - off * 512;
	return uint32(&block[off]);
}

struct dirent *fat32::readdir(DIR *dir)
{
	uint8_t i = DIRindex(dir);
	if (i == 0xFF) {
		_errno = EBADF;
		return NULL;
	}
	uint8_t block[512];
	uint16_t index;
	goto init;
read:
	index = (dir->offset % 16) * 32;
	switch (block[index]) {
	case 0xE5:	// Unused
next:
		dir->offset++;
		if (dir->offset % 16 != 0)		// Not reached boundary
			goto read;
		if (dir->offset == _secPerClus * 16) {	// Last sector
			dir->offset = 0;
			dir->cluster = fatLookup(dir->cluster);
			if (dir->cluster > 0x0FFFFFF8) {
				_errno = EBADF;
				return NULL;
			}
		}
init:
		sd.readBlock(CLUS2OFF(dir->cluster) + dir->offset / 16, block);
		goto read;
	case 0:		// End of directory
		_errno = EBADF;
		return NULL;
	default:
		switch (block[index + 0x0B]) {
		case 0x08:
		case 0x0F:
			goto next;
		}
	}
	_dirent_[i].d_off = block[index + 0x14] * 0x00010000 + \
			    block[index + 0x15] * 0x01000000 + \
			    block[index + 0x1A] + \
			    block[index + 0x1B] * 0x0100;
	_dirent_[i].d_type = block[index + 0x0B];
	strncpy(_dirent_[i].d_name, (char *)&block[index], 11);
	// Count up
	dir->offset++;
	if (dir->offset == _secPerClus * 16) {		// Last sector
		dir->offset = 0;
		dir->cluster = fatLookup(dir->cluster);
		if (dir->cluster > 0x0FFFFFF8) {
			_errno = EBADF;
			return NULL;
		}
	}
	return &_dirent_[i];
}

uint8_t fat32::closedir(DIR *dir)
{
	uint8_t i = DIRindex(dir);
	if (i == 0xFF) {
		_errno = EBADF;
		return 1;
	}
	_dir_use_ &= ~(1 << i);
	return 0;
}

DIR *fat32::opendir(char *path)
{
	if (path == 0) {
		_errno = ENOENT;
		return NULL;
	}
	DIR *dir = NULL;
	uint8_t d;
	for (d = 0; d < MAX_DIR_OPEN; d++)
		if (!(_dir_use_ & (1 << d))) {
			dir = &_dir_[d];
			break;
		}
	if (dir == NULL) {
		_errno = ENOMEM;
		return NULL;
	}
	dir->cluster = _root;
	dir->offset = 0;
	char *name = strtok(path, DELIM);
	if (name == NULL) {
		_dir_use_ |= d;
		return dir;
	}
	struct dirent *dent;
loop:
	while ((dent = readdir(dir)) != NULL) {
		if (strncasecmp(dent->d_name, name, strlen(name)) == 0) {
			for (uint8_t i = strlen(name); i < 11; i++)
				if (*(dent->d_name + i) != ' ')
					goto loop;
			dir->cluster = dent->d_off;
			dir->offset = 0;
			if ((name = strtok(NULL, DELIM)) == NULL) {
				_dir_use_ |= d;
				return dir;
			}
			goto loop;
		}
	}
	_errno = ENOENT;
	return NULL;
}

void fat32::init(const class partition& p)
{
	_status = OK;
	switch (p.type()) {
	case 0x0B:
	case 0x0C:
		break;
	default:
		_status = TYPE_ERROR;
		return;
	}
	uint8_t block[512];
	sd.readBlock(p.begin(), block);
	if (block[510] != 0x55 || block[511] != 0xAA) {
		_status = SIG_ERROR;
		return;
	}
	if (block[0x0B] != 0 || block[0x0C] != 0x02 || block[0x10] != 2) {
		_status = FORMAT_ERROR;
		return;
	}
	// Sectors per cluster
	_secPerClus = block[0x0D];
	// Root directory cluster
	_root = uint32(&block[0x2C]);
	// Basic offset + Reserved sectors
	_fat = p.begin() + block[0x0E] + block[0x0F] * 0x0100;
	// Sectors per FAT
	_fatsize = uint32(&block[0x24]);
	// FAT offset + FAT size
	_cluster = _fat + (2 * _fatsize);
}
