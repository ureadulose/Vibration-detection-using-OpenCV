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
	//ClearFrame();
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
	rectangle(frame_, roi_.tl(), roi_.br(), Scalar(0, 255, 0), 1);
	imshow(window_name_, frame_);
}

void VibrationDisplayer::SetRoi(Rect roi)
{
	roi_ = roi;
}

void VibrationDisplayer::ColorPoints(std::vector<float> frequencies, std::vector<Point2f> points, double range)
{
	ClearFrame();
	// for now maximum possible value of frequency is 15
	// so lets translate frequency [0; 15] to [0; 255] int color value;
	std::cout << "points size is " << points.size() << std::endl;
	for (int i = 0; i < points.size(); i++)
	{
		std::vector<int> color = Rgb(frequencies[i] / range);
		for (int i = 0; i < color.size(); i++)
		{
			std::cout << "color[" << i << "] is " << color[i] << std::endl;
		}
		circle(frame_, points[i], 2, Scalar(color[0], color[1], color[3]), FILLED);
	}
}

void VibrationDisplayer::ClearFrame()
{
	frame_ = Mat(frame_height_, frame_width_, CV_8UC3, Scalar(0, 0, 0));
}

// not mine, taken from stackoverflow
std::vector<int> VibrationDisplayer::Rgb(double ratio)
{
	// we want to normalize ratio so that it fits in to 6 regions
	// where each region is 256 units long
	int normalized = int(ratio * 256 * 6);

	// find the region for this position
	int region = normalized / 256;

	// find the distance to the start of the closest region
	int x = normalized % 256;

	int r = 0, g = 0, b = 0;
	switch (region)
	{
	case 0: r = 255; g = 0;   b = 0;   g += x; break;
	case 1: r = 255; g = 255; b = 0;   r -= x; break;
	case 2: r = 0;   g = 255; b = 0;   b += x; break;
	case 3: r = 0;   g = 255; b = 255; g -= x; break;
	case 4: r = 0;   g = 0;   b = 255; r += x; break;
	case 5: r = 255; g = 0;   b = 255; b -= x; break;
	}
	return { r, g, b };
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


