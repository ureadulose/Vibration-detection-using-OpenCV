#include "data_displayer.h"

void DataDisplayer::output_vibration_parameters(Mat& frame, Point2f point, std::vector<float> vec_of_frequencies)
{
	std::cout << vec_of_frequencies.size() << std::endl;
	for (int j = 0; j < static_cast<int>(vec_of_frequencies.size()); j++)
	{
		if (!vec_of_frequencies.empty())
		{
			//std::cout << "frequency[j] is " << vec_of_frequencies[j] << std::endl;
			putText(
				frame,
				"hz: " + std::to_string(vec_of_frequencies[j]),
				Point(point.x + 15, point.y + j * 20 + 25),
				FONT_HERSHEY_PLAIN,
				1.25,
				Scalar(0, 69, 255),
				2
			);
		}
	}

}