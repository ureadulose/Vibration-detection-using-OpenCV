#ifndef VIBRATION_DISPLAYER_H_
#define VIBRATION_DISPLAYER_H_

// OpenCV headers
#include <opencv2/opencv.hpp>

// standart headers
#include <iostream>

// my headers
#include "contour_finder.h"

using namespace cv;

class VibrationDisplayer : public ContourFinder
{
public:
	VibrationDisplayer(std::string window_name, int width, int height);
	void Init();
	void ContourHandler(std::vector<std::vector<Point>> contour_shapes);
	void GetContourHeadPoints(std::vector<std::vector<Point>> contour_shapes/*, std::vector<Point>& contour_head_points*/);
private:
	
private:
	std::string window_name_;
	int frame_width_;
	int frame_height_;

	Mat frame_;
};

#endif