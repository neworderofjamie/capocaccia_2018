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
void setExcitatoryWeight(unsigned int preIdx, unsigned int postIdx, scalar weight)
{
    gExcitatorySyn[(preIdx * 4) + postIdx] = weight;
}

void setInhibitoryWeight(unsigned int preIdx, unsigned int postIdx, scalar weight)
{
    gInhibitorySyn[(preIdx * 4) + postIdx] = weight;
}

void setBlueInput(scalar value)
{
    iExtNeurons[0] = -7.13f * value * 0.003f * 100000.0f;
}

void setRedInput(scalar value)
{
    iExtNeurons[1] = 4.58f * value * 0.003f * 100000.0f;
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
        if(t > 10.0f && t < 60.0f) {
            setRedInput(leftValue);
            setBlueInput(0.0f);
        }
        else if(t > 70.0f && t < 120.0f) {
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
