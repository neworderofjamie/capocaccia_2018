// GeNN includes
#include "modelSpec.h"

// GeNN robotics includes
#include "connectors.h"
#include "exp_curr.h"
#include "lif.h"

#include "parameters.h"

//----------------------------------------------------------------------------
// LateralGrid
//----------------------------------------------------------------------------
//! Initialises connectivity to connect each neuron to neighbours (in cardinal directions)
class LateralGrid : public InitSparseConnectivitySnippet::Base
{
public:
    DECLARE_SNIPPET(LateralGrid, 2);

    SET_ROW_BUILD_CODE(
        "const int width = (int)$(width);\n"
        "const int height = (int)$(height);\n"
        "// If presynaptic neuron isn't on first row\n"
        "if($(i) >= width) {\n"
        "   $(addSynapse, $(i) - width);\n"
        "}\n"
        "// If presynaptic neuron isn't on last row\n"
        "else if($(i) < ((width * height) - width)) {\n"
        "   $(addSynapse, $(i) + width);\n"
        "}\n"
        "// If presynaptic neuron isn't in first column\n"
        "else if(($(i) % width) > 0) {\n"
        "   $(addSynapse, $(i) - 1);\n"
        "}\n"
        "// If presynaptic neuron isn't in last column\n"
        "else if(($(i) % width) < (width - 1)) {\n"
        "   $(addSynapse, $(i) + 1);\n"
        "}\n"
        "$(endRow);\n");
    
    SET_PARAM_NAMES({"width", "height"});
};
IMPLEMENT_SNIPPET(LateralGrid);

//----------------------------------------------------------------------------
// DirectionGrid
//----------------------------------------------------------------------------
//! Initialises connectivity to connect each neuron to a neighbour with specific offset (if it's valid)
class DirectionGrid : public InitSparseConnectivitySnippet::Base
{
public:
    DECLARE_SNIPPET(DirectionGrid, 4);

    SET_ROW_BUILD_CODE(
        "const int width = (int)$(width);\n"
        "const int height = (int)$(height);\n"
        "const int xTarget = ($(i) % width) + (int)$(xDir);\n"
        "const int yTarget = ($(i) / width) + (int)$(yDir);\n"
        "if(xTarget >= 0 && xTarget < width && yTarget >= 0 && yTarget < height) {\n"
        "   $(addSynapse, (yTarget * width) + xTarget);\n"
        "}\n"
        "$(endRow);\n");
    
    SET_PARAM_NAMES({"width", "height", "xDir", "yDir"});
};
IMPLEMENT_SNIPPET(DirectionGrid);

void modelDefinition(NNmodel &model)
{
    initGeNN();
    model.setDT(1.0);
    model.setName("neuro_slam_1");
    
    // LIF model parameters
    LIF::ParamValues lifParams(1.0,      // 0 - C
                               20.0,     // 1 - TauM 
                               -70.0,    // 2 - Vrest
                               -70.0,    // 3 - Vreset
                               -51.0,    // 4 - Vthresh
                               0.0,      // 5 - Ioffset
                               2.0);     // 6 - TauRefrac
    
    LIF::VarValues lifInit(-70.0,       // 0 - V
                           0.0);        // 1 - RefracTime
    
     // Exponential current parameters
    ExpCurr::ParamValues excitatoryExpCurrParams(
        5.0);  // 0 - TauSyn (ms)

    ExpCurr::ParamValues inhibitoryExpCurrParams(
        10.0);  // 0 - TauSyn (ms)
        
    // Population where each neuron represents a position in the world
    model.addNeuronPopulation<LIF>("Position", Parameters::worldWidth * Parameters::worldHeight, lifParams, lifInit);
    
    // Create recurrent excitation
    WeightUpdateModels::StaticPulse::VarValues positionRecurrentSynapseInit(8.0);
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, ExpCurr>(
        "Position_Recurrent", SynapseMatrixType::RAGGED_GLOBALG, NO_DELAY,
        "Position", "Position",
        {}, positionRecurrentSynapseInit,
        excitatoryExpCurrParams, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>({}));
    
    // Create lateral inhibition
    LateralGrid::ParamValues lateralGridParams(Parameters::worldWidth, Parameters::worldHeight);
    WeightUpdateModels::StaticPulse::VarValues positionLateralSynapseInit(-8.0);
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, ExpCurr>(
        "Position_Lateral", SynapseMatrixType::RAGGED_GLOBALG, NO_DELAY,
        "Position", "Position",
        {}, positionLateralSynapseInit,
        inhibitoryExpCurrParams, {},
        initConnectivity<LateralGrid>(lateralGridParams));
    
    // Population where each (inhibitory) neuron represents a movement direction component
    auto *direction = model.addNeuronPopulation<LIF>("Direction", Parameters::DirectionMax, lifParams, lifInit);
    
    // Create wall population
    auto *wall = model.addNeuronPopulation<LIF>("Wall", 1, lifParams, lifInit);*/
    
    // Loop through directions
    WeightUpdateModels::StaticPulse::VarValues positionDirectionSynapseInit(8.0);
    WeightUpdateModels::StaticPulse::VarValues directionPositionSynapseInit(8.0);
    WeightUpdateModels::StaticPulse::VarValues directionPositionSynapseInit(8.0);
    for(unsigned int d = 0; d < Parameters::DirectionMax; d++) {
        // Population of interneurons to connect neighbours
        // **NOTE** these populations are one neuron in one dimension oversized to simplify indexing
        const std::string name = std::string("Interneuron") + std::string(Parameters::directionNames[d]);
        model.addNeuronPopulation<LIF>(name, Parameters::worldWidth * Parameters::worldHeight, lifParams, lifInit);
        
        // Connect each position neuron to corresponding direction interneuron
        model.addSynapsePopulation<WeightUpdateModels::StaticPulse, ExpCurr>(
            "Position_" + name, SynapseMatrixType::RAGGED_GLOBALG, NO_DELAY,
            "Position", name,
            {}, positionDirectionSynapseInit,
            excitatoryExpCurrParams, {},
            initConnectivity<InitSparseConnectivitySnippet::OneToOne>({}));
        
        // Connect each interneuron to neighbouring  position neuron with correct offset
        /*DirectionGrid::ParamValues directionGridParams(Parameters::worldWidth, 
                                                       Parameters::worldHeight, 
                                                       std::get<0>(Parameters::directionOffsets[d]),
                                                       std::get<1>(Parameters::directionOffsets[d]));
        model.addSynapsePopulation<WeightUpdateModels::StaticPulse, ExpCurr>(
            name + "_Position", SynapseMatrixType::RAGGED_GLOBALG, NO_DELAY,
            name, "Position", 
            {}, directionPositionSynapseInit,
            excitatoryExpCurrParams, {},
            initConnectivity<DirectionGrid>(directionGridParams));
        
        // Connect direction inhibitory neurons to all interneurons
        model.addSynapsePopulation<WeightUpdateModels::StaticPulse, ExpCurr>(
            name + "_Position", SynapseMatrixType::DENSE_GLOBALG, NO_DELAY,
            name, "Position", 
            {}, directionPositionSynapseInit,
            excitatoryExpCurrParams, {});*/
    }
    
    // Create synape populations
    model.finalize();
}