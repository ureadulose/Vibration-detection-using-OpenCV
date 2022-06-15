#ifndef FFT_BUFFER_H_
#define FFT_BUFFER_H_

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

	void ClearTrash();

	std::vector<float> ExecuteFft(int sampling_frequency);
	
	// Collects new data of tracked points (frame at which point was tracked, coordinates of point, time at which point was tracked)
	void CollectTrackedPoints(int frame_pos_of_point, Point2f coordinates_of_points, double frame_time_of_point, int num);

	void WriteSpectrumToTxt();
	void WriteDataToTxt();

	int GetSizeOfVecs();
private:
	// performing FFT
	void SubtractMean();

	std::vector<Point2f> HammingWindow();
	std::vector<Point2f> HighPass();

private:
	int size_of_vecs_;
	float sampling_frequency_; // in fact fps of a video
	std::vector<int> indexes_of_target_frequencies_;

	std::vector<int> frame_pos_of_point_;
	std::vector<cv::Point2f> coordinates_of_point_;
	Scalar meaned_coordinates_of_point_;
	std::vector<double> frame_time_of_point_;

	std::vector<Point2f> fft_result_;
	std::vector<Point2f> p2_;
	std::vector<Point2f> p1_;
	std::vector<float> y_values_;
};

#endif