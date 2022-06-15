#include "contour_finder.h"

std::vector<std::vector<Point>> ContourFinder::GetContours(Mat& image)
{
	std::vector<std::vector<Point>> contours = { {Point(0,0)} };
	std::vector<Vec4i> hierarchy = { 0 };

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<Point>> contour_shapes(contours.size());
	std::vector<Rect> bounding_rectangles(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		int perimeter_of_contour = arcLength(contours[i], true);

		approxPolyDP(contours[i], contour_shapes[i], 0.02 * perimeter_of_contour, true);
		//bounding_rectangles[i] = boundingRect(contour_shapes);

		//rectangle(image, bounding_rectangles[i].tl(), bounding_rectangles[i].br(), Scalar(0, 255, 0), 5);
	}

	return contour_shapes;
}