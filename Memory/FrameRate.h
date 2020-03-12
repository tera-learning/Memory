#pragma once
#include <chrono>

class FrameRate
{
private:
	std::chrono::system_clock::time_point m_BaseTime;
	int m_CountFrameNum;
	double m_FrameRate;

public:
	void StartMeasureTime(void);
	void IncrFrame(void);
	double GetFrameRate() const;
};

