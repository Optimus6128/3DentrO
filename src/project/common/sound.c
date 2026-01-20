#define ENABLE_SOUND

#include "types.h"
#include "audio.h"
#include "music.h"
#include "task.h"

#define		MAXVOICES   8
#define		NUMVOICES   8
#define		NUMCHANNELS 8
#define		NUMSAMPLERS 8
#define		MAXAMPLITUDE MAXDSPAMPLITUDE
#define		MAX_PITCH   60
#define		PITCH_RANGE 40

/*
** Allocate enough space so that you don't get stack overflows.
** An overflow will be characterized by seemingly random crashes
** that defy all attempts at logical analysis.  You might want to
** start big then reduce the size till you crash, then double it.
*/
#define STACKSIZE (10000)

#define NUMBLOCKS (64)
#define BLOCKSIZE (2048)
#define BUFSIZE (NUMBLOCKS*BLOCKSIZE)
#define NUMBUFFS  (2)
//#define MAXAMPLITUDE (0x7FFF)


/********* Globals for Thread **********/
static char *gFileName;
static int32 gSignal1;
static Item gMainTaskItem;
static int32 gNumReps;
static Item SpoolerThread;
static SoundFilePlayer *sfp;

#ifdef PROJECT_3DO
	int musicStatus = 0;
#else
	int musicStatus = 1;	// just to bootstart the main demo animation for now
#endif


/**************************************************************************
** Entry point for background thread.
**************************************************************************/

int32 PlaySoundFile (char *FileName, int32 BufSize, int32 NumReps)
{
	int32 SignalIn, SignalsNeeded;
	int32 LoopCount;

	for(LoopCount = 0; LoopCount < NumReps; LoopCount++) {
		sfp = OpenSoundFile(FileName, NUMBUFFS, BufSize);
		StartSoundFile( sfp, MAXAMPLITUDE );

		// Keep playing until no more samples
		SignalIn = 0;
		SignalsNeeded = 0;

		do {
			++musicStatus;
			if (SignalsNeeded) SignalIn = WaitSignal(SignalsNeeded);
			ServiceSoundFile(sfp, SignalIn, &SignalsNeeded);
		} while (SignalsNeeded);

		StopSoundFile(sfp);
		CloseSoundFile(sfp);
	}
	return 0;
}

void SpoolSoundFileThread( void )
{
	// Even if we do this in core init, we need to do it again here else the music won't work. Probably because this is a separate thread?
	OpenAudioFolio();

	PlaySoundFile(gFileName, BUFSIZE, gNumReps);
	SendSignal(gMainTaskItem, gSignal1);

	CloseAudioFolio();
	WaitSignal(0); // Waits forever. Don't return!
}


void startMusic(char *fileName)
{
	const int32 Priority = 180;
	gFileName = fileName;

	// Play music only once then stop
	gNumReps = 1;

	// Get parent task Item so that thread can signal back.
#ifdef PROJECT_3DO
	gMainTaskItem = KernelBase->kb_CurrentTask->t.n_Item;
#endif

	// Allocate a signal for each thread to notify parent task.
	gSignal1 = AllocSignal(0);

#ifdef PROJECT_3DO
	SpoolerThread = CreateThread("SoundSpooler", Priority, SpoolSoundFileThread, STACKSIZE);
#endif
}

void endMusic()
{
	StopSoundFile(sfp);
	CloseSoundFile(sfp);

	FreeSignal(gSignal1);
#ifdef PROJECT_3DO
	DeleteThread( SpoolerThread );
#endif

	CloseAudioFolio();
}
