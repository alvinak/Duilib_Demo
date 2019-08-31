/*
 *  
 *  $Id: guiappender.cpp $
 *  DeepWise Dicom Viewer
 *
 *  Copyright 2017-2030 DeepWise All rights reserved.
 *  http://www.deepwise.com
 *
 *  This file is licensed under LGPL v3 license.
 *  See License.txt for details
 *
 *
 */
#include <dcmtk/oflog/oflog.h>
#include <dcmtk/oflog/helpers/loglog.h>
#include <dcmtk/oflog/helpers/timehelp.h>
//#include <dcmtk/oflog/stringhelper.h>
//#include <dcmtk/oflog/spi/loggingevent.h>
/*
#include <log4cplus/layout.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/streams.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/spi/loggingevent.h>
*/


#include "guiappender.h"
//#include <api/internationalization/internationalization.h>

#define MAX_LOG_BUFFER_SIZE 30

using namespace log4cplus::helpers;

gnkLog4cplus::GUIAppender* gnkLog4cplus::GUIAppender::m_pInstancia = NULL;



gnkLog4cplus::GUIAppender* gnkLog4cplus::GUIAppender::Instance() {
	if (m_pInstancia == NULL) {
		m_pInstancia = new GUIAppender();
	}
	return m_pInstancia;
}

void gnkLog4cplus::GUIAppender::FreeInstance() {
	if (m_pInstancia != NULL) {
		delete m_pInstancia;
		m_pInstancia = NULL;
	}	
}

gnkLog4cplus::GUIAppender::GUIAppender(bool logToStdErr_, bool immediateFlush_) :
	logToStdErr(logToStdErr_), immediateFlush(immediateFlush_), llmCache(log4cplus::getLogLevelManager()), m_pLogger(NULL)

{
	Init();
	
}

gnkLog4cplus::GUIAppender::GUIAppender(const log4cplus::helpers::Properties properties) :
	Appender(properties), logToStdErr(false), immediateFlush(false), llmCache(log4cplus::getLogLevelManager()), m_Logs(), m_pLogger(NULL)
{
	Init();
}

gnkLog4cplus::GUIAppender::GUIAppender(const GUIAppender& o) : llmCache(o.llmCache), immediateFlush(o.immediateFlush), logToStdErr(o.logToStdErr), m_Logs(o.m_Logs), m_pLogger(o.m_pLogger)
{
	Init();
}

void gnkLog4cplus::GUIAppender::Init() {
	log4cplus::tostringstream dateTimeFormatStream;
	//dateTimeFormatStream << _Std("%Y-%m-%d %H:%M:%S.%q").c_str();
	dateTimeFormatStream << "%Y-%m-%d %H:%M:%S.%q";

	datetimeFormat = dateTimeFormatStream.str().c_str();
}

gnkLog4cplus::GUIAppender::~GUIAppender()
{
	destructorImpl();
	m_pInstancia = NULL;
}


void gnkLog4cplus::GUIAppender::close()
{
	LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( getLogLog().mutex )
    closed = true;
	m_pLogger = NULL;
	m_Logs.clear();
	LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}

void gnkLog4cplus::GUIAppender::clear()
{
	LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( getLogLog().mutex )
    m_Logs.clear();
	LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}

void gnkLog4cplus::GUIAppender::Attach(GNC::GCS::Logging::ILogger* pLogger)
{
	if (this->closed) {
		return;
	}
	LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( getLogLog().mutex )
	m_pLogger = pLogger;
	if (pLogger != NULL) {
		for (ListaLogs::const_iterator it = m_Logs.begin(); it != m_Logs.end(); ++it) {
			pLogger->Append(*it);
		}
	}
	LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}

void gnkLog4cplus::GUIAppender::append(const log4cplus::spi::InternalLoggingEvent& e)
{
	if (this->closed) {
		return;
	}
	
	LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( getLogLog().mutex )
	log4cplus::tostringstream formattedLog;
	layout->formatAndAppend(formattedLog, e);
	
	

	m_Logs.push_back(
		GNC::GCS::Logging::LogEvent(
			e.getLoggerName().c_str(),
			llmCache.toString(e.getLogLevel()).c_str(),
			e.getTimestamp().getFormattedTime(datetimeFormat.c_str(), false).c_str(),
			e.getMessage().c_str(),
			formattedLog.str().c_str(),
			e.getThread().c_str()
		)
	);
	
	if (m_pLogger != NULL) {
		m_pLogger->Append(m_Logs.back());
	}

	if (m_Logs.size() > MAX_LOG_BUFFER_SIZE) {
		m_Logs.pop_front();
	}
	
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}
