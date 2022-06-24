#ifndef FFT_PERFORMER_H_
#define FFT_PERFORMER_H_

// OpenCV headers
#include <opencv2/opencv.hpp>

// standart headers
#include <fstream>
#include <iostream>

// other headers
#include "PeakFinder.h"

using namespace cv;

class FftPerformer
{
public:
	FftPerformer();
	~FftPerformer();

	std::vector<float> ExecuteFft(int sampling_frequency);
	
	// Collects new data of tracked points (frame at which point was tracked, coordinates of point, time at which point was tracked)
	void CollectTrackedPoints(int frame_pos_of_point, Point2f coordinates_of_points, double frame_time_of_point, int num);

	void WriteSpectrumToTxt();
	void WriteDataToTxt();

	int GetSizeOfVecs();
private:
	// performing FFT

private:
	int size_of_vecs_;
	float sampling_frequency_; // in fact fps of a video

	std::vector<int> frame_pos_of_point_;
	std::vector<cv::Point2f> coordinates_of_point_;
	Scalar meaned_coordinates_of_point_;
	std::vector<double> frame_time_of_point_;
};

#endif