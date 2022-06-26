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

bool VibrationDisplayer::InitColors()
{
	if (points_.empty())
		return false;

	if (!colors_.empty())
		return false;

	for (int i = 0; i < points_.size(); i++)
	{
		colors_.push_back(Scalar(0, 0, 255));
	}
	return true;
}

void VibrationDisplayer::ShowFrame()
{
	ClearFrame();

	// drawing ROI and updating colors
	UpdateDisplayingRectangle();
	UpdateColors();

	for (int i = 0; i < points_.size(); i++)
	{
		// drawing points with their own colors - frequencies
		circle(frame_, points_[i], 2, colors_[i], FILLED);
		/*std::cout << "points_ size " << points_.size() << std::endl;
		std::cout << "colors_ size " << colors_.size() << std::endl;
		std::cout << "frame_ size " << frame_.size() << std::endl;*/
	}

	// finally showing frame
	imshow(window_name_, frame_);
}

Mat VibrationDisplayer::GetFrame()
{
	return frame_;
}

void VibrationDisplayer::SetRoi(Rect roi)
{
	roi_ = roi;
}

void VibrationDisplayer::UpdateFrequencies(std::vector<float> frequencies, double range)
{
	frequencies_ = frequencies;
	range_ = range;
}

void VibrationDisplayer::UpdateDisplayingPoints(std::vector<Point2f> points)
{
	points_ = points;
}

void VibrationDisplayer::ClearFrame()
{
	frame_ = Mat(frame_height_, frame_width_, CV_8UC3, Scalar(0, 0, 0));
}

void VibrationDisplayer::UpdateDisplayingRectangle()
{
	rectangle(frame_, roi_.tl(), roi_.br(), Scalar(0, 255, 0), 1);
}

void VibrationDisplayer::UpdateColors()
{
	// for now maximum possible value of frequency is 15
	// so lets translate frequency [0; 15] to [0; 255] int color value;
	if (!frequencies_.empty())
	{
		colors_.clear();
		for (int i = 0; i < points_.size(); i++)
		{
			// there is one feature (not a bug of course)
			// i take only one frequency from frequencies_
			// i'll fix this later, now im too tired
			// вообще на самом деле не знаю, зачем я пищу комменты на англ, потом переделаю)
			// по крайней мере уж этот коммент точно можно было и на русском написать 
			std::vector<int> color = Rgb(frequencies_[0] / range_);
			colors_.push_back(Scalar(color[0], color[1], color[2]));
		}
	}
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


