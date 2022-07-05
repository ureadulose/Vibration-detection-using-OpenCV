#ifndef CAMERA_CALIBRATOR_H
#define CAMERA_CALIBRATOR_H

// OpenCV headers
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

// my headers
#include "video_undistorter.h"

using namespace cv;

class CameraCalibrator
{
public:
	CameraCalibrator(std::string input_file_name_);
	~CameraCalibrator();

	void ExecuteCameraCalibration();

private:
	bool FindCorners(Mat& input_frame, Size pattern_size);
	void SaveFoundParamsToFile(Mat camera_matrix, Mat dist_coeffs_2be_written, std::string file);

	template <typename T>
	std::string ToStringWithPrecision(const T a_value, const int n = 9);
	
private:
	VideoCapture* input_cap_;

	double frame_width_;
	double frame_height_;
	double bitrate_;
	double frame_count_;
	double fps_;

	std::string input_file_name_;
	std::string txt_file_name_;
	Mat camera_matrix_;
	Mat distortion_coefficients_;
	std::vector<Mat> rotation_vector_;
	std::vector<Mat> translation_vector_;

	Mat current_frame_;

	std::vector<Point2f> corners_;
	std::vector<std::vector<Point2f>> vec_of_corners_;
	Size pattern_size_;
	bool pattern_found_;

	double fx_;
	double fy_;
	double px_;
	double py_;

	std::ofstream outfile_;

};

#endif