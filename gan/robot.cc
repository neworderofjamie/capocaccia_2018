// Standard C++ includes
#include <numeric>

// OpenCV includes
#include <opencv2/opencv.hpp>

namespace
{
// Minimum frames of same 
constexpr unsigned int minPatternColourFrames = 10;
constexpr unsigned int minIntervalFrames = 4;
constexpr int minPatternColourLevel = 30000000;
}

int main(int argc, char *argv[])
{
    const unsigned int device = (argc > 1) ? std::atoi(argv[1]) : 0;
    
    // Open video capture device and check it matches desired camera resolution
    cv::VideoCapture capture(device);

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
    for(unsigned int t = 0;; t++) {
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
                
                stateStartTime = t;
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
                if((t - stateStartTime) > minPatternColourFrames) {
                    if(currentStimuliSide != maxSumIndexDiv.rem) {
                        std::cout << "\tSide changed - invalid pattern" << std::endl;
                        state = State::None;
                    }
                    else {
                        std::cout << "\tSecond stimuli:" <<  ((maxSumIndexDiv.quot == 0) ? "Blue" : "Red") << std::endl;
                        stateStartTime = t;
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
                if((t - stateStartTime) > minPatternColourFrames) {                    
                    if(state == State::FirstStimuli) {
                        std::cout << "\tInterval!" << std::endl;
                        stateStartTime = t;
                        state = State::Interval;
                    }
                    else {
                        std::cout << "\tPattern complete!" << std::endl;
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
}