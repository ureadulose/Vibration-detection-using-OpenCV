#ifndef DATA_DISPLAYER_H_
#define DATA_DISPLAYER_H_

// OpenCV headers
#include <opencv2/opencv.hpp>

using namespace cv;

class DataDisplayer
{
public:
	void OutputVibrationParameters(Mat& frame, Point2f point, std::vector<float> vec_of_frequencies);
private:

};

#endif