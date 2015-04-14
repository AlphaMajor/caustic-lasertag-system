/*
 * RCSP-state-saver.cpp
 *
 *  Created on: 08 апр. 2015 г.
 *      Author: alexey
 */

#include "rcsp/RCSP-state-saver.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "core/scheduler.hpp"
#include "hal/ff/ff.h"

#include <string.h>

StateSaver* StateSaver::m_saver = nullptr;
STATIC_DEINITIALIZER_IN_CPP_FILE(StateSaver, m_saver)

StateSaver& StateSaver::instance()
{
	if (!m_saver)
		m_saver = new StateSaver;
	return *m_saver;
}


void StateSaver::addValue(OperationCode code)
{
	printf("Registering to state saver: %u\n", code);
	m_codes.push_back(code);
}

void StateSaver::setFilename(const std::string& filename)
{
	m_file[0] = filename + "_1.bin";
	m_file[1] = filename + "_2.bin";
	m_fileLock[0] = filename + "_1.lock";
	m_fileLock[1] = filename + "_1.lock";
}

void StateSaver::saveState()
{
	printf("Saving state\n");
	FRESULT res = FR_OK;
	FIL file;
	// Creating lock file
	res = f_open(&file, m_fileLock[m_current].c_str(), FA_CREATE_NEW);
	if (res != FR_OK)
	{
		printf("Cannot create lock file: %d!\n", (int)res);
		return;
	}

	f_close(&file);

	// Deleting old state file
	f_unlink(m_file[m_current].c_str());
	// Creating new state file
	res = f_open(&file, m_file[m_current].c_str(), FA_CREATE_NEW | FA_WRITE);
	if (res != FR_OK)
	{
		printf("Cannot create state file: %d!\n", (int)res);
		return;
	}
	// Putting data to state file
	RCSPMultiStream stream;
	for (OperationCode code : m_codes)
		stream.addValue(code);
	stream.writeToFile(&file);
	f_close(&file);

	// Removing lock file
	f_unlink(m_fileLock[m_current].c_str());

	std::swap(m_current, m_next);
}

bool StateSaver::tryRestore(uint8_t variant)
{
	printf("In state restoring func\n");
	// Check if we have not deleted lock file
	if (f_stat(m_fileLock[variant].c_str(), nullptr) == FR_OK)
	{
		printf("Lock file detected, clearing\n");
		f_unlink(m_fileLock[m_current].c_str());
		f_unlink(m_file[m_current].c_str());
		return false;
	}
	// Opening file
	if (f_stat(m_file[variant].c_str(), nullptr) == FR_OK)
	{
		FRESULT res = FR_OK;
		FIL file;
		res = f_open(&file, m_file[m_current].c_str(), FA_READ);
		if (res != FR_OK)
		{
			printf("Cannot open state file!\n");
			return false;
		}
		uint8_t* buffer = new uint8_t[RCSPStream::defaultLength];
		memset(buffer, 0, RCSPStream::defaultLength);
		UINT readed = 0;

		for(;;) {
			res = f_read(&file, buffer, RCSPStream::defaultLength, &readed);
			if (res == FR_OK && readed != 0)
			{
				printf("Dispatching restored parameters chunk...\n");
				RCSPAggregator::instance().dispatchStream(buffer, readed);
			}
			else
				break;
		}

		f_close(&file);
		return true;
	}
	printf("%s does not exists\n", m_file[variant].c_str());
	return false;
}

bool StateSaver::tryRestore()
{
	return tryRestore(0) ? true : tryRestore(1);
}

void StateSaver::runSaver(uint32_t period)
{
	stopSaver();
	m_savingTask = Scheduler::instance().addTask(std::bind(&StateSaver::saveState, this), false, period, 0, period);
}

void StateSaver::stopSaver()
{
	if (m_savingTask)
		Scheduler::instance().stopTask(m_savingTask);
}
