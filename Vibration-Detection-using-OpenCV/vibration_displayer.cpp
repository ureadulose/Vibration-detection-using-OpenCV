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
	namedWindow(window_name_, WINDOW_AUTOSIZE);
	frame_ = Mat(frame_height_, frame_width_, CV_8UC3, Scalar(0, 0, 0));
	
	
}

void VibrationDisplayer::ShowFrame(std::vector<Point2f> vibrating_points)
{
	ClearFrame();
	rectangle(frame_, roi_.tl(), roi_.br(), Scalar(0, 255, 0), 1);

	for (int i = 0; i < vibrating_points.size(); i++)
	{

		// circle
		circle(frame_, vibrating_points[i], 1, (0, 0, 255), FILLED);
	}

	imshow(window_name_, frame_);
}

void VibrationDisplayer::ShowFrame()
{
	ClearFrame();
	rectangle(frame_, roi_.tl(), roi_.br(), Scalar(0, 255, 0), 1);
	imshow(window_name_, frame_);
}

void VibrationDisplayer::SetRoi(Rect roi)
{
	roi_ = roi;
}

void VibrationDisplayer::ClearFrame()
{
	frame_ = Mat(frame_height_, frame_width_, CV_8UC3, Scalar(0, 0, 0));
}

//void VibrationDisplayer::ContourHandler(std::vector<std::vector<Point>> contour_shapes)
//{
//	std::vector<Point> contour_points;
//	// рассматриваем каждый найденный контур
//	for (int i = 0; i < contour_shapes.size(); i++)
//	{
//		
//	}
//	// imshow
//}
//
//void VibrationDisplayer::GetContourHeadPoints(std::vector<std::vector<Point>> contour_shapes, std::vector<Point>& contour_head_points)
//{
//	// рассматриваем каждый найденный контур
//	for (int i = 0; i < contour_shapes.size(); i++)
//	{
//		std::cout << "cum" << std::endl;
//		if (!contour_shapes[i].empty())
//		{
//			std::cout << "cum" << std::endl;
//			contour_head_points.push_back(contour_shapes[i][0]);
//		}
//	}
//}


