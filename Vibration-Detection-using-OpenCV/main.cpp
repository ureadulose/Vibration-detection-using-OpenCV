#include "vibration_detector.h"
#include "video_undistorter.h"
#include "camera_calibrator.h"
#include "amplitude_calibrator.h"

#define CALIBRATION 0
#define UNDISTORTION 1
#define VIBRMON 2
#define AMPLCALIBMODE 3


void ExecuteCameraCalibration(std::string input_file_name, std::string chessboards_path)
{
	CameraCalibrator camera_calibrator(input_file_name, chessboards_path);
	camera_calibrator.ExecuteCameraCalibration();
}

void ExecuteVideoUndistortion(std::string input_file_name, std::string output_file_name)
{
	std::string params;

	std::cout << "Please attach the parameters.txt file: ";
	std::cin >> params;
	std::cout << std::endl;

	VideoUndistorter video_undistorter(input_file_name, output_file_name, params);
	video_undistorter.ExecuteVideoUndistortion();
}

void ExecuteVibrationMonitoring(std::string input_file_name, std::string output_file_name)
{
	VibrationDetector vibration_detector(input_file_name, output_file_name, MAIN_WINDOW_NAME);
	vibration_detector.ExecuteVibrationDetection();
	std::cout << "done" << std::endl;
}

void ExecuteAmplitudeCalibration(std::string input_file_name, std::string output_file_name)
{
	AmplitudeCalibrator amplitude_calibrator;
}

int Execute(std::string txt_file_name, int type)
{
	std::ifstream params_file;
	params_file.open(txt_file_name);

	if (!params_file.is_open())
	{
		std::cout << "Wrong filename" << std::endl;
		return 0;
	}

	std::string input_calibration_video;
	std::string input_distorted_video;
	std::string output_undistorted_video;
	std::string input_vibration_detection;
	std::string output_vibration_detection;
	std::string chessboards_path;
	std::string input_ampl_calib_video;
	std::string output_ampl_calib_file;

	std::getline(params_file, input_calibration_video);
	std::getline(params_file, input_distorted_video);
	std::getline(params_file, output_undistorted_video);
	std::getline(params_file, input_vibration_detection);
	std::getline(params_file, output_vibration_detection);
	std::getline(params_file, chessboards_path);
	std::getline(params_file, input_ampl_calib_video);
	std::getline(params_file, output_ampl_calib_file);

	// calls of modules
	switch (type)
	{
	case CALIBRATION:
		std::cout << "u've chosen camera calibration" << std::endl;
		ExecuteCameraCalibration(input_calibration_video, chessboards_path);
		break;
	case UNDISTORTION:
		std::cout << "u've chosen undistortion" << std::endl;
		ExecuteVideoUndistortion(input_distorted_video, output_undistorted_video);
		break;
	case VIBRMON:
		std::cout << "u've chosen vibration monitoring" << std::endl;
		ExecuteVibrationMonitoring(input_vibration_detection, output_vibration_detection);
		break;
	case AMPLCALIBMODE:
		std::cout << "You have amplitude calibration mode" << std::endl;
		ExecuteAmplitudeCalibration(input_ampl_calib_video, output_ampl_calib_file);
		break;
	}
	return 1;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Not enough arguments\nAttach a .txt" << std::endl;
		return 0;
	}

	std::string program_name;
	std::string txt_file_name;
	std::string type;

	if (argc > 1)
	{
		program_name = argv[0];
		txt_file_name = argv[1];
		type = argv[2];
	}
	int code = std::stoi(type);

	std::cout << "\nProgram " << program_name << " started\n" << std::endl;

	return Execute(txt_file_name, code);
}