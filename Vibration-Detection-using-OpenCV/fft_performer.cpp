#include "fft_performer.h"

FftPerformer::FftPerformer()
	:size_of_vecs_{ 0 }
{
	
}

FftPerformer::~FftPerformer()
{

}

void WriteToTxt(std::vector<float> magnitudes)
{
	std::fstream file;
	file.open("magnitudes.txt", 'w');

	for (int i = 0; i < magnitudes.size(); i++)
	{
		std::string tmp = std::to_string(magnitudes[i]);
		file << tmp << std::endl;
	}

	std::cout << "written written written written written written written written" << std::endl;

	file.close();
}

std::vector<float> FftPerformer::ExecuteFft(int sampling_frequency, bool absolute_peak, int debugging_num)
{
	//std::cout << "doin " << debugging_num << std::endl;

	std::vector<Point2f> fft_result;
	std::vector<Point2f> p2;
	std::vector<Point2f> p1;
	std::vector<float> frequencies;
	std::vector<int> indexes_of_peak_frequencies;
	std::vector<float> magnitudes;

	std::vector<float> peak_frequencies;

	this->sampling_frequency_ = sampling_frequency;
	this->size_of_vecs_ = coordinates_of_point_.size();

	// subtracting mean value so the samples would be fluctuating around zero
	std::vector<cv::Point2f> vec_meaned_coordinates_of_point_;

	mean_coordinates_of_point_ = mean(coordinates_of_point_);
	//std::cout << mean_coordinates_of_point_ << std::endl;
	double meaned_x = mean_coordinates_of_point_.val[0];
	double meaned_y = mean_coordinates_of_point_.val[1];
	//std::cout << meaned_x << std::endl;
	//std::cout << meaned_y << std::endl;

	for (int i = 0; i < coordinates_of_point_.size(); i++)
	{

		vec_meaned_coordinates_of_point_.push_back(Point2f(coordinates_of_point_[i].x - meaned_x, coordinates_of_point_[i].y - meaned_y));
		//std::cout << vec_meaned_coordinates_of_point_[i] << std::endl;
	}

	dft(vec_meaned_coordinates_of_point_, fft_result);

	// computing two-sided spectrum P2
	for (int i = 0; i < fft_result.size(); i++)
	{
		Point2f tmp;
		tmp.x = abs(fft_result[i].x / size_of_vecs_);
		tmp.y = abs(fft_result[i].y / size_of_vecs_);
		p2.push_back(tmp);
	}

	// computing single-sided spectrum P1 based on P2 and the even-valued signal length (the same as size_of_vecs_)
	for (int i = 0; i < p2.size(); i++)
	{
		size_t idx = 0;
		idx = (int)(((double)(i)) / 2.0 + 1.0);
		p1.push_back(p2[idx]);
	}

	// possible frequencies
	for (int i = 0; i < size_of_vecs_; i++)
	{
		float tmp = sampling_frequency_ * ((double)(i) / 2) / size_of_vecs_;
		frequencies.push_back(tmp);
	}

	for (int i = 0; i < p1.size(); i++)
	{
		float current_magnitude = sqrt(p1[i].x * p1[i].x + p1[i].y * p1[i].y);
		magnitudes.push_back(current_magnitude);
	}

	/////////////////////////////////////

	/*std::fstream file;
	file.open("magnitudes.txt", 'w');
	
	for (int i = 0; i < magnitudes.size(); i++)
	{
		std::string tmp = std::to_string(frequencies[i]) + " " + std::to_string(magnitudes[i]) + " " + std::to_string(p1[i].x) + " " + std::to_string(p1[i].y);
		file << tmp << std::endl;
	}
	std::cout << "written written written written written written written written" << std::endl;
	file.close();*/

	/////////////////////////////////////

	if (!absolute_peak)
	{
		// searching peaks in output vector of magnitudes
		PeakFinder::findPeaks(magnitudes, indexes_of_peak_frequencies, false, 1);

		// this function works strange, it doesn't always returns the indexes
		// so i decided to do the following
		int maxIdx = 0;
		if (indexes_of_peak_frequencies.empty())
		{
			//std::cout << "alternative way" << std::endl;
			double tmp_max_value = 0.0;
			// here we start from 1 and not 0
			// it's called "lyutiy kostyl"
			for (int i = 1; i < magnitudes.size(); i++)
			{
				if (magnitudes[i] > tmp_max_value)
				{
					tmp_max_value = magnitudes[i];
					maxIdx = i;
				}

			}
			peak_frequencies.push_back(frequencies[maxIdx]);
		}


		// filling in vector of peak_frequencies with the just found peak frequencies 
		for (int i = 0; i < indexes_of_peak_frequencies.size(); i++)
		{
			peak_frequencies.push_back(frequencies[indexes_of_peak_frequencies[i]]);

			//std::cout << "Max is " << frequencies[indexes_of_peak_frequencies[i]] << std::endl;
		}
	}

	if (absolute_peak)
	{
		int maxIdx = 0;
		double tmp_max_value = 0.0;
		// here we start from 1 and not 0
		// it's called "lyutiy kostyl"
		for (int i = 1; i < magnitudes.size(); i++)
		{
			if (magnitudes[i] > tmp_max_value)
			{
				tmp_max_value = magnitudes[i];
				maxIdx = i;
			}

		}
		peak_frequencies.push_back(frequencies[maxIdx]);
	}

	

	return peak_frequencies;
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
	/*std::fstream file;
	file.open("data.txt", 'w');

	for (int i = 0; i < size_of_vecs_; i++)
	{
		std::string tmp = std::to_string(y_values_[i]) + ' ' + std::to_string(p1_[i].x) + ' ' + std::to_string(p1_[i].y);
		file << tmp << std::endl;
	}

	std::cout << "WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN WRITTEN" << std::endl;

	file.close();*/
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

int FftPerformer::GetLengthOfPointData()
{
	return size_of_vecs_;
}