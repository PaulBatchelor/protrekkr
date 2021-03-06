/*
   How to use:

   1. Copy the .ptp and the Ptk_Properties.h file into the "Replay" directory.
   2. Edit the module.macosx file and replace the YOUR_MODULE.PTP with
      the filename of your module file.
   3. Compile the replay routine (which will be fine tuned for your module).
   4. Compile this test example.

   Note: LATENCY should be increased if the cpu is extensively used for other purposes
         to avoid lags and stuttering.
*/

#include <unistd.h>

#include "../lib/include/ptkreplay.h"

extern "C"
{
    extern unsigned int _PTK_MODULE;
}

/* Initialize with 20 milliseconds of latency */
#define LATENCY 20

int main(void)
{
    if(!Ptk_InitDriver(ptk, LATENCY)) return(0);

    // Load it
    if(!Ptk_InitModule((unsigned char *) &_PTK_MODULE, 0))
    {
        Ptk_ReleaseDriver(ptk);
        return(0);
    }

    // Start playing it
    Ptk_Play(ptk);

    while(1)
    {
        usleep(10);
    }

    Ptk_Stop(ptk);
    Ptk_ReleaseDriver(ptk);
}
