#ifndef VIDEO_PROCESSING_H_
#define VIDEO_PROCESSING_H_

// standart headers
#include <iostream>

// OpenCV headers
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

class VideoProcessor
{
public:
	VideoProcessor(const std::string input_file_name, const std::string output_file_name, const std::string window_name);
	~VideoProcessor();
	bool Init();
	void ReadNextFrame();
	void ShowFrame(Mat frame);
	void WriteFrame(Mat frame);

public:
	bool GetInputCapStatus();
	Mat GetCurrentFrame();
	String GetWindowName();
	double GetCurrentTimeOfFrame();
	int GetCurrentPosOfFrame();
	int GetAmountOfFrames();
	int GetInputFps();
	int GetFrameWidth();
	int GetFrameHeight();

protected:
	Mat input_frame_;
	Mat current_frame_;

	VideoCapture* input_cap_;
	bool input_cap_status_;
	VideoWriter* output_cap_;
	std::string input_path_;
	std::string output_path_;

	// characteristics of a video
	int input_frame_width_;
	int input_frame_height_;
	double input_fps_;

	std::string window_name_;

	// for fast fourier transform and etc.
	double current_time_of_frame_;
	int current_pos_of_frame_;

};

#endif