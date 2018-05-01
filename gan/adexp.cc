#include <fstream>
#include <cmath>

namespace
{
    typedef float scalar;
    
    constexpr scalar dt = 0.1f;
    
    constexpr scalar c = 281.0f; // pF
    constexpr scalar gl = 30.0f; // nS 
    constexpr scalar el = -70.6f; // mV
    constexpr scalar vThresh = -50.4f;   // mV
    constexpr scalar vPeak = -40.0f;     // mV
    constexpr scalar vMax = 10.0f;       // mV
    constexpr scalar vReset = -70.6f;    // mV
    constexpr scalar deltaT = 2.0f;
    constexpr scalar tauW = 144.0f;
    constexpr scalar a = 4.0f;
    constexpr scalar b = 0.0805 * 1000.0f;
    
    /*inline scalar calcV(scalar t, scalar v, scalar w, scalar i) {
        return (1.0f / c) * ((-gl * (v - el)) + (gl * deltaT * exp((v - vThresh) / deltaT)) + i - w);
    }
    
    inline scalar calcW(scalar t, scalar v, scalar w) {
        return (1.0f / tauW) * ((a * (v - el)) - w);
    }*/
}

int main()
{
    scalar i = 0.0f;
    scalar v = el;
    scalar w = 0.0f;
    
    std::ofstream output("data.csv");
    
    #define DV(T, V, W) (1.0f / c) * ((-gl * (V - el)) + (gl * deltaT * exp((V - vThresh) / deltaT)) + i - W)
    #define DW(T, V, W) (1.0f / tauW) * ((a * (V - el)) - W)
    
    for(scalar t = 0.0f; t < 300.0f; t += dt) {
        if(t > 20.0f) {
            i = 700.0f;
        }
        
        // If voltage is 
        if(v >= vMax) {
            v = vReset;
        }
        
        // Calculate RK4 terms
        const scalar v1 = DV(t, v, w);
        const scalar w1 = DW(t, v, w);
        
        const scalar v2 = DV(t + (dt * 0.5f), v + (dt * 0.5f * v1), w + (dt * 0.5f * w1));
        const scalar w2 = DW(t + (dt * 0.5f), v + (dt * 0.5f * v1), w + (dt * 0.5f * w1));
        
        const scalar v3 = DV(t + (dt * 0.5f), v + (dt * 0.5f * v2), w + (dt * 0.5f * w2));
        const scalar w3 = DW(t + (dt * 0.5f), v + (dt * 0.5f * v2), w + (dt * 0.5f * w2));
        
        const scalar v4 = DV(t + dt, v + (dt * v3), w + (dt * w3));
        const scalar w4 = DW(t + dt, v + (dt * v3), w + (dt * w3));
        
        // Update V
        v += (dt / 6.0f) * (v1 + (2.0f * (v2 + v3)) + v4);
        
        // If we're above peak, reset v and add to w
        // **NOTE** we reset v to arbitrary plotting peak rather than to actual reset voltage
        if(v > vPeak) {
            v = vMax;
            w += b;
        }
        // Otherwise update w 
        // **NOTE** it's not safe to do this at peak as wn may well be huge
        else {
            w += (dt / 6.0f) * (w1 + (2.0f * (w2 + w3)) + w4);
        }
        output << t << ", " << v << ", " << w << ", " << v1 << ", " << v2 << ", " << v3 << ", " << v4 << ", " << w1 << ", " << w2 << ", " << w3 << ", " << w4<< std::endl;
        
    }
    return 0;
}