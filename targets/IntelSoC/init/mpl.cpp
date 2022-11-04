#include "setup.h"


int main (void)
{
	// Do board initialization (plls, sdram, pinmux/io, etc.. )
	// Process the launch information.
	// Update the ISW Handoff registers.
	// Setup Lightweigth HPS-to-FPGA and HPS-to-FPGA bridge 
	SetupController ();
   
   // Signal end of preloader   
   __asm("bkpt 0x70");
   
   return 0;
}
