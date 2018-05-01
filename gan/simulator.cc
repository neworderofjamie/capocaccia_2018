// GeNN robotics includes
#include "spike_csv_recorder.h"

// Auto-generated model code
#include "chama_gan_CODE/definitions.h"

//----------------------------------------------------------------------------
// Anonymous namespace
//----------------------------------------------------------------------------
namespace
{
void setExcitatoryWeight(unsigned int preIdx, unsigned int postIdx, scalar weight)
{
    gExcitatorySyn[(preIdx * 4) + postIdx] = weight;
}

void setInhibitoryWeight(unsigned int preIdx, unsigned int postIdx, scalar weight)
{
    gInhibitorySyn[(preIdx * 4) + postIdx] = weight;
}
}   // Anonymous namespace

int main()
{
    allocateMem();
    initialize();
  
    // Configure weights
    setExcitatoryWeight(0, 0, 6.92f);
    setExcitatoryWeight(0, 2, 8.17f);
    
    setExcitatoryWeight(1, 0, 4.89f);
    setExcitatoryWeight(1, 1, 0.52f);
    setExcitatoryWeight(1, 2, 2.6f);
    setExcitatoryWeight(1, 3, 7.65f);
    
    initchama_gan();
  
    // Open CSV output files
    SpikeCSVRecorder spikes("spikes.csv", glbSpkCntNeurons, glbSpkNeurons);

    // Loop through timesteps
    for(unsigned int i = 0; i < 10000; i++)
    {
        // Simulate
#ifndef CPU_ONLY
        stepTimeGPU();

        pullECurrentSpikesFromDevice();
        //pullIEStateFromDevice();
#else
        stepTimeCPU();
#endif

        spikes.record(t);

    }
    
    return 0;
}
