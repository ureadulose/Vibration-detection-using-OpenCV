#include "vibration_detector.h"

VibrationDetector::VibrationDetector()
	:fft_performer_{nullptr},
	number_of_points_{ 0 },
	vec_of_frequencies_{ 0 }
{
	point_selected_ = false;
	lk_win_size_ = 45;
}

VibrationDetector::~VibrationDetector()
{
	delete[] this->fft_performer_;
	delete[] this->data_displayer_;
}

// callback function for determining the event click on mouse
void VibrationDetector::SelectPoint(int event, int x, int y, int flags, void* userdata)
{
	VibrationDetector* d = static_cast<VibrationDetector*>(userdata);
	d->OnMouse(event, x, y, flags);
}

// "helper" function for implementing callback function as a method of C++ class
void VibrationDetector::OnMouse(int event, int x, int y, int flags)
{
	switch (event) {
	case EVENT_LBUTTONDOWN:
		this->point_selected_ = true;
		this->click_coords_.x = x;
		this->click_coords_.y = y;
		this->prev_pts_.push_back(click_coords_);

		this->number_of_points_++;

		this->fft_performer_ = new FftPerformer();
		this->vec_of_fft_performers_.push_back(*fft_performer_);

		this->data_displayer_ = new DataDisplayer();
		this->vec_of_data_displayer_.push_back(*data_displayer_);
	}
}

void VibrationDetector::GoodFeaturesToTrack(int MAX_PTS)
{
	// auto-search for good points (features) to track
	goodFeaturesToTrack(
		prev_img_gray_,
		prev_pts_,
		MAX_PTS,
		0.01,
		5,
		noArray(),
		3,
		false,
		0.04
	);

	cornerSubPix(
		prev_img_gray_,
		prev_pts_,
		Size(lk_win_size_, lk_win_size_),
		Size(-1, -1),
		TermCriteria(
			TermCriteria::MAX_ITER | TermCriteria::EPS,
			20,
			0.03
		)
	);
}

void VibrationDetector::LucasKanadeTracking()
{
	// calling Lucas-Kanade algorithm
	calcOpticalFlowPyrLK(
		prev_img_gray_,
		next_img_gray_,
		prev_pts_,
		next_pts_,
		status_,
		noArray(),
		Size(lk_win_size_ * 2 + 1, lk_win_size_ * 2 + 1),
		5,
		TermCriteria(
			TermCriteria::MAX_ITER | TermCriteria::EPS,
			50,
			0.03
		)
	);
}

void VibrationDetector::DrawLines(std::vector<Point2f> prev_pts, std::vector<Point2f> next_pts)
{
	for (int current_tracking_point = 0; current_tracking_point < static_cast<int>(prev_pts.size()); current_tracking_point++) {
		if (status_[current_tracking_point] == 1) {

			// line between two dots (next and previous)
			line(current_tracking_frame_,
				prev_pts[current_tracking_point],
				next_pts[current_tracking_point],
				Scalar(0, 255, 0),
				1,
				LINE_AA
			);

			// circle
			circle(current_tracking_frame_, next_pts[current_tracking_point], 10, (0, 0, 255), 2);
		}
	}
}

void VibrationDetector::ExecuteVibrationDetection()
{
	VideoProcessor sequence_of_frames(INPUT_FILE_NAME, MAIN_WINDOW_NAME);
	sequence_of_frames.Init();

	VibrationDisplayer vibration_monitor(V_MONITOR_WINDOW_NAME, sequence_of_frames.GetFrameWidth(), sequence_of_frames.GetFrameHeight());
	vibration_monitor.Init();

	// reading the first frame of sequence so we can convert it to gray color space
	sequence_of_frames.ReadNextFrame();
	this->current_tracking_frame_ = sequence_of_frames.GetCurrentFrame();
	this->prev_img_gray_ = RgbToGray(sequence_of_frames.GetCurrentFrame());
	this->sampling_frequency_ = sequence_of_frames.GetInputFps();

	//ContourFinder contour_finder;
	//std::vector<std::vector<Point>> contour_shapes;

	while (sequence_of_frames.GetInputCapStatus())
	{
		// reading next frame and converting it to gray color space
		sequence_of_frames.ReadNextFrame();
		this->current_tracking_frame_ = sequence_of_frames.GetCurrentFrame();
		this->next_img_gray_ = RgbToGray(current_tracking_frame_);

		// callback function for detecting the click - these coords are our starting point
		setMouseCallback(sequence_of_frames.GetWindowName(), SelectPoint, (void*)this);

		// Lucas-Kanade tracking
		if (this->point_selected_)
		{
			// getting next_pts_ updated from Lucas-Kanade tracking
			LucasKanadeTracking();

			// collecting just tracked points
			for (int i = 0; i < number_of_points_; i++)
			{
				vec_of_fft_performers_[i].CollectTrackedPoints(sequence_of_frames.GetCurrentPosOfFrame(), next_pts_[i], sequence_of_frames.GetCurrentTimeOfFrame(), i);

				if (((vec_of_fft_performers_[i].GetSizeOfVecs()) % 30 == 0) && (vec_of_fft_performers_[i].GetSizeOfVecs() != 0))
				{
					vec_of_frequencies_.clear();
					vec_of_frequencies_ = vec_of_fft_performers_[i].ExecuteFft(sampling_frequency_); // for a certain point
				}
				if (vec_of_fft_performers_[i].GetSizeOfVecs() > 30)
				{
					// output data on the frame
					data_displayer_[i].output_vibration_parameters(current_tracking_frame_, next_pts_[i], vec_of_frequencies_);
				}
			}

			// drawing lines
			DrawLines(prev_pts_, next_pts_);

			// update points and frames
			this->prev_pts_ = next_pts_;
			next_img_gray_.copyTo(this->prev_img_gray_);
		}

		// display frame
		sequence_of_frames.ShowFrame(current_tracking_frame_);
		waitKey(20);
	}

	sequence_of_frames.~VideoProcessor();
}

Mat VibrationDetector::RgbToGray(Mat frame_to_be_grayed)
{
	Mat grayed_frame;
	cvtColor(frame_to_be_grayed, grayed_frame, COLOR_BGR2GRAY);
	return grayed_frame;
}