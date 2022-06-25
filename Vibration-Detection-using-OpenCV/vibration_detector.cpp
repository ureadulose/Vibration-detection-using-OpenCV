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
		break;
	case EVENT_RBUTTONDOWN:
		// we need to initialize coordinates so if we select roi again, the default bottim right coordinates would be reset
		this->right_button_down_ = true;
		this->tl_click_coords_.x = x;
		this->tl_click_coords_.y = y;
		this->br_click_coords_.x = x;
		this->br_click_coords_.y = y;
		mouse_move_coords_.x = x;
		mouse_move_coords_.y = y;
		break;
	case EVENT_RBUTTONUP:
		this->right_button_down_ = false;
		this->rectangle_selected_ = true;
		this->br_click_coords_.x = x;
		this->br_click_coords_.y = y;
		break;
	case EVENT_MOUSEMOVE:
		if (right_button_down_)
		{
			mouse_move_coords_.x = x;
			mouse_move_coords_.y = y;
		}
		break;
	}
}

std::vector<Point2f> VibrationDetector::GoodFeaturesToTrack(Mat frame, int MAX_PTS, const Rect& roi)
{
	std::vector<Point2f> corners;

	// adding rectangle of interest to a frame
	Mat image = frame(roi);

	// auto-search for good points (features) to track
	goodFeaturesToTrack(
		image,
		corners,
		MAX_PTS,
		0.01,
		5,
		noArray(),
		3,
		false,
		0.04
	);

	cornerSubPix(
		image,
		corners,
		Size(lk_win_size_, lk_win_size_),
		Size(-1, -1),
		TermCriteria(
			TermCriteria::MAX_ITER | TermCriteria::EPS,
			20,
			0.03
		)
	);

	return corners;
}

void VibrationDetector::LucasKanadeTracking(Mat prev_img_gray, Mat next_img_gray, std::vector<Point2f>& prev_pts, std::vector<Point2f>& next_pts, std::vector<uchar>& status)
{
	// calling Lucas-Kanade algorithm
	calcOpticalFlowPyrLK(
		prev_img_gray,
		next_img_gray,
		prev_pts,
		next_pts,
		status,
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

void VibrationDetector::DrawContours(Mat& frame, std::vector<std::vector<Point>> contour_shapes)
{
	for (int i = 0; i < contour_shapes_.size(); i++)
	{
		drawContours(current_tracking_frame_, contour_shapes, i, Scalar(255, 0, 255), 1);
	}
}

void VibrationDetector::ExecuteVibrationDetection()
{
	VideoProcessor sequence_of_frames(INPUT_FILE_NAME, MAIN_WINDOW_NAME);
	sequence_of_frames.Init();

	VibrationDisplayer vibration_displayer(V_MONITOR_WINDOW_NAME, sequence_of_frames.GetFrameWidth(), sequence_of_frames.GetFrameHeight());
	vibration_displayer.Init();

	vibration_inited_ = false;
	colors_inited_ = false;

	// reading the first frame of sequence so we can convert it to gray color space
	sequence_of_frames.ReadNextFrame();
	this->current_tracking_frame_ = sequence_of_frames.GetCurrentFrame();
	this->prev_img_gray_ = RgbToGray(sequence_of_frames.GetCurrentFrame());
	this->sampling_frequency_ = sequence_of_frames.GetInputFps();

	while (sequence_of_frames.GetInputCapStatus())
	{
		// reading next frame and converting it to gray color space
		sequence_of_frames.ReadNextFrame();
		this->current_tracking_frame_ = sequence_of_frames.GetCurrentFrame();
		this->next_img_gray_ = RgbToGray(current_tracking_frame_);

		// callback function for detecting the click - these coords are our starting point
		setMouseCallback(sequence_of_frames.GetWindowName(), SelectPoint, (void*)this);

		if (this->right_button_down_)
		{
			std::cout << "Selecting ROI" << std::endl;
			this->roi_ = Rect(tl_click_coords_, mouse_move_coords_);
			rectangle(current_tracking_frame_, roi_.tl(), roi_.br(), Scalar(0, 255, 0), 1);
			//vibration_displayer.ShowFrame();
		}

		if (this->rectangle_selected_)
		{
			std::cout << "ROI selected" << std::endl;
			vibration_displayer.SetRoi(Rect(tl_click_coords_, mouse_move_coords_));
		}

		// initialize vibration displaying (second window)
		if ((this->rectangle_selected_) && (vibration_inited_ == false))
		{
			vibration_inited_ = true;
			prev_vibrating_pts_ = GoodFeaturesToTrack(next_img_gray_, 100, Rect(tl_click_coords_, br_click_coords_));
			number_of_vibrating_pts_ = prev_vibrating_pts_.size();

			// sending just found points to vibration displayer and initializing colors of these points
			vibration_displayer.UpdateDisplayingPoints(prev_vibrating_pts_);
			std::cout << "Sent " << prev_vibrating_pts_.size() << " points" << std::endl;
			colors_inited_ = vibration_displayer.InitColors();
			
			// creating vector of fft performers for points in ROI
			for (int i = 0; i < number_of_vibrating_pts_; i++)
			{
				this->rect_fft_performer_ = new FftPerformer();
				this->vec_of_rect_fft_performers_.push_back(*rect_fft_performer_);

				// transform coordinates based on roi to overall coordinates
				prev_vibrating_pts_[i].x = prev_vibrating_pts_[i].x + tl_click_coords_.x;
				prev_vibrating_pts_[i].y = prev_vibrating_pts_[i].y + tl_click_coords_.y;
			}
		}

		// display vibration frame
		if ((this->rectangle_selected_) && (vibration_inited_ == true))
		{
			// for main window
			rectangle(current_tracking_frame_, roi_.tl(), roi_.br(), Scalar(0, 255, 0), 1);

			// call of Lucas-Kanade optical flow tracking
			LucasKanadeTracking(prev_img_gray_, next_img_gray_, prev_vibrating_pts_, next_vibrating_pts_, rect_status_);

			// sending and updating new points in ROI found from optical flow
			vibration_displayer.UpdateDisplayingPoints(next_vibrating_pts_);

			for (int i = 0; i < number_of_vibrating_pts_; i++)
			{
				vec_of_rect_fft_performers_[i].CollectTrackedPoints(sequence_of_frames.GetCurrentPosOfFrame(), next_vibrating_pts_[i], sequence_of_frames.GetCurrentPosOfFrame(), i);

				if (((vec_of_rect_fft_performers_[i].GetSizeOfVecs()) % 30 == 0) && (vec_of_rect_fft_performers_[i].GetSizeOfVecs() != 0))
				{
					vec_of_rect_frequencies_.clear();
					vec_of_rect_frequencies_ = vec_of_rect_fft_performers_[i].ExecuteFft(sampling_frequency_); // for a certain point
				}
			}
			if (!vec_of_rect_frequencies_.empty())
			{
				std::cout << "vec of rect frequencies is not empty" << std::endl;
				vibration_displayer.UpdateFrequencies(vec_of_rect_frequencies_, 15.0);
			}

			vibration_displayer.ShowFrame();
		}

		// Lucas-Kanade tracking
		if (this->point_selected_)
		{
			// getting next_pts_ updated from Lucas-Kanade tracking
			LucasKanadeTracking(prev_img_gray_, next_img_gray_, prev_pts_, next_pts_, status_);

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
					data_displayer_[i].OutputVibrationParameters(current_tracking_frame_, next_pts_[i], vec_of_frequencies_);
				}
			}

			/*contour_prev_pts_ = prev_pts_;
			contour_next_pts_ = next_pts_;
			LucasKanadeTracking(prev_img_gray_, next_img_gray_, contour_prev_pts_, contour_next_pts_, contour_status_);*/

			// drawing lines
			DrawLines(prev_pts_, next_pts_);

			// update points and frames
			this->prev_pts_ = next_pts_;
			this->prev_vibrating_pts_ = next_vibrating_pts_;
			next_img_gray_.copyTo(this->prev_img_gray_);
		}

		// cre

		/*// finding contours on a frame
		contour_shapes_ = vibration_displayer.GetContours(current_tracking_frame_);

		std::vector<Point> contour_head_points;
		vibration_displayer.GetContourHeadPoints(contour_shapes_, contour_head_points);

		for (int i = 0; i < contour_head_points.size(); i++)
		{
			// circle
			circle(current_tracking_frame_, contour_head_points[i], 10, (0, 0, 255), 2);
		}

		// displaying vibration
		vibration_displayer.ContourHandler(contour_shapes_);

		// drawing contours
		DrawContours(current_tracking_frame_, contour_shapes_);*/

		

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