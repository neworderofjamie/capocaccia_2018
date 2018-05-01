#pragma once

// Standard C++ includes
#include <tuple>

// Parameters
namespace Parameters
{
    enum Direction
    {
        DirectionUp,
        DirectionDown,
        DirectionLeft,
        DirectionRight,
        DirectionMax
    };
    
    const char *directionNames[DirectionMax] = 
    {
        "Up",
        "Down",
        "Left",
        "Right"
    };
    
    const std::tuple<int, int> directionOffsets[DirectionMax] = 
    {
        std::make_tuple(0, -1),
        std::make_tuple(0, 1),
        std::make_tuple(0, -1),
        std::make_tuple(0, 1),
    };
    
    // How big is the world?
    const unsigned int worldWidth = 10;
    const unsigned int worldHeight = 10;
    
    inline unsigned int getNeuronIndex(unsigned int x, unsigned int y)
    {
        return (y * worldWidth) + x;
    }
}