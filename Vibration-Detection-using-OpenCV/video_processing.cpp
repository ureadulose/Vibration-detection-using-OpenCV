#include "video_processing.h"

VideoProcessor::VideoProcessor(const std::string input_file_name, const std::string output_file_name, const std::string window_name)
	:input_path_{ input_file_name },
	input_cap_{ new VideoCapture(input_file_name) },
	input_cap_status_{ false },
	output_path_{ output_file_name },
	output_cap_{ new VideoWriter(output_file_name, VideoWriter::fourcc('M', 'J', 'P', 'G'), input_cap_->get(CAP_PROP_FPS), Size(input_cap_->get(CAP_PROP_FRAME_WIDTH), input_cap_->get(CAP_PROP_FRAME_HEIGHT))) },
	window_name_{ window_name },
	current_time_of_frame_{ 0 },
	input_fps_{ input_cap_->get(CAP_PROP_FPS) },
	input_frame_height_{ 0 },
	input_frame_width_{ 0 }

{
}

VideoProcessor::~VideoProcessor()
{
	this->input_cap_->release();
	delete this->input_cap_;
	this->output_cap_->release();
	delete this->output_cap_;

	destroyWindow(window_name_);
}

bool VideoProcessor::Init()
{
	std::cout << "////////////////////////////////////////////////////////////////////////////////////" << std::endl;
	std::cout << input_fps_ << std::endl;

	// input file + initialize capturing
	if (!input_cap_->isOpened()) {
		std::cout << "Error: unable to open input video file" << std::endl;
		return false;
	}

	// getting height and width of a frame
	input_frame_height_ = input_cap_->get(CAP_PROP_FRAME_HEIGHT);
	input_frame_width_ = input_cap_->get(CAP_PROP_FRAME_WIDTH);

	// making a window
	namedWindow(window_name_, WINDOW_AUTOSIZE);

	// TODO: output file

	std::cout << "//////// Inited successfully ////////" << std::endl;
	return true;
}

void VideoProcessor::ReadNextFrame()
{
	input_cap_status_ = input_cap_->read(input_frame_);
	current_time_of_frame_ = input_cap_->get(CAP_PROP_POS_MSEC);
	current_pos_of_frame_ = input_cap_->get(CAP_PROP_POS_FRAMES);
	input_frame_.copyTo(current_frame_);
}

void VideoProcessor::ShowFrame(Mat frame)
{
	imshow(window_name_, frame);
}

void VideoProcessor::WriteFrame(Mat frame)
{
	output_cap_->write(frame);
}

bool VideoProcessor::GetInputCapStatus()
{
	return input_cap_status_;
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

int VideoProcessor::GetFrameWidth()
{
	return input_frame_width_;
}

int VideoProcessor::GetFrameHeight()
{
	return input_frame_height_;
}