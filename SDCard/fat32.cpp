#include <string.h>
#include <stdio.h>
#include "fat32.h"
#include "conv.h"
#include "spi.h"

#define DELIM "/\\"
#define CLUS2OFF(c) (_cluster + (c - 2) * _secPerClus)

#include "tft.h"
void fat32::test(void)
{
	char path[] = "/ILMATT~1/IMAGE/STARTUP BMP";
	struct dirent *d = fopen(path);
	if (d == NULL) {
		puts("Read bmp failed!");
		return;
	}
	sd.readBlockStart(CLUS2OFF(d->d_off));
	for (uint8_t i = 0; i < 10; i++)
		spi::trans();
	uint16_t offset = spi::trans32();
	for (uint8_t i = 10 + 4; i < offset; i++)
		spi::trans();
	uint16_t x, y, b = 1;
	tft.bmp(true);
	tft.all();
	tft.start();
	for (y = 240; y > 0; y--)
		for (x = 0; x < 320; x++) {
			uint32_t c = 0;
			if (offset + 3 > 512) {
				uint8_t ext = offset + 3 - 512;
				while (ext++ != 3) {
					c >>= 8;
					c |= spi::trans() * 0x00010000;
				}
				sd.readBlockStart(CLUS2OFF(d->d_off) + b++);
				offset = offset + 3 - 512;
				for (ext = 0; ext < offset; ext++) {
					c >>= 8;
					c |= spi::trans() * 0x00010000;
				}
			} else {
				c = spi::trans24();
				offset += 3;
			}
			tft.write(conv::c32to16(c));
		}
	while (offset++ < 512)
		spi::trans();
	tft.bmp(false);
	fclose(d);
}

struct dirent *fat32::fopen(char *path)
{
	char p[strlen(path)];
	char *str = strrchr(path, '/');
	strncpy(p, path, str - path);
	p[str - path] = '\0';
	DIR *dir = opendir(p);
	if (dir == NULL)
		return NULL;
	struct dirent *d;
	do {
		d = readdir(dir);
		if (d == NULL) {
			closedir(dir);
			return NULL;
		}
	} while (strncmp(d->d_name, str + 1, 11) != 0);
	return d;
}

struct dirent *fat32::readdir(DIR *dir)
{
	if (!dir->opened()) {
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
	dir->d_off = block[index + 0x14] * 0x00010000 + \
			    block[index + 0x15] * 0x01000000 + \
			    block[index + 0x1A] + \
			    block[index + 0x1B] * 0x0100;
	dir->d_type = block[index + 0x0B];
	strncpy(dir->d_name, (char *)&block[index], 11);
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
	return dir;
}

DIR *fat32::opendir(char *path)
{
	if (path == 0) {
		_errno = ENOENT;
		return NULL;
	}
	DIR *dir;
	uint8_t d;
	for (d = 0; d < MAX_DIR_OPEN; d++)
		if (!_dir_[d].opened()) {
			dir = &_dir_[d];
			dir->open(d);
			break;
		}
	if (d == MAX_DIR_OPEN) {
		_errno = ENOMEM;
		return NULL;
	}
	dir->cluster = _root;
	dir->offset = 0;
	char *name = strtok(path, DELIM);
	if (name == NULL)
		return dir;
	struct dirent *dent;
loop:
	while ((dent = readdir(dir)) != NULL)
		if (strncasecmp(dent->d_name, name, strlen(name)) == 0) {
			for (uint8_t i = strlen(name); i < 11; i++)
				if (*(dent->d_name + i) != ' ')
					goto loop;
			dir->cluster = dent->d_off;
			dir->offset = 0;
			if ((name = strtok(NULL, DELIM)) == NULL)
				return dir;
			goto loop;
		}
	_errno = ENOENT;
	dir->close();
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
	sd.readBlockStart(p.begin());
	for (uint8_t i = 0; i < 0x0B; i++)
		spi::trans();
	if (spi::trans() != 0) {			// 0x0B
		_status = FORMAT_ERROR;
		return;
	}
	if (spi::trans() != 0x02) {			// 0x0C
		_status = FORMAT_ERROR;
		return;
	}
	// Sectors per cluster
	_secPerClus = spi::trans();			// 0x0D
	// Basic offset + Reserved sectors
	_fat = p.begin() + spi::trans16();		// 0x0E, 0x0F
	if (spi::trans() != 2) {			// 0x10
		_status = FORMAT_ERROR;
		return;
	}
	// Sectors per FAT
	for (uint8_t i = 0x11; i < 0x24; i++)
		spi::trans();
	_fatsize = spi::trans32();			// 0x24 - 0x27
	// Root directory cluster
	for (uint8_t i = 0x28; i < 0x2C; i++)
		spi::trans();
	_root = spi::trans32();				// 0x2C - 0x2F
	// FAT offset + FAT size
	for (uint16_t i = 0x30; i < 510; i++)
		spi::trans();
	if (spi::trans() != 0x55) {
		_status = SIG_ERROR;
		return;
	}
	if (spi::trans() != 0xAA) {
		_status = SIG_ERROR;
		return;
	}
	_cluster = _fat + (2 * _fatsize);
}
