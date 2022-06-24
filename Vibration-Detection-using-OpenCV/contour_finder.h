#ifndef CONTOUR_FINDER_H_
#define CONTOUR_FINDER_H_

// OpenCV headers
#include <opencv2/opencv.hpp>

using namespace cv;

class ContourFinder
{
public:
	std::vector<std::vector<Point>> GetContours(Mat input_frame);
	Mat GetPreparedFrame();
private:
	// prepare a frame for finding contours
	Mat PrepareFrame(Mat input_frame);
private:
	Mat prep_frame_;
};

#endif