#ifndef POINTS_TRACKER_H_
#define POINTS_TRACKER_H_

// VREMENNO
#define INPUT_FILE_NAME "Resources/engine.mp4" // leaf.mp4 or engine.mp4 or vibration1.mp4
#define WINDOW_NAME "Video"

// standart headers
#include <iostream>
#include <vector>

// OpenCV headers
#include <opencv2/video/tracking.hpp>

// my headers
#include "video_processing.h"
#include "fft_buffer.h"
#include "data_displayer.h"
#include "contour_finder.h"

class PointsTracker
{
public:
	PointsTracker();
	~PointsTracker();
	void ExecuteTracking();



	///////////////// FOR FAST FOURIER TRANSFORM //////////////////

private:
	// Converts RGB to gray scale
	Mat RgbToGray(Mat frame_to_be_grayed);

	// Callback functions for detecting the click
	static void SelectPoint(int event, int x, int y, int flags, void* userdata);
	void OnMouse(int event, int x, int y, int flags);

	// Lucas-Kanade tracking
	void LucasKanadeTracking();

	// Lucas-Kanade auto-tracking
	void GoodFeaturesToTrack(int MAX_PTS);

	// Draws a track of movements
	void DrawLines(std::vector<Point2f> prev_pts, std::vector<Point2f> next_pts);



	///////////////// FOR FAST FOURIER TRANSFORM //////////////////


private:
	String window_name_;

	Mat current_tracking_frame_;
	
	// for detecting the click
	bool point_selected_;
	Point2f click_coords_;


	// for Lucas-Kanade tracking
	Mat prev_img_gray_;
	Mat next_img_gray_;
	std::vector<Point2f> prev_pts_;
	std::vector<Point2f> next_pts_;
	std::vector<uchar> status_; // 1 if features found, 0 if not
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
};

#endif