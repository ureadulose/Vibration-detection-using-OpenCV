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
	bool InitColors();
	//void ContourHandler(std::vector<std::vector<Point>> contour_shapes);
	//void GetContourHeadPoints(std::vector<std::vector<Point>> contour_shapes, std::vector<Point>& contour_head_points);
	void ProcessFrame(Mat tmp_frame);
	Mat GetFrame();
	void SetRoi(Rect roi);
	void UpdateFrequencies(std::vector<float> frequencies, double range);
	void UpdateDisplayingPoints(std::vector<Point2f> points);
private:
	// making frame black
	void ClearFrame();
	void UpdateDisplayingRectangle();
	void UpdateColors();
	void AddGradient();

public:
	// Getting color value from gradient
	std::vector<int> Rgb(double ratio);
private:
	Mat tmp_frame_;

	std::string window_name_;
	int frame_width_;
	int frame_height_;

	std::vector<Point2f> points_;
	std::vector<float> frequencies_;
	std::vector<Scalar> colors_;
	double range_;

	Mat frame_;
	Rect roi_;
};

#endif