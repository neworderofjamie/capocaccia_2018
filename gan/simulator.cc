// GeNN robotics includes
#include "analogue_csv_recorder.h"
#include "spike_csv_recorder.h"

// Auto-generated model code
#include "chama_gan_CODE/definitions.h"

#include "simulator_common.h"

//----------------------------------------------------------------------------
// Anonymous namespace
//----------------------------------------------------------------------------
namespace
{
constexpr float startTime = 10.0f;
constexpr float stimDuration = 8.0f;
constexpr float stimSpacing = 3.0f;
constexpr float interStimTime = 100.0f;
constexpr float experimentDuration = (stimDuration * 2.0f) + stimSpacing + interStimTime;

constexpr float experiment1Start = startTime;
constexpr float experiment2Start = experiment1Start + experimentDuration;
constexpr float experiment3Start = experiment2Start + experimentDuration;
constexpr float experiment4Start = experiment3Start + experimentDuration;
constexpr float experiment5Start = experiment4Start + experimentDuration;
constexpr float experiment6Start = experiment5Start + experimentDuration;
constexpr float experiment7Start = experiment6Start + experimentDuration;
constexpr float experiment8Start = experiment7Start + experimentDuration;


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
  
    initConnectivity();
  
    // Open CSV output files
    SpikeCSVRecorder spikes("spikes.csv", glbSpkCntNeurons, glbSpkNeurons);
    AnalogueCSVRecorder<scalar> voltages("voltages.csv", VNeurons, 5, "Membrane voltage [mV]");

    std::ofstream stimuli("stim.csv");
    
    // Loop through timesteps
    while(t < 800.0f) {
        setRedInput(0.0f);
        setBlueInput(0.0f);
            //AB= red-blue

        // Red, Blue (left)
        applyExperimentStimuli(t, 
            leftValue, 0.0f, //red
            0.0f, leftValue, //blue
            experiment1Start);
        applyExperimentStimuli(t, 
            rightValue, 0.0f, //red
            0.0f, rightValue, //blue
            experiment2Start);
        // Blue, Red (left) // 
        applyExperimentStimuli(t, 
	    rightValue, 0.0f,
            rightValue, 0.0f, 
            experiment3Start);
	applyExperimentStimuli(t, 
	    leftValue, 0.0f,
            leftValue, 0.0f, 
            experiment4Start);
        
        // 
        applyExperimentStimuli(t, 
            0.0f, rightValue, 
            0.0f, rightValue,
            experiment5Start);
        
        // 
         applyExperimentStimuli(t, 
            0.0f, leftValue, 
            0.0f, leftValue,
            experiment6Start);
        
        stimuli << gExtExcitatorySyn[2] << ", " << gExtExcitatorySyn[1] << std::endl;
        
        // Simulate
        stepTimeCPU();

        // Record spikes and voltage
        spikes.record(t);
        voltages.record(t);

    }
    
    return 0;
}
