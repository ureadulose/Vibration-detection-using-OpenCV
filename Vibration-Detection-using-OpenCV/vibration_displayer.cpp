#include "vibration_displayer.h"

VibrationDisplayer::VibrationDisplayer(std::string window_name, int frame_width, int frame_height)
	:window_name_{ window_name },
	frame_width_{ frame_width },
	frame_height_{ frame_height }
{
	
}

void VibrationDisplayer::Init()
{
	// making a window
	
	// UNCOMMENT THIS LATER
	// 
	// 
	//namedWindow(window_name_, WINDOW_AUTOSIZE);
	//frame_ = Mat(frame_height_, frame_width_, CV_8UC3, Scalar(0, 0, 0));
	//
	//imshow(window_name_, frame_);
}

void VibrationDisplayer::ContourHandler(std::vector<std::vector<Point>> contour_shapes)
{

}
