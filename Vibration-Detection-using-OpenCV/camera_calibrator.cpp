#include "camera_calibrator.h"

CameraCalibrator::CameraCalibrator(std::string input_file_name, std::string chessboards_path) :
	input_file_name_{ input_file_name },
	chessboards_path_{ chessboards_path }
{
	input_cap_ = new VideoCapture(input_file_name);

	if (!input_cap_->isOpened())
		std::cout << "FUCK FUCK FUCK FUCK FUCK FUCK FUCK FUCK FUCK" << std::endl;

	frame_width_ = input_cap_->get(CAP_PROP_FRAME_WIDTH);
	frame_height_ = input_cap_->get(CAP_PROP_FRAME_HEIGHT);
	bitrate_ = input_cap_->get(CAP_PROP_BITRATE);
	frame_count_ = input_cap_->get(CAP_PROP_FRAME_COUNT);
	fps_ = input_cap_->get(CAP_PROP_FPS);


	// наших углов должен быть 10 и 7 по горизонтали и вертикали соответственно
	pattern_size_ = Size(10, 7);

	txt_file_name_ = "parameters.txt";
}

CameraCalibrator::~CameraCalibrator()
{
	input_cap_->release();
	delete[] input_cap_;
}

// тут кстати пишу щас это после video_undistorter, и так-то тут по-хорошему применить полиморфизм. создать класс-родитель с конструктором, как тут повыше и методами, например, Execute()
// классами-потомками сделать калибратор и андистортер, а методы заоверрайдить (например Execute)
// так как основой их является чтение и сохранение кадров, а также у них есть как минимум один общий метод - Execute(). короче, можно улучшить)
void CameraCalibrator::ExecuteCameraCalibration()
{
	std::cout << "Starting camera calibration..." << std::endl;

	std::cout << "Finding corners..." << std::endl;
	int chessboard_number = 0;

	LoadImages(chessboards_path_);

	while (chessboard_number < chessboards_amount_)
	{
		
		std::cout << "Current frame: " << chessboard_number << "/" << chessboards_amount_ << std::endl;
		
		Mat chessboard = chessboards_[chessboard_number];

		pattern_found_ = FindCorners(chessboard, pattern_size_);

		if (pattern_found_ == true)
		{
			std::cout << "Pattern found" << std::endl;
			vec_of_corners_.push_back(corners_);
		}
		chessboard_number++;
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
	std::cout << "camera_matrix_ is " << camera_matrix_ << std::endl;
	std::cout << "distortion_coefficients_ is " << distortion_coefficients_ << std::endl;

	for (int i = 0; i < rotation_vector_.size(); i++)
	{
		std::cout << "rotation_vector_ is " << rotation_vector_[i] << std::endl;
	}
	for (int i = 0; i < translation_vector_.size(); i++)
	{
		std::cout << "translation_vector_ is " << translation_vector_[i] << std::endl;
	}

	// saving found parameters to a txt file
	SaveFoundParamsToFile(camera_matrix_, distortion_coefficients_, txt_file_name_);
}

void CameraCalibrator::LoadImages(std::string chessboards_path)
{
	Mat chessboard;

	// transform path a little bit
	std::string path = chessboards_path;
	chessboards_amount_ = 0;
	path += "chessboard_"; // location/chessboard_
	path += std::to_string(chessboards_amount_); // location/chessboard_0
	path += ".png"; // location/chessboard_0.png

	// first iter
	chessboard = imread(path);

	while (!chessboard.empty())
	{
		chessboards_.push_back(chessboard);
		chessboards_amount_++;

		// transforming path for reading next image
		path = chessboards_path;
		path += "chessboard_"; // location/chessboard_
		path += std::to_string(chessboards_amount_); // location/chessboard_0
		path += ".png"; // location/chessboard_0.png

		std::cout << "Reading " << chessboards_amount_ << " image" << std::endl;
		chessboard = imread(path);
	}
	std::cout << "Amount of chessboards: " << chessboards_amount_ << std::endl;
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
