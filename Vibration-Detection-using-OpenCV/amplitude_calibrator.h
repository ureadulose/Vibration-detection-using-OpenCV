#ifndef AMPLITUDE_CALIBRATOR_H
#define AMPLITUDE_CALIBRATOR_H

#define WINNAME "Amplitude calibration window"

// standart headers
#include <iostream>

// OpenCV headers
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/opencv.hpp>

// my headers
#include "video_processing.h"

class AmplitudeCalibrator// : public VideoProcessor
{
public:
	AmplitudeCalibrator(Mat frame);
	~AmplitudeCalibrator();
	void Execute();
	double GetCmPerPixelRatio();

private:
	static void SelectPoint(int event, int x, int y, int flags, void* userdata);
	void OnMouse(int event, int x, int y, int flags);
	double CalculateLength(Point2f first_point, Point2f second_point);

private:
	Mat frame_;
	Point2f first_point_;
	Point2f second_point_;

	double ethalon_length_;
	double pixel_length_;
};

#endif