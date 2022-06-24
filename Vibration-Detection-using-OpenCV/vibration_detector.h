#ifndef VIBRATION_DETECTOR_H
#define VIBRATION_DETECTOR_H

// VREMENNO
#define INPUT_FILE_NAME "Resources/engine.mp4" // leaf.mp4 or engine.mp4 or vibration1.mp4
#define MAIN_WINDOW_NAME "Video"
#define V_MONITOR_WINDOW_NAME "Vibration Monitor"

// standart headers
#include <iostream>
#include <vector>

// OpenCV headers
#include <opencv2/video/tracking.hpp>

// my headers
#include "video_processing.h"
#include "fft_performer.h"
#include "data_displayer.h"
#include "vibration_displayer.h"

class VibrationDetector
{
public:
	VibrationDetector();
	~VibrationDetector();
	void ExecuteVibrationDetection();

private:
	// converts RGB to gray scale
	Mat RgbToGray(Mat frame_to_be_grayed);

	// callback functions for detecting the click
	static void SelectPoint(int event, int x, int y, int flags, void* userdata);
	void OnMouse(int event, int x, int y, int flags);

	// Lucas-Kanade tracking
	void LucasKanadeTracking(Mat prev_img_gray, Mat next_img_gray, std::vector<Point2f>& prev_pts, std::vector<Point2f>& next_pts, std::vector<uchar>& status);

	// Lucas-Kanade auto-tracking
	void GoodFeaturesToTrack(int MAX_PTS);

	// draws a track of movements
	void DrawLines(std::vector<Point2f> prev_pts, std::vector<Point2f> next_pts);

	// draws shapes of found contours
	void DrawContours(Mat& frame, std::vector<std::vector<Point>> contour_shapes);

private:
	String main_window_name_;
	String v_monitor_window_name_;

	Mat current_tracking_frame_;
	
	// for detecting the click
	bool point_selected_;
	Point2f click_coords_;

	// for Lucas-Kanade tracking
	Mat prev_img_gray_;
	Mat next_img_gray_;
	std::vector<Point2f> prev_pts_;
	std::vector<Point2f> next_pts_;
	std::vector<Point2f> contour_prev_pts_;
	std::vector<Point2f> contour_next_pts_;
	std::vector<uchar> status_; // 1 if features found, 0 if not
	std::vector<uchar> contour_status_;
	int lk_win_size_;

	// useful parameters
	int number_of_points_;

	// Fast Fourier Transform
	int sampling_frequency_;
	FftPerformer* fft_performer_;
	std::vector<FftPerformer> vec_of_fft_performers_;

	// for displaying data
	DataDisplayer* data_displayer_;
	std::vector<DataDisplayer> vec_of_data_displayer_;
	std::vector<float> vec_of_frequencies_; // for a certain point

	// for displaying vibration with points
	std::vector<std::vector<Point>> contour_shapes_;
};

#endif