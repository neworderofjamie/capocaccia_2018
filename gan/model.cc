#include "modelSpec.h"

// GeNN robotics includes
#include "exp_curr.h"
#include "adexp.h"


void modelDefinition(NNmodel &model)
{
    initGeNN();
    model.setDT(0.1);
    model.setName("chama_gan");

    //---------------------------------------------------------------------------
    // Parameters
    //---------------------------------------------------------------------------
    // AdExp model parameters
    AdExp::ParamValues neuronParamVals(281.0,    // Membrane capacitance [pF]
                                       30.0,     // Leak conductance [nS]
                                       -70.6,    // Leak reversal potential [mV]
                                       2.0,      // Slope factor [mV]
                                       -50.4,    // Threshold voltage [mV]
                                       10.0,     // Artificial spike height [mV]
                                       -70.6,    // Reset voltage [mV]
                                       144.0,    // Adaption time constant
                                       4.0,      // Subthreshold adaption [nS]
                                       0.0805,   // Spike-triggered adaptation [nA]
                                       0.0);    // Offset current
    
    AdExp::VarValues neuronInitVals(-70.6,       // 0 - V
                                    0.0);        // 1 - W

    PostsynapticModels::ExpCond::ParamValues excitatorySynParamVals(5.0,    // Tau
                                                                    0.0);   // Reversal potential [mV]
    
    PostsynapticModels::ExpCond::ParamValues inhibitorySynParamVals(5.0,    // Tau
                                                                    -70.0); // Reversal potential [mV]
    
    WeightUpdateModels::StaticPulse::VarValues staticSynapseInitVals(0.0);  // Weight
    
    //---------------------------------------------------------------------------
    // Parameters
    //---------------------------------------------------------------------------
    model.addNeuronPopulation<AdExp>("Neurons", 4, neuronParamVals, neuronInitVals);
    
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>("ExcitatorySyn", SynapseMatrixType::DENSE_INDIVIDUALG, NO_DELAY, 
                                                                                             "Neurons", "Neurons",
                                                                                             {}, staticSynapseInitVals,
                                                                                             excitatorySynParamVals, {});
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>("InhibitorySyn", SynapseMatrixType::DENSE_INDIVIDUALG, NO_DELAY, 
                                                                                             "Neurons", "Neurons",
                                                                                             {}, staticSynapseInitVals,
                                                                                             inhibitorySynParamVals, {});
    model.finalize();
}