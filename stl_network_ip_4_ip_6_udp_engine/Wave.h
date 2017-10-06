#pragma once


#include "Mmsystem.h"

#pragma comment(lib, "Winmm.lib")


#pragma pack(1)
typedef struct __WAVEDESCR
{
	BYTE riff[4];
	DWORD size;
	BYTE wave[4];

} _WAVEDESCR, *_LPWAVEDESCR;

typedef struct __WAVEFORMAT
{
	BYTE id[4];
	DWORD size;
	SHORT format;
	SHORT channels;
	DWORD sampleRate;
	DWORD byteRate;
	SHORT blockAlign;
	SHORT bitsPerSample;
	WORD extra_format_bytes;

} _WAVEFORMAT, *_LPWAVEFORMAT;
#pragma pack()


class CWave
{
public:
	CWave(void);
	virtual ~CWave(void);

public:
	// Public methods
	BOOL Load(LPTSTR lpszFilePath);
	BOOL Save(LPTSTR lpszFilePath);
	BOOL Play();
	BOOL Stop();
	BOOL Pause();
	BOOL Mix(CWave& wave);
	BOOL IsValid()				{return (m_lpData != NULL);}
	BOOL IsPlaying()			{return (!m_bStopped && !m_bPaused);}
	BOOL IsStopped()			{return m_bStopped;}
	BOOL IsPaused()				{return m_bPaused;}
	LPBYTE GetData()			{return m_lpData;}
	DWORD GetSize()				{return m_dwSize;}
	SHORT GetChannels()			{return m_Format.channels;}
	DWORD GetSampleRate()		{return m_Format.sampleRate;}
	SHORT GetBitsPerSample()	{return m_Format.bitsPerSample;}

	void SetData(LPBYTE parameter, DWORD size)
	{
		if(m_lpData!=NULL)
		{
			AllocateData(size);
		}
		if(parameter!=NULL)
		{
			if(size>m_dwSize)
			{
				AllocateData(size-m_dwSize);
			}
			if(m_lpData!=NULL)
			{
				memcpy(m_lpData, parameter,size);
			}
		}
	}
	void SetSize(DWORD parameter)			{m_Format.size = parameter;}
	void SetChannels(SHORT parameter)		{m_Format.channels = parameter;m_Format.blockAlign = parameter << 1;}
	void SetSampleRate(DWORD parameter)		{m_Format.sampleRate = parameter;}
	void SetBitsPerSample(SHORT parameter)	{m_Format.bitsPerSample = parameter;}
	void SetByteRate(DWORD parameter)		{m_Format.byteRate = parameter;}

	BOOL Load(IStream *local_stream);
	BOOL Save(IStream *local_stream);


private:
	// Private methods
	BOOL Open(SHORT channels, DWORD sampleRate, SHORT bitsPerSample);
	BOOL Close();
	BOOL static CALLBACK WaveOut_Proc(HWAVEOUT hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

	void AllocateData(DWORD size)
	{
		if (m_lpData == NULL)
		{
			m_lpData = (LPBYTE)malloc(size*sizeof(BYTE));
			m_dwSize = size;
		}
		else
		{
			m_lpData = (LPBYTE)realloc(m_lpData, (m_dwSize+size)*sizeof(BYTE));
			m_dwSize += size;
		}
	}

private:
	// Private members
	_WAVEDESCR m_Descriptor;
	_WAVEFORMAT m_Format;
	LPBYTE m_lpData;
	DWORD m_dwSize;
	HWAVEOUT m_hWaveout;
	WAVEHDR m_WaveHeader;
	BOOL m_bStopped;
	BOOL m_bPaused;
};
