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
constexpr float gScale = 1.7f;
constexpr float startTime = 10.0f;
constexpr float stimDuration = 30.0f;
constexpr float stimSpacing = 5.0f;
constexpr float interStimTime = 100.0f;
constexpr float experimentDuration = (stimDuration * 2.0f) + stimSpacing + interStimTime;

constexpr float experiment1Start = startTime;
constexpr float experiment2Start = experiment1Start + experimentDuration;
constexpr float experiment3Start = experiment2Start + experimentDuration;
constexpr float experiment4Start = experiment3Start + experimentDuration;

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

void applyExperimentStimuli(scalar t, scalar red1, scalar blue1, scalar red2, scalar blue2, scalar startTime)
{
    // If we're in experiment at all
    if(t >= startTime && t < (startTime + experimentDuration)) {
        // If we're in first stimuli of experiment
        const scalar expT = t - startTime;
        if(expT < stimDuration) {
            setRedInput(red1);
            setBlueInput(blue1);
        }
        else if(expT >= (stimDuration + stimSpacing) && expT < (stimDuration + stimDuration + stimSpacing)) {
            setRedInput(red2);
            setBlueInput(blue2);
        }
    }
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

    std::ofstream stimuli("stim.csv");
    
    // Loop through timesteps
    while(t < 800.0f) {
        setRedInput(0.0f);
        setBlueInput(0.0f);
            
        // Red, Blue (left)
        applyExperimentStimuli(t, 
            leftValue, 0.0f, 
            0.0f, leftValue,
            experiment1Start);
        
        // Blue, Red (left)
        applyExperimentStimuli(t, 
            0.0f, leftValue,
            leftValue, 0.0f, 
            experiment2Start);
        
        // Red, Blue (left)
        applyExperimentStimuli(t, 
            rightValue, 0.0f, 
            0.0f, rightValue,
            experiment3Start);
        
        // Blue, Red (left)
        applyExperimentStimuli(t, 
            0.0f, rightValue,
            rightValue, 0.0f, 
            experiment4Start);
        
        stimuli << gExtInhibitorySyn[0] << ", " << gExtExcitatorySyn[1] << std::endl;
        
        // Simulate
        stepTimeCPU();

        // Record spikes and voltage
        spikes.record(t);
        voltages.record(t);

    }
    
    return 0;
}
