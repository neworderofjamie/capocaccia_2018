// Standard C++ includes
#include <numeric>

// OpenCV includes
#include <opencv2/opencv.hpp>

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
    
    while(true) {
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
        const int maxSum = std::max_element(&sums[0], &sums[4]) - &sums[0];
        const auto maxSumDiv = std::div(maxSum, 2);
        
        std::cout << ((maxSumDiv.quot == 0) ? "Blue" : "Red");
        std::cout << " " << ((maxSumDiv.rem == 0) ? "Left" : "Right") << std::endl;
        
        // Show original view
        cv::imshow("View", rgbInput);
        if(cv::waitKey(1) == 27) {
            break;
        }
    }
    return EXIT_SUCCESS;
}