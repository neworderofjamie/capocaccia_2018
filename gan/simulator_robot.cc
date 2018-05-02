// Standard C++ includes
#include <numeric>

// OpenCV includes
#include <opencv2/opencv.hpp>

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

// Minimum frames of same 
constexpr unsigned int minPatternColourFrames = 10;
constexpr unsigned int minIntervalFrames = 4;
constexpr int minPatternColourLevel = 30000000;


void simulate(scalar red1, scalar blue1, scalar red2, scalar blue2)
{
    // Open CSV output files
    SpikeCSVRecorder spikes("spikes.csv", glbSpkCntNeurons, glbSpkNeurons);
    AnalogueCSVRecorder<scalar> voltages("voltages.csv", VNeurons, 5, "Membrane voltage [mV]");

    // Loop through timesteps
    unsigned int numOut1 = 0;
    unsigned int numOut2 = 0;
    const float startT = t;
    while(t < (startT + experimentDuration)) {
        const float relativeT = t - startT;
        
        // Present single stimuli
        if(relativeT >= startTime && relativeT < (startTime + stimDuration)) {
            setRedInput(red1);
            setBlueInput(blue1);
        }
        else if(relativeT >= (startTime + stimDuration + stimSpacing) && relativeT < (startTime + stimDuration + stimSpacing + stimDuration)) {
            setRedInput(red2);
            setBlueInput(blue2);
        }
        else {
            setRedInput(0.0f);
            setBlueInput(0.0f);
        }
        
        // Simulate
        stepTimeCPU();

        // Record spikes and voltage
        spikes.record(t);
        voltages.record(t);
        
        // Accumulate spike counts
        for(unsigned int i = 0; i < glbSpkCntNeurons[0]; i++) {
            if(glbSpkNeurons[i] == 3) {
                numOut1++;
            }
            else if(glbSpkNeurons[i] == 4) {
                numOut2++;
            }
        }
        
    }
    
    std::cout << numOut1 << ", " << numOut2 << std::endl;
}
}   // Anonymous namespace

int main(int argc, char *argv[])
{
    const scalar leftValue = 0.55f;
    const scalar rightValue = 0.45f;
    const unsigned int device = (argc > 1) ? std::atoi(argv[1]) : 0;
    
    // Open video capture device and check it matches desired camera resolution
    cv::VideoCapture capture(device);
   
    allocateMem();
    initialize();
  
    initConnectivity();
  
    const cv::Size camRes(640, 480);
    assert(capture.get(cv::CAP_PROP_FRAME_WIDTH) == camRes.width);
    assert(capture.get(cv::CAP_PROP_FRAME_HEIGHT) == camRes.height);
    
    cv::namedWindow("View", CV_WINDOW_NORMAL);
    cv::resizeWindow("View", camRes.width, camRes.height);
    
    // Allocate image to hold camera input
    cv::Mat rgbInput(camRes, CV_8UC3);

    // Allocate images for each colour channel
    cv::Mat rgbChannels[3]{
        cv::Mat(camRes, CV_8UC1), 
        cv::Mat(camRes, CV_8UC1), 
        cv::Mat(camRes, CV_8UC1)};

    // Allocate vectors to hold reductions
    cv::Mat rColumns(1, camRes.width, CV_32SC1);
    cv::Mat bColumns(1, camRes.width, CV_32SC1);
    
    enum class State
    {
        None,
        FirstStimuli,
        Interval,
        SecondStimuli,
    };
    
    State state = State::None;
    int currentStimuliIndex = -1;
    int currentStimuliSide = -1;
    unsigned int stateStartTime = 0;
    scalar red1;
    scalar blue1;
    scalar red2;
    scalar blue2;
    for(unsigned int i = 0;; i++) {
        // Capture frame
        if(!capture.read(rgbInput)) {
            return EXIT_FAILURE;
        }
        
        // Split image channels
        cv::split(rgbInput, rgbChannels);
        
        // Reduce red and blue into columns (widening datatype)
        // **NOTE** BGR ordering
        cv::reduce(rgbChannels[0], bColumns, 0, CV_REDUCE_SUM, CV_32SC1);
        cv::reduce(rgbChannels[2], rColumns, 0, CV_REDUCE_SUM, CV_32SC1);
        
        // Get raw pixels
        const int32_t *bColumnsRaw = reinterpret_cast<const int32_t*>(bColumns.data);
        const int32_t *rColumnsRaw = reinterpret_cast<const int32_t*>(rColumns.data);
        
        // Sum left and right half 
        const int32_t sums[4] = {
            std::accumulate(&bColumnsRaw[0], &bColumnsRaw[camRes.width / 2], 0),
            std::accumulate(&bColumnsRaw[camRes.width / 2], &bColumnsRaw[camRes.width], 0),
            std::accumulate(&rColumnsRaw[0], &rColumnsRaw[camRes.width / 2], 0),
            std::accumulate(&rColumnsRaw[camRes.width / 2], &rColumnsRaw[camRes.width], 0) };
        
        // Find the largest sum
        const int maxSumIndex = std::max_element(&sums[0], &sums[4]) - &sums[0];
        const auto maxSumIndexDiv = std::div(maxSumIndex, 2);
        
        // If a colour is presented
        if(sums[maxSumIndex] > minPatternColourLevel) {
            // If we're waiting for a stimuli
            if(state == State::None) {
                std::cout << "Start pattern:" <<  ((maxSumIndexDiv.quot == 0) ? "Blue" : "Red");
                std::cout << " " << ((maxSumIndexDiv.rem == 0) ? "Left" : "Right") << std::endl;
                
                // Blue
                if(maxSumIndexDiv.quot == 0) {
                    red1 = 0.0f;
                    blue1 = (maxSumIndexDiv.rem == 0) ? leftValue : rightValue;
                }
                // Red
                else {
                    red1 = (maxSumIndexDiv.rem == 0) ? leftValue : rightValue;
                    blue1 = 0.0f;
                }
                stateStartTime = i;
                currentStimuliIndex = maxSumIndexDiv.quot;
                currentStimuliSide = maxSumIndexDiv.rem;
                state = State::FirstStimuli;
            }
            // If we're receiving a stimuli
            else if(state == State::FirstStimuli || state == State::SecondStimuli) {
                // If stimuli colour has changed
                if(currentStimuliIndex != maxSumIndexDiv.quot) {
                    std::cout << "\tColour changed - invalid pattern" << std::endl;
                    state = State::None;
                }
                else if(currentStimuliSide != maxSumIndexDiv.rem) {
                    std::cout << "\tSide changed - invalid pattern" << std::endl;
                    state = State::None;
                }
            }
            else if(state == State::Interval) {
                // If more than minimum interval frames has elapsed, enter second stimuli
                if((i - stateStartTime) > minPatternColourFrames) {
                    if(currentStimuliSide != maxSumIndexDiv.rem) {
                        std::cout << "\tSide changed - invalid pattern" << std::endl;
                        state = State::None;
                    }
                    else {
                        std::cout << "\tSecond stimuli:" <<  ((maxSumIndexDiv.quot == 0) ? "Blue" : "Red") << std::endl;
                        
                        // Blue
                        if(maxSumIndexDiv.quot == 0) {
                            red2 = 0.0f;
                            blue2 = (maxSumIndexDiv.rem == 0) ? leftValue : rightValue;
                        }
                        // Red
                        else {
                            red2 = (maxSumIndexDiv.rem == 0) ? leftValue : rightValue;
                            blue2 = 0.0f;
                        }
                    
                        stateStartTime = i;
                        currentStimuliIndex = maxSumIndexDiv.quot;
                        state = State::SecondStimuli;
                    }
                }
                else {
                    std::cout << "\tInterval too short - invalid pattern" << std::endl;
                    state = State::None;
                }
            }
        }
        // Otherwise, if blank is presented
        else {
            // If we're receiving a stimuli
            if(state == State::FirstStimuli || state == State::SecondStimuli) {
                // If more than minimum pattern frames has elapsed, enter interval
                if((i - stateStartTime) > minPatternColourFrames) {                    
                    if(state == State::FirstStimuli) {
                        std::cout << "\tInterval!" << std::endl;
                        stateStartTime = i;
                        state = State::Interval;
                    }
                    else {
                        std::cout << "\tPattern complete!" << std::endl;
                        std::cout << red1 << ", " << blue1 << ", " << red2 << ", " << blue2 << std::endl;
                        simulate(red1, blue1, red2, blue2);
                        state = State::None;
                    }
                }
                else {
                    std::cout << "\tStimuli too short - invalid pattern" << std::endl;
                    state = State::None;
                }
            }
        }
        
        // Show original view
        cv::imshow("View", rgbInput);
        if(cv::waitKey(1) == 27) {
            break;
        }
    }
    return EXIT_SUCCESS;
    
    
    return 0;
}
