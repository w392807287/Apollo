#include "Framework.h"
#include "CWaves.h"

#define NUMBUFFERS              (4)
#define	SERVICE_UPDATE_PERIOD	(20)

#define	TEST_WAVE_FILE		"stereo.wav"

int main()
{
  ALuint		    uiBuffers[NUMBUFFERS];
  ALuint		    uiSource;
  ALuint			uiBuffer;
  ALint			iState;
  CWaves *		pWaveLoader = NULL;
  WAVEID			WaveID;
  ALint			iLoop;
  ALint			iBuffersProcessed, iTotalBuffersProcessed, iQueuedBuffers;
  WAVEFORMATEX	wfex;
  unsigned long	ulDataSize = 0;
  unsigned long	ulFrequency = 0;
  unsigned long	ulFormat = 0;
  unsigned long	ulBufferSize;
  unsigned long	ulBytesWritten;
  void *			pData = NULL;

  // Initialize Framework
  ALFWInit();

  ALFWprintf("PlayStream Test Application\n");

  if (!ALFWInitOpenAL())
  {
    ALFWprintf("Failed to initialize OpenAL\n");
    ALFWShutdown();
    return 0;
  }

  // Generate some AL Buffers for streaming
  alGenBuffers(NUMBUFFERS, uiBuffers);

  // Generate a Source to playback the Buffers
  alGenSources(1, &uiSource);

  // Create instance of WaveLoader class
  pWaveLoader = new CWaves();
  if ((pWaveLoader) && (SUCCEEDED(pWaveLoader->OpenWaveFile(ALFWaddMediaPath(TEST_WAVE_FILE), &WaveID))))
  {
    pWaveLoader->GetWaveSize(WaveID, &ulDataSize);
    pWaveLoader->GetWaveFrequency(WaveID, &ulFrequency);
    pWaveLoader->GetWaveALBufferFormat(WaveID, &alGetEnumValue, &ulFormat);

    // Queue 250ms of audio data
    pWaveLoader->GetWaveFormatExHeader(WaveID, &wfex);
    ulBufferSize = wfex.nAvgBytesPerSec >> 2;

    // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
    ulBufferSize -= (ulBufferSize % wfex.nBlockAlign);

    if (ulFormat != 0)
    {
      pData = malloc(ulBufferSize);
      if (pData)
      {
        // Set read position to start of audio data
        pWaveLoader->SetWaveDataOffset(WaveID, 0);

        // Fill all the Buffers with audio data from the wavefile
        for (iLoop = 0; iLoop < 4; iLoop++)
        {
          if (SUCCEEDED(pWaveLoader->ReadWaveData(WaveID, pData, ulBufferSize, &ulBytesWritten)))
          {
            alBufferData(uiBuffers[iLoop], ulFormat, pData, ulBytesWritten, ulFrequency);
            alSourceQueueBuffers(uiSource, 1, &uiBuffers[iLoop]);
          }
        }

        // Start playing source
        alSourcePlay(uiSource);

        iTotalBuffersProcessed = 0;

        while (!ALFWKeyPress())
        {
          Sleep(SERVICE_UPDATE_PERIOD);

          // Request the number of OpenAL Buffers have been processed (played) on the Source
          iBuffersProcessed = 0;
          alGetSourcei(uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

          // Keep a running count of number of buffers processed (for logging purposes only)
          iTotalBuffersProcessed += iBuffersProcessed;
          ALFWprintf("Buffers Processed %d\r", iTotalBuffersProcessed);

          // For each processed buffer, remove it from the Source Queue, read next chunk of audio
          // data from disk, fill buffer with new data, and add it to the Source Queue
          while (iBuffersProcessed)
          {
            // Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
            uiBuffer = 0;
            alSourceUnqueueBuffers(uiSource, 1, &uiBuffer);

            // Read more audio data (if there is any)
            pWaveLoader->ReadWaveData(WaveID, pData, ulBufferSize, &ulBytesWritten);
            if (ulBytesWritten)
            {
              // Copy audio data to Buffer
              alBufferData(uiBuffer, ulFormat, pData, ulBytesWritten, ulFrequency);
              // Queue Buffer on the Source
              alSourceQueueBuffers(uiSource, 1, &uiBuffer);
            }

            iBuffersProcessed--;
          }

          // Check the status of the Source.  If it is not playing, then playback was completed,
          // or the Source was starved of audio data, and needs to be restarted.
          alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);
          if (iState != AL_PLAYING)
          {
            // If there are Buffers in the Source Queue then the Source was starved of audio
            // data, so needs to be restarted (because there is more audio data to play)
            alGetSourcei(uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
            if (iQueuedBuffers)
            {
              alSourcePlay(uiSource);
            }
            else
            {
              // Finished playing
              break;
            }
          }
        }

        // Stop the Source and clear the Queue
        alSourceStop(uiSource);
        alSourcei(uiSource, AL_BUFFER, 0);

        // Release temporary storage
        free(pData);
        pData = NULL;
      }
      else
      {
        ALFWprintf("Out of memory\n");
      }
    }
    else
    {
      ALFWprintf("Unknown Audio Buffer format\n");
    }

    // Close Wave Handle
    pWaveLoader->DeleteWaveFile(WaveID);
  }
  else
  {
    ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(TEST_WAVE_FILE));
  }

  // Clean up buffers and sources
  alDeleteSources(1, &uiSource);
  alDeleteBuffers(NUMBUFFERS, uiBuffers);

  if (pWaveLoader)
    delete pWaveLoader;

  ALFWShutdownOpenAL();

  ALFWShutdown();

  return 0;
}
