#include "amplitude_calibrator.h"

AmplitudeCalibrator::AmplitudeCalibrator(Mat frame) :
	frame_{ frame },
	ethalon_length_{ 10.0 }

{
	namedWindow(WINNAME);
	imshow(WINNAME, frame_);
}

AmplitudeCalibrator::~AmplitudeCalibrator()
{
	destroyWindow(WINNAME);
}

void AmplitudeCalibrator::Execute()
{
	std::cout << "left click - first point, right click - second point" << std::endl;
	setMouseCallback(WINNAME, SelectPoint, (void*)this);
	if (waitKey(0) == 27)
	{
		pixel_length_ = CalculateLength(first_point_, second_point_);
		std::cout << "The length in pixels is: " << pixel_length_ << std::endl;
	}
}

double AmplitudeCalibrator::GetCmPerPixelRatio()
{
	return ethalon_length_ / pixel_length_;
}

void AmplitudeCalibrator::SelectPoint(int event, int x, int y, int flags, void* userdata)
{
	AmplitudeCalibrator* d = static_cast<AmplitudeCalibrator*>(userdata);
	d->OnMouse(event, x, y, flags);
}

// "helper" function for implementing callback function as a method of C++ class
void AmplitudeCalibrator::OnMouse(int event, int x, int y, int flags)
{
	switch (event) {
	case EVENT_LBUTTONDOWN:
	{
		first_point_.x = x;
		first_point_.y = y;
		std::cout << "first point has been set" << std::endl;
		break;
	}
	case EVENT_RBUTTONDOWN:
	{
		second_point_.x = x;
		second_point_.y = y;
		std::cout << "second point has been set" << std::endl;
		break;
	}
	}
}

double AmplitudeCalibrator::CalculateLength(Point2f first_point, Point2f second_point)
{
	return sqrt((second_point.x - first_point.x) * (second_point.x - first_point.x) + (second_point.y - first_point.y) * (second_point.y - first_point.y));
}
