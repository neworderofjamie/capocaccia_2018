#include "modelSpec.h"

// GeNN robotics includes
#include "adexp.h"

//----------------------------------------------------------------------------
// ExpCondExtCond
//----------------------------------------------------------------------------
class ExpCondExtCond : public PostsynapticModels::Base
{
public:
    DECLARE_MODEL(ExpCondExtCond, 2, 1);

    SET_DECAY_CODE("$(inSyn)*=$(expDecay);");

    SET_CURRENT_CONVERTER_CODE("($(inSyn) + $(gExt)) * ($(E) - $(V))");

    SET_PARAM_NAMES({"tau", "E"});
    SET_VARS({{"gExt", "scalar"}});

    SET_DERIVED_PARAMS({{"expDecay", [](const vector<double> &pars, double dt){ return std::exp(-dt / pars[0]); }}});
};
IMPLEMENT_MODEL(ExpCondExtCond);

void modelDefinition(NNmodel &model)
{
    initGeNN();
    model.setDT(0.1);
    model.setName("chama_gan");

    //---------------------------------------------------------------------------
    // Parameters
    //---------------------------------------------------------------------------
    // AdExp model parameters
    AdExp::ParamValues neuronParamVals(200.0,     // Membrane capacitance [pF]
                                       30.0,      // Leak conductance [nS]
                                       -70.0,     // Leak reversal potential [mV]
                                       2.0,       // Slope factor [mV]
                                       -50.0,     // Threshold voltage [mV]
                                       0.0,       // Artificial spike height [mV]
                                       -58.0,     // Reset voltage [mV]
                                       30.0,      // Adaption time constant
                                       2.0,       // Subthreshold adaption [nS]
                                       0.0,       // Spike-triggered adaptation [nA]
                                       0.0);      // Offset current
    
    AdExp::VarValues neuronInitVals(-70.0,    // 0 - V
                                    0.0);     // 1 - W

    ExpCondExtCond::ParamValues excitatorySynParamVals(5.0,    // Tau
                                                       0.0);   // Reversal potential [mV]
    
    ExpCondExtCond::ParamValues inhibitorySynParamVals(5.0,    // Tau
                                                       -70.0); // Reversal potential [mV]
    
    ExpCondExtCond::VarValues synInitVals(0.0); // gExt
    
    WeightUpdateModels::StaticPulse::VarValues staticSynapseInitVals(0.0);  // Weight
    
    //---------------------------------------------------------------------------
    // Parameters
    //---------------------------------------------------------------------------
    model.addNeuronPopulation<AdExp>("Neurons", 5, neuronParamVals, neuronInitVals); //number of neurons in the network
    
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, ExpCondExtCond>("ExcitatorySyn", SynapseMatrixType::DENSE_INDIVIDUALG, NO_DELAY, 
                                                                                "Neurons", "Neurons",
                                                                                {}, staticSynapseInitVals,
                                                                                excitatorySynParamVals, synInitVals);
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, ExpCondExtCond>("InhibitorySyn", SynapseMatrixType::DENSE_INDIVIDUALG, NO_DELAY, 
                                                                                "Neurons", "Neurons",
                                                                                {}, staticSynapseInitVals,
                                                                                inhibitorySynParamVals, synInitVals);
    model.finalize();
}
