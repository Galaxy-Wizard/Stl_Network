#include "StdAfx.h"
#include "Wave.h"
#include "math.h"


CWave::CWave(void)
{
	// Init members
	memset(&m_Descriptor, 0, sizeof(_WAVEDESCR));
	memset(&m_Format, 0, sizeof(_WAVEFORMAT));
	m_lpData = NULL;
	m_dwSize = 0;
	m_hWaveout = NULL;
	memset(&m_WaveHeader, 0, sizeof(WAVEHDR));
	m_bPaused = FALSE;
	m_bStopped = TRUE;
}

CWave::~CWave(void)
{
	// Close output device
	Close();
}

BOOL CWave::Open(SHORT channels, DWORD sampleRate, SHORT bitsPerSample)
{
	BOOL bResult = TRUE;

	// Open output device
	SHORT format = WAVE_FORMAT_PCM;
	SHORT blockAlign = channels << 1;
	WAVEFORMATEX wfex;
	wfex.wFormatTag = format;
	wfex.nChannels = channels;
	wfex.nSamplesPerSec = sampleRate;
	wfex.nAvgBytesPerSec = blockAlign * sampleRate;
	wfex.nBlockAlign = blockAlign;
	wfex.wBitsPerSample = bitsPerSample;
	wfex.cbSize = 0;
	if (waveOutOpen(&m_hWaveout, WAVE_MAPPER, &wfex, (DWORD_PTR)&CWave::WaveOut_Proc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	{
		m_hWaveout = NULL;
		bResult = FALSE;
	}
	m_bPaused = FALSE;
	m_bStopped = TRUE;

	return bResult;
}

BOOL CWave::Close()
{
	BOOL bResult = TRUE;

	// Stop playback
	if (Stop())
	{
		// Close output device
		if (waveOutClose(m_hWaveout) != MMSYSERR_NOERROR)
		{
			m_hWaveout = NULL;
			bResult = FALSE;
		}
	}

	// Check for valid sound data
	if (IsValid())
	{
		// Clear sound data buffer
		free(m_lpData);
		m_lpData = NULL;
		m_dwSize = 0;
	}

	return bResult;
}

BOOL CWave::Load(LPTSTR lpszFilePath)
{
	BOOL bResult = FALSE;

	// Close output device
	Close();

	// Load .WAV file
	FILE* file = NULL;
	_tfopen_s(&file, lpszFilePath, _T("rb"));
	if (file != NULL)
	{
		// Read .WAV descriptor
		fread(&m_Descriptor, sizeof(_WAVEDESCR), 1, file);

		// Check for valid .WAV file
		if (strncmp((LPCSTR)m_Descriptor.wave, "WAVE", 4) == 0)
		{
			// Read .WAV format
			fread(&m_Format, sizeof(_WAVEFORMAT), 1, file);

			// Check for valid .WAV file
			if ((strncmp((LPCSTR)m_Format.id, "fmt", 3) == 0) && (m_Format.format == 1))
			{
				// Read next chunk
				BYTE id[4];
				DWORD size;
				fread(id, sizeof(BYTE), 4, file);
				fread(&size, sizeof(DWORD), 1, file);
				DWORD offset = ftell(file);

				// Read .WAV data
				LPBYTE lpTemp = (LPBYTE)malloc(m_Descriptor.size*sizeof(BYTE));
				while (offset < m_Descriptor.size)
				{
					// Check for .WAV data chunk
					if (strncmp((LPCSTR)id, "data", 4) == 0)
					{
						if (m_lpData == NULL)
							m_lpData = (LPBYTE)malloc(size*sizeof(BYTE));
						else
							m_lpData = (LPBYTE)realloc(m_lpData, (m_dwSize+size)*sizeof(BYTE));
						fread(m_lpData+m_dwSize, sizeof(BYTE), size, file);
						m_dwSize += size;
					}
					else
						fread(lpTemp, sizeof(BYTE), size, file);

					// Read next chunk
					fread(id, sizeof(BYTE), 4, file);
					fread(&size, sizeof(DWORD), 1, file);
					offset = ftell(file);
				}
				free(lpTemp);

				// Open output device
				if (!Open(m_Format.channels, m_Format.sampleRate, m_Format.bitsPerSample))
				{
					m_hWaveout = NULL;
					bResult = FALSE;
				}
				else
					bResult = TRUE;
			}
		}

		// Close .WAV file
		fclose(file);
	}

	return bResult;
}

BOOL CWave::Save(LPTSTR lpszFilePath)
{
	BOOL bResult = FALSE;

	// Save .WAV file
	FILE* file = NULL;
	_tfopen_s(&file, lpszFilePath, _T("wb"));
	if (file != NULL)
	{
		// Save .WAV descriptor
		m_Descriptor.riff[0] = 'R';
		m_Descriptor.riff[1] = 'I';
		m_Descriptor.riff[2] = 'F';
		m_Descriptor.riff[3] = 'F';
		m_Descriptor.wave[0] = 'W';
		m_Descriptor.wave[1] = 'A';
		m_Descriptor.wave[2] = 'V';
		m_Descriptor.wave[3] = 'E';
		m_Descriptor.size = sizeof(_WAVEDESCR) + sizeof(_WAVEFORMAT) +  m_dwSize - 8;
		fwrite(&m_Descriptor, sizeof(_WAVEDESCR), 1, file);

		// Save .WAV format
		m_Format.id[0] = 'f';
		m_Format.id[1] = 'm';
		m_Format.id[2] = 't';
		m_Format.id[3] = ' ';
		m_Format.format = WAVE_FORMAT_PCM;
		m_Format.extra_format_bytes = 0;
		m_Format.size = sizeof(_WAVEFORMAT) - 8;
		fwrite(&m_Format, sizeof(_WAVEFORMAT), 1, file);

		// Write .WAV data
		BYTE id[4] = {'d', 'a', 't', 'a'};
		fwrite(id, sizeof(BYTE), 4, file);
		fwrite(&m_dwSize, sizeof(DWORD), 1, file);

		fwrite(m_lpData, sizeof(BYTE), m_dwSize, file);
		bResult = TRUE;

		// Close .WAV file
		fclose(file);
	}

	return bResult;
}

BOOL CWave::Play()
{
	BOOL bResult = TRUE;

	// Check for valid sound data
	if (IsValid() && (m_bStopped || m_bPaused))
	{
		if (m_bPaused)
		{
			// Continue playback
			if (waveOutRestart(m_hWaveout) != MMSYSERR_NOERROR)
				bResult = FALSE;
		}
		else
		{
			// Start playback
			m_WaveHeader.lpData = (LPSTR)m_lpData;
			m_WaveHeader.dwBufferLength = m_dwSize;
			if (waveOutPrepareHeader(m_hWaveout, &m_WaveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
				bResult = FALSE;
			else
			{
				if (waveOutWrite(m_hWaveout, &m_WaveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
					bResult = FALSE;
			}
		}
		m_bPaused = FALSE;
		m_bStopped = FALSE;
	}

	return bResult;
}

BOOL CWave::Stop()
{
	BOOL bResult = TRUE;

	// Check for valid sound data
	if (IsValid() && (!m_bStopped))
	{
		// Stop playback
		m_bStopped = TRUE;
		if (waveOutReset(m_hWaveout) != MMSYSERR_NOERROR)
			bResult = FALSE;
		else
		{
			if (waveOutUnprepareHeader(m_hWaveout, &m_WaveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
				bResult = FALSE;
		}
	}

	return bResult;
}

BOOL CWave::Pause()
{
	BOOL bResult = TRUE;

	// Check for valid sound data
	if (IsValid() && (!m_bPaused) && (!m_bStopped))
	{
		// Pause playback
		m_bPaused = TRUE;
		if (waveOutPause(m_hWaveout) != MMSYSERR_NOERROR)
			bResult = FALSE;
	}

	return bResult;
}

BOOL CWave::Mix(CWave& wave)
{
	BOOL bResult = FALSE;

	// Check for valid sound data
	if ((IsValid() && m_bStopped) && (wave.IsValid() && wave.IsStopped()))
	{
		// Check for valid sound format
		if ((m_Format.channels == wave.GetChannels()) && (m_Format.sampleRate == wave.GetSampleRate()) && (m_Format.bitsPerSample == wave.GetBitsPerSample()))
		{
			// Mix .WAVs
			long sampleSize = min(m_dwSize, wave.GetSize()) / (m_Format.bitsPerSample >> 3);
			switch (m_Format.bitsPerSample)
			{
				case 8:
					{
						LPBYTE lpSrcData = wave.GetData();
						LPBYTE lpDstData = m_lpData;
						float gain = log10(20.0f);
						for (long i=0; i<sampleSize; i++)
						{
							*lpDstData = (BYTE)(((*lpSrcData+*lpDstData)>>1)*gain);
							lpSrcData++;
							lpDstData++;
						}
					}
					break;

				case 16:
					{
						LPWORD lpSrcData = (LPWORD)wave.GetData();
						LPWORD lpDstData = (LPWORD)m_lpData;
						for (long i=0; i<sampleSize; i++)
						{
							float sample1 = (*lpSrcData - 32768) / 32768.0f;
							float sample2 = (*lpDstData - 32768) / 32768.0f;
							if (fabs(sample1*sample2) > 0.25f)
								*lpDstData = (WORD)(*lpSrcData + *lpDstData);
							else
								*lpDstData = fabs(sample1) < fabs(sample2) ? *lpSrcData : *lpDstData;
							lpSrcData++;
							lpDstData++;
						}
					}
					break;
			}
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL CWave::WaveOut_Proc(HWAVEOUT hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	BOOL bResult = TRUE;

	// Get current object
	CWave* pWave = (CWave*)((DWORD_PTR)&dwInstance);

	// Check for playback finished
	if ((uMsg == WOM_DONE) && ((!pWave->m_bStopped) || (!pWave->m_bPaused)))
	{
		// Relase sound info
		if (waveOutUnprepareHeader(pWave->m_hWaveout, &pWave->m_WaveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			bResult = FALSE;
	}

	return bResult;
}

BOOL CWave::Load(IStream *local_stream)
{
	BOOL bResult = FALSE;
	ULONG cdRead = 0;
	ULONG cdReadCounter = 0;

	// Close output device
	Close();

	// Load .WAV file
	if (local_stream != NULL)
	{
		// Read .WAV descriptor
		local_stream->Read(&m_Descriptor, sizeof(_WAVEDESCR)* 1, &cdRead);
		cdReadCounter += cdRead;

		// Check for valid .WAV file
		if (strncmp((LPCSTR)m_Descriptor.wave, "WAVE", 4) == 0)
		{
			// Read .WAV format
			local_stream->Read(&m_Format, sizeof(_WAVEFORMAT)* 1, &cdRead);
			cdReadCounter += cdRead;

			// Check for valid .WAV file
			if ((strncmp((LPCSTR)m_Format.id, "fmt", 3) == 0) && (m_Format.format == 1))
			{
				// Read next chunk
				BYTE id[4];
				DWORD size;
				local_stream->Read(id, sizeof(BYTE)* 4,& cdRead);
				cdReadCounter += cdRead;
				local_stream->Read(&size, sizeof(DWORD)* 1, &cdRead);
				cdReadCounter += cdRead;
				DWORD offset = cdReadCounter;

				// Read .WAV data
				LPBYTE lpTemp = (LPBYTE)malloc(m_Descriptor.size*sizeof(BYTE));
				while (offset < m_Descriptor.size)
				{
					// Check for .WAV data chunk
					if (strncmp((LPCSTR)id, "data", 4) == 0)
					{
						if (m_lpData == NULL)
							m_lpData = (LPBYTE)malloc(size*sizeof(BYTE));
						else
							m_lpData = (LPBYTE)realloc(m_lpData, (m_dwSize+size)*sizeof(BYTE));
						local_stream->Read(m_lpData+m_dwSize, sizeof(BYTE)* size, &cdRead);
						cdReadCounter += cdRead;
						m_dwSize += size;
					}
					else
					{
						local_stream->Read(lpTemp, sizeof(BYTE)* size, &cdRead);
						cdReadCounter += cdRead;
					}

					// Read next chunk
					local_stream->Read(id, sizeof(BYTE)* 4, &cdRead);
					cdReadCounter += cdRead;
					local_stream->Read(&size, sizeof(DWORD)* 1, &cdRead);
					cdReadCounter += cdRead;
					offset = cdReadCounter;
				}
				free(lpTemp);

				// Open output device
				if (!Open(m_Format.channels, m_Format.sampleRate, m_Format.bitsPerSample))
				{
					m_hWaveout = NULL;
					bResult = FALSE;
				}
				else
					bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL CWave::Save(IStream *local_stream)
{
	BOOL bResult = FALSE;
	ULONG cdWritten = 0;

	// Save .WAV stream
	if (local_stream != NULL)
	{
		// Save .WAV descriptor
		m_Descriptor.riff[0] = 'R';
		m_Descriptor.riff[1] = 'I';
		m_Descriptor.riff[2] = 'F';
		m_Descriptor.riff[3] = 'F';
		m_Descriptor.wave[0] = 'W';
		m_Descriptor.wave[1] = 'A';
		m_Descriptor.wave[2] = 'V';
		m_Descriptor.wave[3] = 'E';
		m_Descriptor.size = sizeof(_WAVEDESCR) + sizeof(_WAVEFORMAT) +  m_dwSize - 8;
		local_stream->Write(&m_Descriptor, sizeof(_WAVEDESCR)* 1, &cdWritten);

		// Save .WAV format
		m_Format.id[0] = 'f';
		m_Format.id[1] = 'm';
		m_Format.id[2] = 't';
		m_Format.id[3] = ' ';
		m_Format.format = WAVE_FORMAT_PCM;
		m_Format.extra_format_bytes = 0;
		m_Format.size = sizeof(_WAVEFORMAT) - 8;
		local_stream->Write(&m_Format, sizeof(_WAVEFORMAT)* 1, &cdWritten);

		// Write .WAV data
		BYTE id[4] = {'d', 'a', 't', 'a'};
		local_stream->Write(id, sizeof(BYTE)* 4, &cdWritten);
		local_stream->Write(&m_dwSize, sizeof(DWORD)* 1, &cdWritten);

		local_stream->Write(m_lpData, sizeof(BYTE)* m_dwSize, &cdWritten);
		bResult = TRUE;
	}

	return bResult;
}