#include <cstring>
#include "setup.h"
#include "DebugPrint.h"
extern "C"
{
	#include <libmem.h>
	#include <libmem_loader.h>
}
#include "alt_qspi.h"

static int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size);
static int libmem_Read        (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static uint32_t libmem_CRC32  (libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc);
static int libmem_Flush       (libmem_driver_handle_t *h);

static int EraseSector        (libmem_driver_handle_t *h, libmem_sector_info_t *si);
static int ProgramPage        (libmem_driver_handle_t *h, uint8_t *dest_addr, const uint8_t *src_addr);


// Define the structure of the Flash (Sector Count and Size)
// Spansion S25FL256L - 32 Mebabyte
static libmem_geometry_t geometry[]
{	// count - size
	{0x2000, 4096}, // --> 8192 sectors with 4 Kilobytes each
	{0, 0} 
};

libmem_driver_paged_write_ctrlblk_t PageWriteControlBlock{};

static constexpr uint32_t FlashPageSize  = 256U;
static uint8_t write_buffer[FlashPageSize];

static const libmem_driver_functions_t DriverFunctions
{
	libmem_ProgramPage,
	nullptr,
	libmem_EraseSector,
	nullptr,
	nullptr,
	libmem_Flush
};

static const libmem_ext_driver_functions_t DriverFunctions_Extended
{
	nullptr,
	libmem_Read,
	libmem_CRC32
};

extern uint32_t __DDR_segment_start__[];
const uint32_t RamStart = reinterpret_cast<uint32_t>(&__DDR_segment_start__[0]);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmain"
int main ([[maybe_unused]]uint32_t flags, uint32_t param)
{
#pragma GCC diagnostic pop
	// Do board initialization (plls, pinmux/io, DDR-RAM (not Arria-10) etc.. )
	// Process the launch information.
	// Update the ISW Handoff registers.
	// Setup Lightweight HPS-to-FPGA and HPS-to-FPGA bridge 
	SetupController ();

	ConfigUart ();
	DebugPrintf ("Hello Intel SoC Flash-Loader\r\nCalled with Param 0x%X\r\n", param);

	alt_qspi_init ();
	alt_qspi_enable ();

	libmem_driver_handle_t flashHandle;
	// The QSPI-Flash is not memmory-mapped. Use the DDR-Address-Range instead
	libmem_register_driver (&flashHandle, (uint8_t *)RamStart, 0x2000000, geometry, nullptr, &DriverFunctions, &DriverFunctions_Extended);
	libmem_driver_paged_write_init (&PageWriteControlBlock, write_buffer, sizeof(write_buffer), ProgramPage, 4, LIBMEM_DRIVER_PAGED_WRITE_OPTION_DISABLE_PAGE_PRELOAD);

	#ifdef DEBUG
		// testcode for libmem driver
		static uint32_t TestMem [4096/sizeof(uint32_t)];
		static const uint8_t *MemPointer = (const uint8_t *)(1024*1024);
		uint8_t *erase_start = 0;
		size_t erase_size = 0;
		libmem_erase ((uint8_t *)MemPointer, 4096, &erase_start, &erase_size);

		libmem_read (reinterpret_cast<uint8_t *>(&TestMem), MemPointer, 4096);

		for (int i=0; i<sizeof(TestMem)/sizeof(TestMem[0]); i++)
			TestMem[i] = 0xDEADBEEF;

		libmem_write ((uint8_t *)MemPointer, (const uint8_t *)TestMem, 4088);
		libmem_read (reinterpret_cast<uint8_t *>(&TestMem), MemPointer, 4096);
		libmem_erase ((uint8_t *)MemPointer, 4096, &erase_start, &erase_size);
		libmem_read (reinterpret_cast<uint8_t *>(&TestMem), MemPointer, 4096);
	#else
		extern uint8_t __OCRAM_segment_used_end__;
		extern uint8_t __OCRAM_segment_end__;
		libmem_rpc_loader_start (&__OCRAM_segment_used_end__, &__OCRAM_segment_end__ - 1);
	#endif

	return 0;
}

static int ProgramPage ([[maybe_unused]]libmem_driver_handle_t *h, uint8_t *dest_addr, const uint8_t *src_addr)
{
	DebugPrintf ("Programm page at 0x%X\r\n", dest_addr);
//	memcpy (dest_addr, src_addr, FlashPageSize);

	if (alt_qspi_write (reinterpret_cast<uint32_t>(dest_addr) - RamStart, src_addr, 256) == ALT_E_SUCCESS)
		return LIBMEM_STATUS_SUCCESS;
	return LIBMEM_STATUS_ERROR;
}
/*! libmem_ProgramPage:
The LIBMEM driver's write function.
\param h    A pointer to the handle of the LIBMEM driver.
\param dest A pointer to the memory address in memory range handled by driver to write data to.
\param src  pointer to the memory address to read data from.
\param size The number of bytes to write.
\return int The LIBMEM status result */
static int libmem_ProgramPage ([[maybe_unused]]libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
	DebugPrintf ("libmem_ProgramPage at 0x%x - size: %d\r\n", dest, size);
	return libmem_driver_paged_write (h, dest, src, size, &PageWriteControlBlock);
}

static int EraseSector ([[maybe_unused]]libmem_driver_handle_t *h, libmem_sector_info_t *si)
{
	DebugPrintf ("Erase setor at 0x%X\r\n", si->start);
//	memset (si->start, 0xFF, si->size); // Set DDR memory to 0xFF

	if (alt_qspi_erase_sector (reinterpret_cast<uint32_t>(si->start) - RamStart) == ALT_E_SUCCESS)
		return LIBMEM_STATUS_SUCCESS;
	return LIBMEM_STATUS_ERROR;
}
/*! libmem_EraseSector:
The LIBMEM driver's erase function
\param h           A pointer to the handle of the LIBMEM driver.
\param start       A pointer to the initial memory address in memory range handled by driver to erase.
\param size        The number of bytes to erase.
\param erase_start A pointer to a location in memory to store a pointer to the start of the memory range that has actually been erased or nullptr if not required.
\param erase_size  A pointer to a location in memory to store the size in bytes of the memory range that has actually been erased or nullptr if not required.
\return int        The LIBMEM status result */
static int libmem_EraseSector ([[maybe_unused]]libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size)
{
	DebugPrintf ("libmem_EraseSector at 0x%x - size: %d\r\n", start, size);
	return libmem_foreach_sector_in_range (h, start, size, EraseSector, erase_start, erase_size);
}
/*! libmem_Flush:
The LIBMEM driver's flush function.
\param h    A pointer to the handle of the LIBMEM driver. 
\return int The LIBMEM status result */
//static int libmem_Flush ([[maybe_unused]]libmem_driver_handle_t *h)
//{
//	DebugPrint ("libmem_Flush");
//	return LIBMEM_STATUS_SUCCESS;
//}


/*! libmem_Read
\brief The LIBMEM driver's read extended function
\param h    A pointer to the handle of the LIBMEM driver.
\param dest A pointer to the initial memory address to write data to.
\param src  A pointer to the initial memory address in the memory range handled by the driver to read data from.
\param size The number of bytes to write.
\return     The LIBMEM status result.
The driver's \a read function is an optional extended function.
It has been provided to allow you to write a driver for memory that is not memory mapped.

Typically memory read operations will be direct memory mapped operations however implementing a driver's \a read function
allows you to access non-memory mapped memory through the LIBMEM interface. */
static int libmem_Read ([[maybe_unused]]libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
	DebugPrintf ("libmem_Read at 0x%X\r\n", src);
//	memcpy (dest, src, size);

	if (alt_qspi_read (dest, reinterpret_cast<uint32_t>(src) - RamStart, size) == ALT_E_SUCCESS)
		return LIBMEM_STATUS_SUCCESS;
	return LIBMEM_STATUS_ERROR;
}

/*! libmem_CRC32
\brief The LIBMEM driver's crc32 extended function
\b \this is a function pointer to a LIBMEM driver's crc32 extended function.
\param h     A pointer to the handle of the LIBMEM driver.
\param start A pointer to the start of the address range.
\param size  The size of the address range in bytes.
\param crc   The initial CRC-32 value.
\return      The computed CRC-32 value.
The driver's \a crc function is an optional extended function.
It has been provided to allow you to write a driver for memory that is not memory mapped.

Typically memory read operations will be direct memory mapped  operations however implementing a driver's \a crc function
allows you to carry out a crc32 operation on non-memory mapped memory through the LIBMEM interface. */
static uint32_t libmem_CRC32 ([[maybe_unused]]libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc)
{
	DebugPrintf ("libmem_CRC32 at 0x%X - size: %d\r\n", start, size);

	static uint8_t page_buffer[4096];
	static constexpr size_t BufferSize = sizeof (page_buffer);

	while (size >= BufferSize)
	{
		libmem_Read(h, page_buffer, (uint8_t*)start, BufferSize);
		crc = libmem_crc32_direct(page_buffer,  BufferSize, crc);
		start += BufferSize;
		size  -= BufferSize;
	}
	if (size)
	{
		libmem_Read(h, page_buffer, (uint8_t*)start, BufferSize);
		crc = libmem_crc32_direct(page_buffer, size, crc);
	}
	return crc;
}


/*! libmem_Flush
\brief The LIBMEM driver's flush function
\param h A pointer to the handle of the LIBMEM driver.
\return  The LIBMEM status result.
The driver's \a flush function should complete any outstanding memory operations (if any) and return the memory to read mode.
If this operation is not required the function should return \a LIBMEM_STATUS_SUCCESS. */
static int libmem_Flush (libmem_driver_handle_t *h)
{
	DebugPrint ("libmem_Flush\r\n");

	return libmem_driver_paged_write_flush (h, &PageWriteControlBlock);
}