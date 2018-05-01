#pragma once

//------------------------------------------------------------------------
// Parameters
//------------------------------------------------------------------------
namespace Parameters
{
    // Population sizes
    const unsigned int numTN2 = 2;
    const unsigned int numTL = 8;
    const unsigned int numCL1 = 8;
    const unsigned int numTB1 = 4;
    const unsigned int numCPU4 = 8;
    const unsigned int numPontine = 8;
    const unsigned int numCPU1 = 8;

    const double c = 0.33;

    const double pi = 3.141592653589793238462643383279502884;
    
    enum Hemisphere
    {
        HemisphereLeft,
        HemisphereRight,
        HemisphereMax,
    };  
}