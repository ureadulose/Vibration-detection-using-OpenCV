#include "data_displayer.h"

void DataDisplayer::OutputVibrationParameters(Mat& frame, Point2f point, double amplitude_coefficient, double amplitude)
{
	for (int j = 0; j < static_cast<int>(vec_of_frequencies_.size()); j++)
	{
		if (!vec_of_frequencies_.empty())
		{
			putText(
				frame,
				"hz: " + std::to_string(vec_of_frequencies_[j]),
				Point(point.x + 15, point.y + j * 20 + 25),
				FONT_HERSHEY_PLAIN,
				1.25,
				Scalar(0, 69, 255),
				2
			);
			
		}
	}
	putText(
		frame,
		"amplitude: " + std::to_string(amplitude * amplitude_coefficient),
		Point(point.x + 15, point.y + (vec_of_frequencies_.size() - 1) * 20 + 45),
		FONT_HERSHEY_PLAIN,
		1.25,
		Scalar(0, 69, 255),
		2
	);

}

void DataDisplayer::SetVectorOfFrequencies(std::vector<float> vec_of_frequencies)
{
	vec_of_frequencies_ = vec_of_frequencies;
}
