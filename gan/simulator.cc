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
constexpr float gScale = 3.6f;
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

void setExcitatoryWeight(unsigned int preIdx, unsigned int postIdx, scalar weight)
{
    gExcitatorySyn[(preIdx * 5) + postIdx] = weight * gScale;
}

void setInhibitoryWeight(unsigned int preIdx, unsigned int postIdx, scalar weight)
{
    gInhibitorySyn[(preIdx * 5) + postIdx] = weight * gScale;
}

void setBlueInput(scalar value) //B
{
	gExtExcitatorySyn[0] = 5.39f * value * 3.0f * gScale;
	gExtExcitatorySyn[1] = 0.37f * value * 3.0f * gScale;
	gExtInhibitorySyn[2] = -1.23f * value * 3.0f * gScale;
}

void setRedInput(scalar value) //A
{
	gExtExcitatorySyn[0] = 0.29f * value * 3.0f * gScale;
	gExtExcitatorySyn[2] = 0.78f * value * 3.0f * gScale;
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
// AL,AR order of last two neurons
    //====neuron 0======
    setExcitatoryWeight(0, 1, 4.74f);
    setExcitatoryWeight(0, 2, 6.65f);
    setExcitatoryWeight(0, 3, 8.45f); // turn right::3
    setExcitatoryWeight(0, 4, 4.34f); //turn left:: 4
    setInhibitoryWeight(0, 0, -4.32f);
    //====neuron 1======
	
    setExcitatoryWeight(1, 4, 3.64f);
    setExcitatoryWeight(1, 2, 1.1f);
    setInhibitoryWeight(1, 0, -2.67f);
    //====neuron 2======
    setExcitatoryWeight(2, 4, 3.64f);
    setExcitatoryWeight(2, 2, 1.1f);
    setInhibitoryWeight(2, 0, -2.67f);
    
    initchama_gan();
  
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
