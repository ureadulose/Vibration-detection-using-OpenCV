#include "vibration_detector.h"

VibrationDetector::VibrationDetector(std::string input_file_name, std::string output_file_name, std::string window_name) :
	input_file_name_{ input_file_name },
	output_file_name_{ output_file_name },
	window_name_{ window_name },
	fft_performer_{nullptr},
	number_of_points_{ 0 },
	vec_of_frequencies_{ 0 },
	amplitude_coeff_{ 1.0 }
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
		std::cout << "l_click" << std::endl;
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

	/*cornerSubPix(
		image,
		corners,
		Size(lk_win_size_, lk_win_size_),
		Size(-1, -1),
		TermCriteria(
			TermCriteria::MAX_ITER | TermCriteria::EPS,
			20,
			0.03
		)
	);*/

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


void VibrationDetector::DrawLines(std::vector<Point2f> prev_pts, std::vector<Point2f> next_pts, Mat& frame)
{
	for (int current_tracking_point = 0; current_tracking_point < static_cast<int>(prev_pts.size()); current_tracking_point++) {
		//if (status_[current_tracking_point] == 1) {

		//	// line between two dots (next and previous)
		//	line(frame,
		//		prev_pts[current_tracking_point],
		//		next_pts[current_tracking_point],
		//		Scalar(0, 255, 0),
		//		1,
		//		LINE_AA
		//	);

		//	// circle
		//	circle(frame, next_pts[current_tracking_point], 10, (0, 0, 255), 2);
		//}
		
		// line between two dots (next and previous)
		line(frame,
			prev_pts[current_tracking_point],
			next_pts[current_tracking_point],
			Scalar(0, 255, 0),
			1,
			LINE_AA
		);

		// circle
		circle(frame, next_pts[current_tracking_point], 10, (0, 0, 255), 2);
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
	VideoProcessor frame_processor(input_file_name_, output_file_name_, MAIN_WINDOW_NAME);
	frame_processor.Init();

	VibrationDisplayer vibration_displayer(V_MONITOR_WINDOW_NAME, frame_processor.GetFrameWidth(), frame_processor.GetFrameHeight());
	vibration_displayer.Init();

	vibration_inited_ = false;
	colors_inited_ = false;

	// reading the first frame of sequence so we can convert it to gray color space
	frame_processor.ReadNextFrame();
	current_tracking_frame_ = frame_processor.GetCurrentFrame();
	prev_img_gray_ = RgbToGray(frame_processor.GetCurrentFrame());
	sampling_frequency_ = frame_processor.GetInputFps();

	// conditions of exit
	running_ = true;
	int current_num_of_frame = 0;
	int amount_of_frames = frame_processor.GetAmountOfFrames();

	while ((current_num_of_frame <= amount_of_frames) && running_ == true)
	{
		current_num_of_frame = frame_processor.GetCurrentPosOfFrame();
		// reading next frame and converting it to gray color space
		frame_processor.ReadNextFrame();
		current_tracking_frame_ = frame_processor.GetCurrentFrame();
		next_img_gray_ = RgbToGray(current_tracking_frame_);

		// callback function for detecting the click - these coords are our starting point
		setMouseCallback(frame_processor.GetWindowName(), SelectPoint, (void*)this);

		if (this->right_button_down_)
		{
			this->roi_ = Rect(tl_click_coords_, mouse_move_coords_);
			rectangle(current_tracking_frame_, roi_.tl(), roi_.br(), Scalar(0, 255, 0), 1);
			//vibration_displayer.ShowFrame();
		}

		if (this->rectangle_selected_)
		{
			vibration_displayer.SetRoi(Rect(tl_click_coords_, mouse_move_coords_));
		}

		// initialize vibration displaying (second window)
		if ((this->rectangle_selected_) && (vibration_inited_ == false))
		{
			vibration_inited_ = true;
			prev_vibrating_pts_ = GoodFeaturesToTrack(next_img_gray_, 500, Rect(tl_click_coords_, br_click_coords_));
			number_of_vibrating_pts_ = prev_vibrating_pts_.size();

			// sending just found points to vibration displayer and initializing colors of these points
			vibration_displayer.UpdateDisplayingPoints(prev_vibrating_pts_);
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

			vec_of_updated_frequencies_.clear();

			// executing fft for each point
			for (int i = 0; i < number_of_vibrating_pts_; i++)
			{
				vec_of_rect_fft_performers_[i].CollectTrackedPoints(frame_processor.GetCurrentPosOfFrame(), next_vibrating_pts_[i], frame_processor.GetCurrentPosOfFrame(), i);

				if (((vec_of_rect_fft_performers_[i].GetLengthOfPointData()) % 3 == 0) && (vec_of_rect_fft_performers_[i].GetLengthOfPointData() != 0))
				{
					vec_of_rect_frequencies_.clear();
					vec_of_rect_frequencies_ = vec_of_rect_fft_performers_[i].ExecuteFft(sampling_frequency_, true); // for a certain point

					rect_point_frequency_ = vec_of_rect_frequencies_[0];
					std::cout << i << " - point. Freq: " << rect_point_frequency_ << std::endl;
					vec_of_updated_frequencies_.push_back(rect_point_frequency_);
				}
			}
			

			vibration_displayer.UpdateFrequencies(vec_of_updated_frequencies_, 15.0);
			/*if (!vec_of_rect_frequencies_.empty())
			{
			}*/

			// it's not actually showing frame. just updating. gonna fix this later
			Mat tmp = current_tracking_frame_;
			vibration_displayer.ProcessFrame(tmp);
			current_tracking_frame_ = vibration_displayer.GetFrame();
		}

		// Lucas-Kanade tracking
		if (this->point_selected_)
		{
			// getting next_pts_ updated from Lucas-Kanade tracking
			LucasKanadeTracking(prev_img_gray_, next_img_gray_, prev_pts_, next_pts_, status_);

			// collecting just tracked points
			std::cout << "doin " << std::endl;
			for (int i = 0; i < next_pts_.size(); i++)
			{
				vec_of_fft_performers_[i].CollectTrackedPoints(frame_processor.GetCurrentPosOfFrame(), next_pts_[i], frame_processor.GetCurrentTimeOfFrame(), i);

				if (((vec_of_fft_performers_[i].GetLengthOfPointData()) % 3 == 0) && (vec_of_fft_performers_[i].GetLengthOfPointData() != 0))
				{

					vec_of_frequencies_.clear();
					vec_of_frequencies_ = vec_of_fft_performers_[i].ExecuteFft(sampling_frequency_, false, i); // for a certain point

					vec_of_data_displayer_[i].SetVectorOfFrequencies(vec_of_frequencies_);
					
					current_amplitude_ = vec_of_fft_performers_[i].GetAmplitude();
				}
				
				vec_of_data_displayer_[i].OutputVibrationParameters(current_tracking_frame_, next_pts_[i], amplitude_coeff_, current_amplitude_);
			}

			/*contour_prev_pts_ = prev_pts_;
			contour_next_pts_ = next_pts_;
			LucasKanadeTracking(prev_img_gray_, next_img_gray_, contour_prev_pts_, contour_next_pts_, contour_status_);*/

			// drawing lines
			DrawLines(prev_pts_, next_pts_, current_tracking_frame_);

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

		//if (!prev_pts_.empty())
		//{
		//	for (int current_tracking_point = 0; current_tracking_point < static_cast<int>(prev_pts_.size()); current_tracking_point++) {
		//		// line between two dots (next and previous)
		//		line(current_tracking_frame_,
		//			prev_pts_[current_tracking_point],
		//			next_pts_[current_tracking_point],
		//			Scalar(0, 255, 0),
		//			1,
		//			LINE_AA
		//		);
		//		std::cout << "risuyu" << std::endl;
		//		// circle
		//		circle(current_tracking_frame_, next_pts_[current_tracking_point], 10, (0, 0, 255), 2);
		//	}
		//}
		

		// display and write frame
		frame_processor.ShowFrame(current_tracking_frame_);
		frame_processor.WriteFrame(current_tracking_frame_);

		// 20 - delay in ms
		int code = waitKey(20);
		switch (code)
		{
		case 'v':
		{
			std::cout << "waiting..." << std::endl;
			AmplitudeCalibrator amplitude_calibrator(current_tracking_frame_.clone());
			amplitude_calibrator.Execute();
			amplitude_coeff_ = amplitude_calibrator.GetCmPerPixelRatio();
			waitKey(0);
			std::cout << "continuing..." << std::endl;
			break;
		}
		case 'q':
		{
			frame_processor.~VideoProcessor();
			running_ = false;
			std::cout << frame_processor.GetInputCapStatus() << std::endl;
			break;
		}
		}
	}
}

Mat VibrationDetector::RgbToGray(Mat frame_to_be_grayed)
{
	Mat grayed_frame;
	cvtColor(frame_to_be_grayed, grayed_frame, COLOR_BGR2GRAY);
	return grayed_frame;
}