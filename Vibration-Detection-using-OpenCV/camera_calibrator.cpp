#include "camera_calibrator.h"

CameraCalibrator::CameraCalibrator(std::string input_file_name) :
	input_file_name_{ input_file_name }
{
	input_cap_ = new VideoCapture(input_file_name);

	if (!input_cap_->isOpened())
		std::cout << "FUCK FUCK FUCK FUCK FUCK FUCK FUCK FUCK FUCK" << std::endl;

	frame_width_ = input_cap_->get(CAP_PROP_FRAME_WIDTH);
	frame_height_ = input_cap_->get(CAP_PROP_FRAME_HEIGHT);
	bitrate_ = input_cap_->get(CAP_PROP_BITRATE);
	frame_count_ = input_cap_->get(CAP_PROP_FRAME_COUNT);
	fps_ = input_cap_->get(CAP_PROP_FPS);


	// ����� ����� ������ ���� 10 � 7 �� ����������� � ��������� ��������������
	pattern_size_ = Size(10, 7);

	txt_file_name_ = "parameters.txt";
}

CameraCalibrator::~CameraCalibrator()
{
	input_cap_->release();
	delete[] input_cap_;
}

// ��� ������ ���� ��� ��� ����� video_undistorter, � ���-�� ��� ��-�������� ��������� �����������. ������� �����-�������� � �������������, ��� ��� ������ � ��������, ��������, Execute()
// ��������-��������� ������� ���������� � �����������, � ������ ������������� (�������� Execute)
// ��� ��� ������� �� �������� ������ � ���������� ������, � ����� � ��� ���� ��� ������� ���� ����� ����� - Execute(). ������, ����� ��������)
void CameraCalibrator::ExecuteCameraCalibration()
{
	std::cout << "Starting camera calibration..." << std::endl;

	std::cout << "Finding corners..." << std::endl;
	int frame_number = 1;

	while (frame_number < frame_count_)
	{
		frame_number++;
		std::cout << "Current frame: " << frame_number << "/" << frame_count_ << std::endl;
		input_cap_->read(current_frame_);
		pattern_found_ = FindCorners(current_frame_, pattern_size_);

		if (pattern_found_ == true)
		{
			std::cout << "Pattern found" << std::endl;
			vec_of_corners_.push_back(corners_);
		}
	}
	///////// creating coordinate plain //////////
	std::vector<Point3f> object_points;
	for (int i = 0; i < pattern_size_.height; i++)
	{
		for (int j = 0; j < pattern_size_.width; j++)
		{
			object_points.push_back(Point3f(i, j, 0));
		}
	}
	std::vector<std::vector<Point3f>> vec_of_object_points(vec_of_corners_.size());
	for (int i = 0; i < vec_of_corners_.size(); i++)
	{
		vec_of_object_points[i] = object_points;
	}
	/////////////////////////////////////////////

	std::cout << "Camera calibration..." << std::endl;
	float error = calibrateCamera
	(
		vec_of_object_points,
		vec_of_corners_,
		Size(frame_height_, frame_width_),
		camera_matrix_,
		distortion_coefficients_,
		rotation_vector_,
		translation_vector_
	);
	std::cout << "Error is " << error << std::endl;

	// saving found parameters to a txt file
	SaveFoundParamsToFile(camera_matrix_, distortion_coefficients_, txt_file_name_);
}

bool CameraCalibrator::FindCorners(Mat& input_frame, Size pattern_size)
{
	Mat gray_frame;
	cvtColor(input_frame, gray_frame, COLOR_BGR2GRAY);

	bool pattern_found = findChessboardCorners
	(
		gray_frame,
		pattern_size,
		corners_,
		CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK
	);

	if (pattern_found)
		cornerSubPix
		(
			gray_frame, corners_,
			Size(11, 11), // window size
			Size(-1, -1), // zero zone
			TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 40, 0.001)
		);

	return pattern_found;
}

void CameraCalibrator::SaveFoundParamsToFile(Mat camera_matrix, Mat dist_coeffs_2be_written, std::string file)
{
	fx_ = camera_matrix.at<double>(0, 0);
	fy_ = camera_matrix.at<double>(1, 1);
	px_ = camera_matrix.at<double>(0, 2);
	py_ = camera_matrix.at<double>(1, 2);

	std::vector<double> distortion_coefficients;
	distortion_coefficients =
	{
		dist_coeffs_2be_written.at<double>(0,0),
		dist_coeffs_2be_written.at<double>(0,1),
		dist_coeffs_2be_written.at<double>(0,2),
		dist_coeffs_2be_written.at<double>(0,3),
		dist_coeffs_2be_written.at<double>(0,4)
	};

	outfile_.open(file, std::ios::out | std::ios::trunc);

	outfile_ << "fx:" + ToStringWithPrecision(fx_) << std::endl;
	outfile_ << "fy:" + ToStringWithPrecision(fy_) << std::endl;
	outfile_ << "px:" + ToStringWithPrecision(px_) << std::endl;
	outfile_ << "py:" + ToStringWithPrecision(py_) << std::endl;
	outfile_ << "dist:" + ToStringWithPrecision(distortion_coefficients[0]) + "," + ToStringWithPrecision(distortion_coefficients[1])
		+ "," + ToStringWithPrecision(distortion_coefficients[2]) + "," + ToStringWithPrecision(distortion_coefficients[3]) + "," + ToStringWithPrecision(distortion_coefficients[4]);

	outfile_.close();
}

template <typename T>
std::string CameraCalibrator::ToStringWithPrecision(const T a_value, const int n)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}
