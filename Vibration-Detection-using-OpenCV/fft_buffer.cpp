#include "fft_buffer.h"

FftPerformer::FftPerformer()
	:size_of_vecs_{ 0 }
{
	
}

FftPerformer::~FftPerformer()
{

}

void FftPerformer::ClearTrash()
{
	// clearing out vectors
	y_values_.clear();
	fft_result_.clear();
	p1_.clear();
	p2_.clear();
	indexes_of_target_frequencies_.clear();
}

std::vector<float> FftPerformer::ExecuteFft(int sampling_frequency)
{
	this->sampling_frequency_ = sampling_frequency;
	this->size_of_vecs_ = coordinates_of_point_.size();

	ClearTrash();

	SubtractMean();
	dft(coordinates_of_point_, fft_result_);

	// computing two-sided spectrum P2
	for (int i = 0; i < fft_result_.size(); i++)
	{
		Point2f tmp;
		tmp.x = abs(fft_result_[i].x / size_of_vecs_);
		tmp.y = abs(fft_result_[i].y / size_of_vecs_);
		p2_.push_back(tmp);
	}

	std::cout << coordinates_of_point_.size() << std::endl;

	// computing single-sided spectrum P1 based on P2 and the even-valued signal length (the same as int size_of_vecs_)
	for (int i = 0; i < p2_.size(); i++)
	{
		p1_.push_back(p2_[(int)(i / 2 + 1)]);
	}

	// y_values are possible frequencies
	for (int i = 0; i < size_of_vecs_; i++)
	{
		float tmp = sampling_frequency_ * (i / 2) / size_of_vecs_;
		y_values_.push_back(tmp);
	}

	float max_magnitude = 0;
	
	std::vector<float> magnitudes; // im creating this vec here cause i dont need to know what are magnidutes of previous calculation of fft

	for (int i = 0; i < p1_.size(); i++)
	{
		float current_magnitude = sqrt(p1_[i].x * p1_[i].x + p1_[i].y * p1_[i].y);
		magnitudes.push_back(current_magnitude);
	}

	double threshold = (double)0.0;

	// https://github.com/chongkaechin/FindMultiplePeaksAlgorithm
	// усебне цнбмн
	// оепедекюи
	PeakFinder::findPeaks(magnitudes, indexes_of_target_frequencies_, false, 1);

	std::vector<float> dst;

	for (int i = 0; i < indexes_of_target_frequencies_.size(); i++)
	{
		dst.push_back(y_values_[indexes_of_target_frequencies_[i]]);
	}

	return dst;
}

void FftPerformer::CollectTrackedPoints(int frame_pos_of_point, Point2f coordinates_of_points, double frame_time_of_point, int num)
{

	this->coordinates_of_point_.push_back(coordinates_of_points);
	this->frame_pos_of_point_.push_back(frame_pos_of_point);
	this->frame_time_of_point_.push_back(frame_time_of_point);
	size_of_vecs_++;
}

void FftPerformer::WriteSpectrumToTxt()
{
	std::fstream file;
	file.open("data.txt", 'w');

	for (int i = 0; i < size_of_vecs_; i++)
	{
		std::string tmp = std::to_string(y_values_[i]) + ' ' + std::to_string(p1_[i].x) + ' ' + std::to_string(p1_[i].y);
		file << tmp << std::endl;
	}

	std::cout << "WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN" << std::endl;

	file.close();
}

void FftPerformer::WriteDataToTxt()
{
	std::fstream file;
	file.open("data.txt", 'w');

	if (file.is_open())
	{
	}

	for (int i = 0; i < this->coordinates_of_point_.size(); i++)
	{
		std::string tmp = std::to_string(coordinates_of_point_[i].x) + ' ' + std::to_string(coordinates_of_point_[i].y) + ' ' + std::to_string(frame_time_of_point_[i]);
		file << tmp << std::endl;
	}

	std::cout << "WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN" << std::endl;

	file.close();
}

int FftPerformer::GetSizeOfVecs()
{
	return size_of_vecs_;
}

void FftPerformer::SubtractMean()
{
	meaned_coordinates_of_point_ = mean(coordinates_of_point_);
	double meaned_x = meaned_coordinates_of_point_.val[0];
	double meaned_y = meaned_coordinates_of_point_.val[1];

	for (int i = 0; i < coordinates_of_point_.size(); i++)
	{
		coordinates_of_point_[i].x -= meaned_x;
		coordinates_of_point_[i].y -= meaned_y;
	}
}

std::vector<Point2f> FftPerformer::HammingWindow()
{

	return std::vector<Point2f>();
}

std::vector<Point2f> FftPerformer::HighPass()
{

	return std::vector<Point2f>();
}