// GeNN robotics includes
#include "analogue_csv_recorder.h"
#include "spike_csv_recorder.h"

// Auto-generated model code
#include "chama_gan_CODE/definitions.h"

//----------------------------------------------------------------------------
// Anonymous namespace
//----------------------------------------------------------------------------
namespace
{
const float gScale = 2.0f;

void setExcitatoryWeight(unsigned int preIdx, unsigned int postIdx, scalar weight)
{
    gExcitatorySyn[(preIdx * 4) + postIdx] = weight * gScale;
}

void setInhibitoryWeight(unsigned int preIdx, unsigned int postIdx, scalar weight)
{
    gInhibitorySyn[(preIdx * 4) + postIdx] = weight * gScale;
}

void setBlueInput(scalar value)
{
    gExtInhibitorySyn[0] = -7.13f * value * 3.0f * gScale;
}

void setRedInput(scalar value)
{
    gExtExcitatorySyn[1] = 4.58f * value * 3.0f * gScale;
}
}   // Anonymous namespace

int main()
{
    const scalar leftValue = 0.55f;
    const scalar rightValue = 0.45f;
    
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
    AnalogueCSVRecorder<scalar> voltages("voltages.csv", VNeurons, 4, "Membrane voltage [mV]");

    // Loop through timesteps
    while(t < 200.0f) {
        // Apply some stimuli
        if(t > 10.0f && t < 40.0f) {
            setRedInput(leftValue);
            setBlueInput(0.0f);
        }
        else if(t > 45.0f && t < 75.0f) {
            setRedInput(0.0f);
            setBlueInput(leftValue);
        }
        else {
            setRedInput(0.0f);
            setBlueInput(0.0f);
        }
        
        // Simulate
        stepTimeCPU();

        // Record spikes and voltage
        spikes.record(t);
        voltages.record(t);

    }
    
    return 0;
}
