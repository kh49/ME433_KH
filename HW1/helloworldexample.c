/********************************************************************
Skeleton Code to blink an LED with PIC32
Andrew Long
********************************************************************/

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

// NOTE THAT BECAUSE WE USE THE BOOTLOADER, NO CONFIGURATION IS NECESSARY
// THE BOOTLOADER PROJECT ACTUALLY CONTROLS ALL OF OUR CONFIG BITS

// Let compile time pre-processor calculate the CORE_TICK_RATE
#define SYS_FREQ 				(80000000L)
#define TOGGLES_PER_SEC			1000
#define CORE_TICK_RATE	       (SYS_FREQ/2/TOGGLES_PER_SEC)

// Decriments every 1 ms.
volatile static UINT16 OneMSTimer;

int main(void)
{
	// This is in ms, and is how long we wait between blinks
	UINT16	BlinkTime = 1000;

    // Configure the proper PB frequency and the number of wait states
	SYSTEMConfigPerformance(80000000L);

    //Initialize all of the LED pins
	mInitAllLEDs();

	// Open up the core timer at our 1ms rate
	OpenCoreTimer(CORE_TICK_RATE);

    // set up the core timer interrupt with a priority of 2 and zero sub-priority
	mConfigIntCoreTimer((CT_INT_ON | CT_INT_PRIOR_2 | CT_INT_SUB_PRIOR_0));

    // enable multi-vector interrupts
	INTEnableSystemMultiVectoredInt();

    while(1)
    {
	    // What we're going to do is blink an LED on and off
	    // The rate we use will be determined by the user pushing
	    // the User button.
	    mLED_3_On();

	    OneMSTimer = BlinkTime;
	    while (OneMSTimer);

	    mLED_3_Off();
	    
	    OneMSTimer = BlinkTime;
	    while (OneMSTimer);

	    // Look for a button press to switch between the two speeds
	    if (swUser)
	    {
		    BlinkTime = 1000;		// 1second
		}
		else
		{
			BlinkTime = 100;		// 100ms
		}
    }
}

void __ISR(_CORE_TIMER_VECTOR, ipl2) CoreTimerHandler(void)
{
    // clear the interrupt flag
    mCTClearIntFlag();

	if (OneMSTimer)
	{
		OneMSTimer--;
	}

    // update the period
    UpdateCoreTimer(CORE_TICK_RATE);
}

