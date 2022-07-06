#include "video_undistorter.h"

VideoUndistorter::VideoUndistorter(std::string input_file_name, std::string output_file_name, std::string txt_file_name) :
	input_file_name_{ input_file_name },
	output_file_name_{ output_file_name },
	txt_file_name_{ txt_file_name }
{
	input_cap_ = new VideoCapture(input_file_name);

	frame_width_ = input_cap_->get(CAP_PROP_FRAME_WIDTH);
	frame_height_ = input_cap_->get(CAP_PROP_FRAME_HEIGHT);
	bitrate_ = input_cap_->get(CAP_PROP_BITRATE);
	frame_count_ = input_cap_->get(CAP_PROP_FRAME_COUNT);
	fps_ = input_cap_->get(CAP_PROP_FPS);

	output_cap_ = new VideoWriter
	(
		output_file_name_,
		cv::VideoWriter::fourcc('m', 'p', '4', 'v'), // codec
		fps_,
		Size(frame_width_, frame_height_)
	);

}

VideoUndistorter::~VideoUndistorter()
{
	input_cap_->release();
	output_cap_->release();

	delete[] input_cap_;
	delete[] output_cap_;
}

void VideoUndistorter::ExecuteVideoUndistortion()
{
	// read distortion parameters from txt file
	LoadFoundParamsFromFile(txt_file_name_);

	std::cout << frame_count_ << std::endl;

	int frame_num = 0;

	//undistort each frame in loop
	while (input_cap_->get(CAP_PROP_POS_FRAMES) < frame_count_)
	{
		input_cap_->read(current_frame_);
		if (!current_frame_.empty())
		{
			std::cout << "Processing frame " << input_cap_->get(CAP_PROP_POS_FRAMES) << "/" << frame_count_ << std::endl;
			Mat mapX, mapY;
			initUndistortRectifyMap(camera_matrix_, distortion_coefficients_, Matx33f::eye(), camera_matrix_, Size(frame_width_, frame_height_), CV_32FC1, mapX, mapY);
			//undistort(current_frame_, undistorted_frame_, camera_matrix_, distortion_coefficients_);
			remap(current_frame_, undistorted_frame_, mapX, mapY, cv::INTER_LINEAR);
			SaveFrame(undistorted_frame_);
		}
	}

	std::cout << "Undistortion completed" << std::endl;
}

void VideoUndistorter::SaveFrame(Mat frame)
{
	output_cap_->write(frame);
}

void VideoUndistorter::LoadFoundParamsFromFile(std::string txt_file_name)
{
	
	std::vector<double>* dist = new std::vector<double>(0);

	input_file_.open(txt_file_name);
	std::string line;

	if (input_file_.is_open())
	{
		std::cout << "Param file opened" << std::endl;
		while (std::getline(input_file_, line))
		{
			//std::cout << "Read line: " << line << '\n';
			std::vector<cv::String> line_sp = SplitString(line, ":");
			if (line_sp[0] == "fx")
				fx_ = std::stod(line_sp[1]);
			else if (line_sp[0] == "fy")
				fy_ = std::stod(line_sp[1]);
			else if (line_sp[0] == "px")
				px_ = std::stod(line_sp[1]);
			else if (line_sp[0] == "py")
				py_ = std::stod(line_sp[1]);
			else if (line_sp[0] == "dist")
			{
				std::vector<cv::String> dist_sp = SplitString(line_sp[1], ",");
				dist->resize(dist_sp.size());
				for (int n = 0; n < dist_sp.size(); n++)
					dist->at(n) = std::stod(dist_sp[n]);
			}
		}
		input_file_.close();
	}

	std::vector<double>* intrin_arr = new std::vector<double>{ fx_, 0, px_, 0, fy_, py_, 0, 0, 1 };
	intrin_arr->data();
	camera_matrix_ = Mat(3, 3, CV_64F, intrin_arr->data());
	distortion_coefficients_ = Mat(1, dist->size(), CV_64F, dist->data());

	std::cout << camera_matrix_.size() << std::endl;
	std::cout << distortion_coefficients_.size() << std::endl;
}

std::vector<std::string> VideoUndistorter::SplitString(std::string input_string, std::string delimiter)
{
	int start_position = 0;
	int end_position;
	int delimiter_length = delimiter.length();

	std::string token;
	std::vector<std::string> dst;

	while ((end_position = input_string.find(delimiter, start_position)) != std::string::npos) {
		token = input_string.substr(start_position, end_position - start_position);
		start_position = end_position + delimiter_length;
		dst.push_back(token);
	}

	dst.push_back(input_string.substr(start_position));
	return dst;
}

