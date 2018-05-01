#include "modelSpec.h"

//----------------------------------------------------------------------------
// AdExp
//----------------------------------------------------------------------------
//! Adaptive exponential - solved using RK4
class AdExpExtCurrent : public NeuronModels::Base
{
public:
    DECLARE_MODEL(AdExpExtCurrent, 10, 3);

    SET_SIM_CODE(
        "#define DV(V, W) (1.0 / $(c)) * ((-$(gL) * ((V) - $(eL))) + ($(gL) * $(deltaT) * exp(((V) - $(vThresh)) / $(deltaT))) + i - (W))\n"
        "#define DW(V, W) (1.0 / $(tauW)) * (($(a) * (V - $(eL))) - W)\n"
        "const scalar i = $(Isyn) + $(iExt);\n"
        "// If voltage is above artificial spike height\n"
        "if($(V) >= $(vSpike)) {\n"
        "   $(V) = $(vReset);\n"
        "}\n"
        "// Calculate RK4 terms\n"
        "const scalar v1 = DV($(V), $(W));\n"
        "const scalar w1 = DW($(V), $(W));\n"
        "const scalar v2 = DV($(V) + (DT * 0.5 * v1), $(W) + (DT * 0.5 * w1));\n"
        "const scalar w2 = DW($(V) + (DT * 0.5 * v1), $(W) + (DT * 0.5 * w1));\n"
        "const scalar v3 = DV($(V) + (DT * 0.5 * v2), $(W) + (DT * 0.5 * w2));\n"
        "const scalar w3 = DW($(V) + (DT * 0.5 * v2), $(W) + (DT * 0.5 * w2));\n"
        "const scalar v4 = DV($(V) + (DT * v3), $(W) + (DT * w3));\n"
        "const scalar w4 = DW($(V) + (DT * v3), $(W) + (DT * w3));\n"
        "// Update V\n"
        "$(V) += (DT / 6.0) * (v1 + (2.0f * (v2 + v3)) + v4);\n"
        "// If we're not above peak, update w\n"
        "// **NOTE** it's not safe to do this at peak as wn may well be huge\n"
        "if($(V) <= -40.0) {\n"
        "   $(W) += (DT / 6.0) * (w1 + (2.0 * (w2 + w3)) + w4);\n"
        "}\n"
    );

    SET_THRESHOLD_CONDITION_CODE("$(V) > -40");

    SET_RESET_CODE(
        "// **NOTE** we reset v to arbitrary plotting peak rather than to actual reset voltage\n"
        "$(V) = $(vSpike);\n"
        "$(W) += ($(b) * 1000.0);");

    SET_PARAM_NAMES({
        "c",        // Membrane capacitance [pF]
        "gL",       // Leak conductance [nS]
        "eL",       // Leak reversal potential [mV]
        "deltaT",   // Slope factor [mV]
        "vThresh",  // Threshold voltage [mV]
        "vSpike",   // Artificial spike height [mV]
        "vReset",   // Reset voltage [mV]
        "tauW",     // Adaption time constant
        "a",        // Subthreshold adaption [nS]
        "b"});      // Spike-triggered adaptation [nA]

    SET_VARS({{"V", "scalar"}, {"W", "scalar"}, {"iExt", "scalar"}});
};
IMPLEMENT_MODEL(AdExpExtCurrent);

void modelDefinition(NNmodel &model)
{
    initGeNN();
    model.setDT(0.1);
    model.setName("chama_gan");

    //---------------------------------------------------------------------------
    // Parameters
    //---------------------------------------------------------------------------
    // AdExp model parameters
    AdExpExtCurrent::ParamValues neuronParamVals(200.0,     // Membrane capacitance [pF]
                                                 30.0,      // Leak conductance [nS]
                                                 -70.0,     // Leak reversal potential [mV]
                                                 2.0,       // Slope factor [mV]
                                                 -50.0,     // Threshold voltage [mV]
                                                 0.0,       // Artificial spike height [mV]
                                                 -58.0,     // Reset voltage [mV]
                                                 30.0,      // Adaption time constant
                                                 2.0,       // Subthreshold adaption [nS]
                                                 0.0);      // Spike-triggered adaptation [nA]
    
    AdExpExtCurrent::VarValues neuronInitVals(-58.0,    // 0 - V
                                              0.0,      // 1 - W
                                              0.0);     // 2 - iExt

    PostsynapticModels::ExpCond::ParamValues excitatorySynParamVals(5.0,    // Tau
                                                                    0.0);   // Reversal potential [mV]
    
    PostsynapticModels::ExpCond::ParamValues inhibitorySynParamVals(5.0,    // Tau
                                                                    -70.0); // Reversal potential [mV]
    
    WeightUpdateModels::StaticPulse::VarValues staticSynapseInitVals(0.0);  // Weight
    
    //---------------------------------------------------------------------------
    // Parameters
    //---------------------------------------------------------------------------
    model.addNeuronPopulation<AdExpExtCurrent>("Neurons", 4, neuronParamVals, neuronInitVals);
    
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