#include "vibration_detector.h"

void ExecuteVibrationMonitoring()
{
	VibrationDetector b(INPUT_FILE_NAME, OUTPUT_FILE_NAME, MAIN_WINDOW_NAME);
	b.ExecuteVibrationDetection();
}

void main()
{
	ExecuteVibrationMonitoring();
}