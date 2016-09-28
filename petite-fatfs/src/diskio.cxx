/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2009      */
/*-----------------------------------------------------------------------*/

#include <stdint.h>

extern "C"
{
#include "diskio.h"
}

#include <QFile>
#include <QDebug>


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

static QFile f(":/sys/floppy.img");
static int last_sector = -1;
char secbuf[512];

DSTATUS disk_initialize (void)
{
	DSTATUS stat = RES_OK;

	// Put your code here
	if (!f.open(QFile::ReadOnly))
		qDebug() << "error opening disk image file";

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* dest,			/* Pointer to the destination object */
	DWORD sector,		/* Sector number (LBA) */
	WORD sofs,			/* Offset in the sector */
	WORD count			/* Byte count (bit15:destination) */
)
{
	DRESULT res = RES_OK;

	if (sector != last_sector)
	{
		f.seek(sector * 512);
		f.read(secbuf, 512);
		last_sector = sector;
	}
	memcpy(dest, secbuf + sofs, count);

	// Put your code here

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	const BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res;


	if (!buff) {
		if (sc) {

			// Initiate write process

		} else {

			// Finalize write process

		}
	} else {

		// Send data to the disk

	}

	return res;
}

