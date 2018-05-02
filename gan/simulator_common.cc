#include "simulator_common.h"

// Auto-generated model code
#include "chama_gan_CODE/definitions.h"

constexpr float gScale = 3.6f;

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

void initConnectivity()
{
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
}