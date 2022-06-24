#include "contour_finder.h"

std::vector<std::vector<Point>> ContourFinder::GetContours(Mat frame)
{
	std::vector<std::vector<Point>> suitable_contour_shapes;

	// preparing a frame for finding contours
	Mat prep_frame;
	cvtColor(frame, prep_frame, COLOR_BGR2GRAY);
	GaussianBlur(prep_frame, prep_frame, Size(7, 7), 0);
	Canny(prep_frame, prep_frame, 64, 192);

	// doing dzhob
	std::vector<std::vector<Point>> contours = { {Point(0,0)} };
	std::vector<Vec4i> hierarchy = { 0 };

	findContours(prep_frame, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<Point>> contour_shapes(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		int perimeter_of_contour = arcLength(contours[i], true);

		// check if area is suitable
		if (area > 50)
		{
			approxPolyDP(contours[i], contour_shapes[i], 0.02 * perimeter_of_contour, true);
			suitable_contour_shapes.push_back(contour_shapes[i]);
		}
	}



	return suitable_contour_shapes;
}