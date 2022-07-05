#ifndef VIDEO_UNDISTORTER_H
#define VIDEO_UNDISTORTER_H

// OpenCV headers
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/videoio/videoio.hpp>

#include <iostream>
#include <fstream>

// my headers

using namespace cv;

class VideoUndistorter
{
public:
	VideoUndistorter(std::string input_file_name_, std::string output_file_name, std::string txt_file_name);
	~VideoUndistorter();

	void ExecuteVideoUndistortion();

private:
	void SaveFrame(Mat frame);
	void LoadFoundParamsFromFile(std::string txt_file_name);
	std::vector<std::string> SplitString(std::string line, std::string delimiter);

private:
	VideoCapture* input_cap_;
	VideoWriter* output_cap_;

	double frame_width_;
	double frame_height_;
	double bitrate_;
	double frame_count_;
	double fps_;

	std::string input_file_name_;
	std::string output_file_name_;
	Mat camera_matrix_;
	Mat distortion_coefficients_;

	Mat current_frame_;
	Mat undistorted_frame_;

	std::ifstream input_file_;

	double fx_;
	double fy_;
	double px_;
	double py_;

	std::string txt_file_name_;
};

#endif