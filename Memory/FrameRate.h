#pragma once
#include <chrono>

class FrameRate
{
private:
	std::chrono::system_clock::time_point m_BaseTime;
	std::chrono::system_clock::time_point m_PreTime;
	std::chrono::microseconds m_OneFrameTime;
	std::chrono::microseconds m_MaxOneFrameTime;
	int m_CountFrameNum;
	double m_FrameRate;

public:
	void StartMeasureTime(void);
	void IncrFrame(void);
	double GetFrameRate(void) const;
	long long GetOneFrameTime(void) const;
	long long GetMaxOneFrameTime(void) const;

};

