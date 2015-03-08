#include <stdlib.h>
#include <string.h>
#include "fat32.h"
#include "errno.h"
#include "conv.h"

fat32_t::fat32_t(hw_t *hardware, const uint32_t addr) : fs_t()
{
	hw = hardware;
	if (!hw->streamStart(hw_t::Read, addr)) {
		errno = 1;
		return;
	}
	hw->skipBytes(0x0B);
	if (hw->nextBytes(2) != 512) {			// Bytes per sector
		errno = 2;
		goto ret;
	}
	secPerClus = hw->nextByte();			// Sectors per cluster
	FATAddr = addr + hw->nextBytes(2);		// Number of reserved sectors
	if (hw->nextByte() != 2) {			// Number of FATs
		errno = 3;
		goto ret;
	}
	hw->skipBytes(0x24 - 0x10 - 1);
	clusAddr = FATAddr + 2 * hw->nextBytes(4);	// Sectors per FAT
	hw->skipBytes(0x2C - (0x24 + 3) - 1);
	rootClus = hw->nextBytes(4);			// Root directory first cluster
	hw->skipBytes(0x1FE - (0x2C + 3) - 1);
	if (hw->nextBytes(2) != 0xAA55) {		// Signature
		errno = 4;
		goto ret;
	}
	errno = 0;
	counter = 0xFF;
ret:
	if (!hw->streamStop(hw_t::Read))
		errno = 5;
}

bool fat32_t::readDirEntry(DIR *dir, struct dirent *ent)
{
	uint8_t buff[12], index = 0;
	chainRead(dir->addr, dir->offset);
newEntry:
	dir->offset += 32;
	for (uint8_t i = 0; i < 12; i++)
		buff[i] = chainRead();
	if (buff[0] == 0xE5) {				// Deleted items
		for (uint8_t i = 0; i < 20; i++)
			chainRead();
		goto newEntry;
	}
	if (buff[0] == 0x00) {				// End of directory chain
		chainReadClose();
		return false;
	}
	if (buff[11] == 0x0F) {				// DIR_Attr field == Long file name
		uint8_t seq = buff[0] &= ~0x40;
		ent->d_name[seq * 13] = '\0';
		while (seq--) {
			dir->offset += 32;
			index = 0;
			for (uint8_t i = 0; i < 5; i++)
				ent->d_name[seq * 13 + index++] = conv::ucs2decode(buff[i * 2 + 1], buff[i * 2 + 2]);
			chainRead();			// Type, always 0x00
			chainRead();			// Checksum of DOS file name
			for (uint8_t i = 0; i < 6; i++) {
				char a = chainRead(), b = chainRead();
				ent->d_name[seq * 13 + index++] = conv::ucs2decode(a, b);
			}
			chainRead();			// First cluster
			chainRead();			// Always 0x0000
			char a = chainRead(), b = chainRead();
			ent->d_name[seq * 13 + index++] = conv::ucs2decode(a, b);
			a = chainRead(), b = chainRead();
			ent->d_name[seq * 13 + index++] = conv::ucs2decode(a, b);
			for (uint8_t i = 0; i < 12; i++)
				buff[i] = chainRead();	// For next loop
		}
		chainRead();				// Type byte
	} else						// 8.3 format short file name
		if (buff[11] & IS_DIR) {
			uint8_t type = chainRead();	// Type byte
			for (uint8_t i = 11; i > 0; i--)
				if (buff[i - 1] != ' ')
					break;
				else
					buff[i - 1] = '\0';
			for (uint8_t i = 0; i < 11; i++)
				ent->d_name[index++] = (type & (1 << 3)) ? tolower(buff[i]) : buff[i];
		} else {
			uint8_t type = chainRead();	// Type byte
			for (uint8_t i = 8; i > 0; i--)
				if (buff[i - 1] != ' ')
					break;
				else
					buff[i - 1] = '\0';
			for (uint8_t i = 0; i < 8 && buff[i] != '\0'; i++)
				ent->d_name[index++] = (type & (1 << 3)) ? tolower(buff[i]) : buff[i];
			uint8_t i;
			for (i = 11; i > 8; i--)
				if (buff[i - 1] != ' ')
					break;
				else
					buff[i - 1] = '\0';
			if (i != 8)
				ent->d_name[index++] = '.';
			for (i = 0; i < 3; i++)
				ent->d_name[index++] = (type & (1 << 4)) ? tolower(buff[i + 8]) : buff[i + 8];
			ent->d_name[index++] = '\0';
		}
	ent->d_type = buff[11];
	for (uint8_t i = 0; i < 7; i++)
		chainRead();
	ent->d_addr = (uint32_t)chainRead() << 16;
	ent->d_addr |= (uint32_t)chainRead() << 24;
	for (uint8_t i = 0; i < 4; i++)
		chainRead();
	ent->d_addr |= (uint32_t)chainRead();
	ent->d_addr |= (uint32_t)chainRead() << 8;
	ent->d_size = (uint32_t)chainRead();
	ent->d_size |= (uint32_t)chainRead() << 8;
	ent->d_size |= (uint32_t)chainRead() << 16;
	ent->d_size |= (uint32_t)chainRead() << 24;
	chainReadClose();
	if (curClus != dir->addr) {
		dir->addr = curClus;
		dir->offset %= secPerClus * 512;
	}
	return true;
}

bool fat32_t::readFileInfo(const char *path, file_t *file)
{
	char *dirpath = (char *)malloc(strlen(path) + 1);
	strcpy(dirpath, path);
	char *p = dirpath, *last = 0;
	while (*p != '\0') {
		if (*p == '/')
			last = p;
		p++;
	}
	DIR *dir;
	if (last != 0) {
		*last++ = '\0';
		dir = op::opendir(dirpath);
	} else
		dir = op::opendir("");
	if (dir == NULL) {
		free(dirpath);
		::errno = ENOENT;
		return false;
	}
	struct dirent *ent;
	while ((ent = op::readdir(dir)) != NULL)
		if (strcasecmp(ent->d_name, last) == 0) {
			if (ent->d_type & IS_DIR)
				goto failed;
			file->addr = ent->d_addr;
			file->offset = 0;
			file->size = ent->d_size;
			free(dirpath);
			op::closedir(dir);
			::errno = 0;
			return true;
		}
failed:
	free(dirpath);
	op::closedir(dir);
	::errno = ENOENT;
	return false;
}
