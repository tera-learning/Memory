#include "FrameRate.h"
#include <chrono>

void FrameRate::StartMeasureTime(void)
{
	m_CountFrameNum = 0;
	m_FrameRate = 0.0f;
	m_BaseTime = std::chrono::system_clock::now();
}

void FrameRate::IncrFrame(void)
{
	// ���ݎ������擾
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	// ������Ƃ̍������擾
	std::chrono::milliseconds startTime = std::chrono::duration_cast<std::chrono::milliseconds>(m_BaseTime.time_since_epoch());
	std::chrono::milliseconds endTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
	std::chrono::milliseconds diff = endTime - startTime;
	double duration = std::chrono::duration<double>(diff).count();
	// �t���[�����C���N�������g	
	m_CountFrameNum++;
	// 1�b�ȏ�o�߂��Ă�����FrameRae�X�V	
	if (duration >= 1.0f)
	{
		m_FrameRate = static_cast<float>(m_CountFrameNum) / duration;
		m_BaseTime = std::chrono::system_clock::now();
		m_CountFrameNum = 0;
	}
}

double FrameRate::GetFrameRate(void) const
{
	return m_FrameRate;
}
