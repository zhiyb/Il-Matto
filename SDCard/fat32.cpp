#include <string.h>
#include <stdio.h>
#include "fat32.h"
#include "conv.h"
#include "spi.h"

#define CLUS2OFF(c) (fs->cluster + (c - 2) * fs->secPerClus)

class fat32_file _file_[MAX_FILE_OPEN];

bool fat32_file::open(class fat32 *f, uint8_t i, char *path)
{
	struct dirent *d = f->_fopen(path);
	if (d == NULL)
		return false;
	cluster = position = d->d_off;
	offset = 0;
	sector = 0;
	remain = size = d->d_size;
	index = i;
	fs = f;
	sd.readBlock(CLUS2OFF(cluster), buff);
	f->_fclose(d);
	return true;
}

int fat32_file::getc(void)
{
	char c = buff[offset++];
	if (!remain--) {
		remain = 0;
		return EOF;
	}
	if (offset == 512) {				// Sector end
		offset = 0;
		if (++sector == fs->secPerClus) {	// Cluster end
			sector = 0;
			cluster = fs->fatLookup(cluster);
		}
		sd.readBlock(CLUS2OFF(cluster) + sector, buff);
	}
	return (uint8_t)c;
}

#undef CLUS2OFF
#define CLUS2OFF(c) (cluster + (c - 2) * secPerClus)
#define DELIM "/\\"

static class fat32_file *fat32_file_source = NULL;

static inline int fat32_file_getc(FILE *stream)
{
	return fat32_file_source->getc();
}

FILE *fat32_filedev(class fat32_file *f)
{
	static FILE *out = NULL;
	fat32_file_source = f;
	if (out == NULL)
		out = fdevopen(NULL, fat32_file_getc);
	return out;
}

class fat32_file *fat32::fopen(char *path)
{
	for (uint8_t i = 0; i < MAX_FILE_OPEN; i++)
		if (!_file_[i].opened()) {
			if (_file_[i].open(this, i, path))
				return &_file_[i];
			else
				return NULL;
		}
	return NULL;
}

#include "tft.h"
void fat32::test(void)
{
#if 1
	char path[] = "/ILMATT~1/IMAGE/STARTUP BMP";
	struct dirent *d = _fopen(path);
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
			if (offset > 512 - 3) {
				uint8_t ext = offset + 3 - 512;
				while (ext++ != 3) {
					c >>= 8;
					c |= (uint32_t)spi::trans() << 16;
				}
				sd.readBlockStart(CLUS2OFF(d->d_off) + b++);
				offset = offset + 3 - 512;
				for (ext = 0; ext < (uint8_t)offset; ext++) {
					c >>= 8;
					c |= (uint32_t)spi::trans() << 16;
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
	_fclose(d);
	return;
#else
	char path[] = "/ILMATT~1/IMAGE/STARTUP BMP";
	class fat32_file *file = fopen(path);
	if (file == NULL) {
		puts("Read bmp failed!");
		return;
	}
	FILE *f = fat32_filedev(file);
	for (uint8_t i = 0; i < 10; i++)
		fgetc(f);
	uint16_t offset = conv::uint32(f);
	for (uint8_t i = 10 + 4; i < offset; i++)
		fgetc(f);
	uint16_t x, y;
	tft.bmp(true);
	tft.all();
	tft.start();
	for (y = 240; y > 0; y--)
		for (x = 0; x < 320; x++)
			tft.write(conv::c32to16(conv::uint24(f)));
	tft.bmp(false);
	fclose(file);
#endif
}

struct dirent *fat32::_fopen(char *path)
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
		errno = EBADF;
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
		if (dir->offset == secPerClus * 16) {	// Last sector
			dir->offset = 0;
			dir->cluster = fatLookup(dir->cluster);
			if (dir->cluster > 0x0FFFFFF8) {
				errno = EBADF;
				return NULL;
			}
		}
init:
		sd.readBlock(CLUS2OFF(dir->cluster) + dir->offset / 16, block);
		goto read;
	case 0:		// End of directory
		errno = EBADF;
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
	dir->d_size = conv::uint32(block + index + 0x1C);
	dir->d_type = block[index + 0x0B];
	strncpy(dir->d_name, (char *)block + index, 11);
	// Count up
	dir->offset++;
	if (dir->offset == secPerClus * 16) {		// Last sector
		dir->offset = 0;
		dir->cluster = fatLookup(dir->cluster);
		if (dir->cluster > 0x0FFFFFF8) {
			errno = EBADF;
			return NULL;
		}
	}
	return dir;
}

DIR *fat32::opendir(char *path)
{
	if (path == 0) {
		errno = ENOENT;
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
		errno = ENOMEM;
		return NULL;
	}
	dir->cluster = root;
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
	errno = ENOENT;
	dir->close();
	return NULL;
}

void fat32::init(const class partition& p)
{
	status = OK;
	switch (p.type()) {
	case 0x0B:
	case 0x0C:
		break;
	default:
		status = TYPE_ERROR;
		return;
	}
	sd.readBlockStart(p.begin());
	for (uint8_t i = 0; i < 0x0B; i++)
		spi::trans();
	if (spi::trans() != 0) {			// 0x0B
		status = FORMAT_ERROR;
		return;
	}
	if (spi::trans() != 0x02) {			// 0x0C
		status = FORMAT_ERROR;
		return;
	}
	// Sectors per cluster
	secPerClus = spi::trans();			// 0x0D
	// Basic offset + Reserved sectors
	fat = p.begin() + spi::trans16();		// 0x0E, 0x0F
	if (spi::trans() != 2) {			// 0x10
		status = FORMAT_ERROR;
		return;
	}
	// Sectors per FAT
	for (uint8_t i = 0x11; i < 0x24; i++)
		spi::trans();
	fatsize = spi::trans32();			// 0x24 - 0x27
	// Root directory cluster
	for (uint8_t i = 0x28; i < 0x2C; i++)
		spi::trans();
	root = spi::trans32();				// 0x2C - 0x2F
	// FAT offset + FAT size
	for (uint16_t i = 0x30; i < 510; i++)
		spi::trans();
	if (spi::trans() != 0x55) {
		status = SIG_ERROR;
		return;
	}
	if (spi::trans() != 0xAA) {
		status = SIG_ERROR;
		return;
	}
	cluster = fat + (2 * fatsize);
}
