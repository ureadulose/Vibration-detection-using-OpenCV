#include "vibration_detector.h"

void ExecuteVibrationMonitoring()
{
	VibrationDetector b;
	b.ExecuteVibrationDetection();
}

void main()
{
	ExecuteVibrationMonitoring();
}