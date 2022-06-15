#include "video_processing.h"

VideoProcessor::VideoProcessor(const std::string input_file_name, const std::string window_name)
	:input_path_{ input_file_name },
	input_cap_{ new VideoCapture(input_file_name) },
	window_name_{ window_name },
	current_time_of_frame_{ 0 },
	input_fps_{ 0 },
	input_frame_height_{ 0 },
	input_frame_width_{ 0 }

{
	std::cout << "VideoProcessor Constructor" << std::endl;
}

VideoProcessor::~VideoProcessor()
{
	std::cout << "VideoProcessor Destructor" << std::endl;

	this->input_cap_->release();
	delete[] this->input_cap_;

	destroyAllWindows;
}

bool VideoProcessor::Init()
{
	// input file + initialize capturing
	if (!input_cap_->isOpened()) {
		std::cout << "Error: unable to open input video file" << std::endl;
		return false;
	}

	// getting height and width of a frame
	this->input_frame_height_ = input_cap_->get(CAP_PROP_FRAME_HEIGHT);
	this->input_frame_width_ = input_cap_->get(CAP_PROP_FRAME_WIDTH);

	// making a window
	namedWindow(window_name_, WINDOW_AUTOSIZE);

	// getting fps of a video
	this->input_fps_ = input_cap_->get(CAP_PROP_FPS);

	// TODO: output file

	std::cout << "//////// Inited successfully ////////" << std::endl;
	return true;
}

void VideoProcessor::ReadNextFrame()
{
	this->input_cap_->read(input_frame_);
	this->current_time_of_frame_ = input_cap_->get(CAP_PROP_POS_MSEC);
	this->current_pos_of_frame_ = input_cap_->get(CAP_PROP_POS_FRAMES);
	input_frame_.copyTo(this->current_frame_);
}

void VideoProcessor::ShowFrame(Mat frame)
{
	imshow(window_name_, frame);
}

bool VideoProcessor::GetInputCapStatus()
{
	return input_cap_->isOpened();
}

Mat VideoProcessor::GetCurrentFrame()
{
	return current_frame_;
}

String VideoProcessor::GetWindowName()
{
	return window_name_;
}

double VideoProcessor::GetCurrentTimeOfFrame()
{
	return current_time_of_frame_;
}

int VideoProcessor::GetCurrentPosOfFrame()
{
	return current_pos_of_frame_;
}

int VideoProcessor::GetInputFps()
{
	return input_fps_;
}