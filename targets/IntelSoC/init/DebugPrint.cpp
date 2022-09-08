/** Loader for Intel-SoC-Family
Copyright (C) 2019-2022 Markus Klein
https://github.com/Masmiseim36/IntelSoc

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE. */


#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "DebugPrint.h"


#if defined ENABLE_DEBUG_PRINT && ENABLE_DEBUG_PRINT != 0
	#include "alt_16550_uart.h"

	#if defined CYCLONE_V_0 || defined CYCLONE_V_1
		constexpr int UartIndex = 0;
	#elif defined ARRIA10_0 || defined ARRIA10_1
		constexpr int UartIndex = 1;
	#else
		#error "Unknown Controller"
	#endif

	static constexpr ALT_16550_DEVICE_e Devicelist [] {ALT_16550_DEVICE_SOCFPGA_UART0, ALT_16550_DEVICE_SOCFPGA_UART1};
	ALT_16550_HANDLE_t handle{};

	/*! ConfigUart:
	\brief Configure a UART to use it to print debug-messages
	\param  void
	\return bool @true if successfully */
	bool ConfigUart (void)
	{
		ALT_STATUS_CODE status;
		status  = alt_16550_init				(Devicelist[UartIndex], nullptr, 0, &handle);
		status |= alt_16550_line_config_set (&handle, ALT_16550_DATABITS_8, ALT_16550_PARITY_DISABLE, ALT_16550_STOPBITS_1);
		status |= alt_16550_baudrate_set	   (&handle, ALT_16550_BAUDRATE_115200);
		status |= alt_16550_fifo_enable	   (&handle);
		status |= alt_16550_enable			   (&handle);

		return (status == ALT_E_SUCCESS);
	}

	/*! DebugPrint:
	\brief Send a message to the UART
	\param message The zero terminated string to send
	\return void */
	void DebugPrint (const char *message)
	{
		int len = strlen (message);
      DebugPrint (message, len);
   }

   void DebugPrint (const char *message, int len)
   {
//		alt_16550_fifo_write (&handle, message, strlen(message));
		ALT_STATUS_CODE status = ALT_E_SUCCESS;
		uint32_t size_tx;
		if (status == ALT_E_SUCCESS)
			status = alt_16550_fifo_size_get_tx (&handle, &size_tx);

		for (int i = 0; i < 1000; ++i)
		{
			if (status != ALT_E_SUCCESS)
				break;

			if (len == 0)
				break;

			// Wait for the THRE line status
			int j = 1000000;
			while (--j)
			{
				uint32_t line_status = 0;
				status = alt_16550_line_status_get (&handle, &line_status);
				if (status != ALT_E_SUCCESS)
					break;
				if (line_status & (ALT_16550_LINE_STATUS_THRE | ALT_16550_LINE_STATUS_TEMT))
					break;
			}
			if (j == 0)
				status = ALT_E_TMO;

			uint32_t level_tx;

			if (status == ALT_E_SUCCESS)
				status = alt_16550_fifo_level_get_tx (&handle, &level_tx);

			if (status == ALT_E_SUCCESS)
			{
				uint32_t size_write = ALT_MIN (len, size_tx - level_tx);
				status = alt_16550_fifo_write (&handle, message, size_write);
				if (status == ALT_E_SUCCESS)
				{
					len -= size_write;
					message += size_write;
				}
			}

		}
	}
	/*! DebugPrintf:
	\brief Send a formated message to the UART
	\param message The zero terminated string to send including formating information
	\param  ... Sequence of additional arguments used to replace a format specifier
	\return void */
	void DebugPrintf (const char *message, ...)
	{
		static char buffer [128];
		va_list ArgPtr;
		va_start(ArgPtr, message);
		size_t Length = vsnprintf (buffer, sizeof(buffer), message, ArgPtr);
		va_end(ArgPtr);

		if (Length > 0 && Length <= sizeof(buffer))
			DebugPrint (buffer, Length);
	}
#else
	bool ConfigUart (void)
	{
		return true;
	}

	void DebugPrint (const char *message)
	{
		(void)message;
	}

	void DebugPrintf (const char *message, ...)
	{
		(void)message;
	}
#endif // defined ENABLE_DEBUG_PRINT && ENABLE_DEBUG_PRINT != 0