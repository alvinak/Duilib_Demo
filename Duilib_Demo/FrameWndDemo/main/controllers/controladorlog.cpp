/*
 *  
 *  $Id: controladorlog.cpp $
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
//#include <api/ientorno.h>
//#include <api/internationalization/internationalization.h>
#include <dcmtk/oflog/oflog.h>
#include <dcmtk/oflog/fileap.h>
#include <dcmtk/oflog/helpers/loglog.h>
#include <dcmtk/oflog/configrt.h>
#include "log4cplus/guiappender.h"
//#include "configurationcontroller.h"
#include "controladorlog.h"

#include "utility_tool/common_utils.h"
#include "controller/configcontroller.h"


#define L4C_LITERAL(lit) (lit)


#define LOGGER_DEFECTO "default"

namespace GNC {
	//region "Patron singleton"
	GNC::ControladorLog* GNC::ControladorLog::m_pInstance = 0;

	GNC::ControladorLog::ControladorLog()
	{
		//en debug siempre se usa el appender fichero + el consola, ignoramos los properties
		//esto es porque si no esta el flush immediatly en windows cuando se cierra inesperadamente queda colgada
		//una consola
//#ifndef _GINKGO_DEBUG 
//		bool existeProperties=false;
//		std::fstream fin;
//		fin.open("ginkgo.properties",std::ios::in);
//		if( fin.is_open() )
//		{
//			existeProperties = true;
//		}
//		fin.close();
//
//		if(existeProperties){
//			log4cplus::PropertyConfigurator::doConfigure(L4C_LITERAL("ginkgo.properties"));
//			log4cplus::SharedAppenderPtr append_0(gnkLog4cplus::GUIAppender::Instance());
//			append_0->setName(L4C_LITERAL("AppenderGUI"));
//			append_0->setLayout( OFauto_ptr<log4cplus::Layout>(new log4cplus::TTCCLayout()) );
//			log4cplus::Logger::getRoot().addAppender(append_0);
//
//		} else {
//#endif
			//instanciamos el log
			std::ostringstream ostr;
			//ostr << GNC::GCS::IEntorno::Instance()->GetGinkgoUserDir().c_str() << "\\Log\\";
			ostr <<  ".\\Log\\";

			TryCreateDir(ostr.str());
            //if(!wxDirExists(ostr.str())){
            //    bool bRet = wxMkdir(ostr.str(), 511);
            //    if (!bRet)
            //    {
            //        LOG_ERROR("wxMkdir", ostr.str());
            //    }
            //}
            
			ostr << "PacsDownload.log";
			log4cplus::SharedAppenderPtr append_0(gnkLog4cplus::GUIAppender::Instance());
			append_0->setName(LOG4CPLUS_TEXT("AppenderGUI"));
			append_0->setLayout( OFauto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%D{%Y/%m/%d %H:%M:%S.%q} [%t] %-5p %c %x - %m%n")) );
			log4cplus::Logger::getRoot().addAppender(append_0);
			
			OFString tmpStr(ostr.str().c_str());
			log4cplus::SharedAppenderPtr append_1(new log4cplus::RollingFileAppender(tmpStr, 5*1024*1024, 100, true));
			append_1->setName(LOG4CPLUS_TEXT("AppenderFichero"));
			append_1->setLayout( OFauto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%D{%Y/%m/%d %H:%M:%S.%q} [%t] %-5p %c %x - %m%n")) );
			log4cplus::Logger::getRoot().addAppender(append_1);
			
			int logLevel;
			#ifdef _DEBUG
			log4cplus::Logger::getRoot().setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
			//to avoid verbosivity of dcmdata
			log4cplus::Logger::getInstance(L4C_LITERAL("dcmtk.dcmdata")).setLogLevel(log4cplus::INFO_LOG_LEVEL);

			logLevel = ConfigController::Instance()->GetLogLevel();
			//GNC::GCS::ConfigurationController::Instance()->readIntGeneral("/DeepWiseCore/Log", "LogLevel", logLevel, log4cplus::DEBUG_LOG_LEVEL);
			#else
			log4cplus::Logger::getRoot().setLogLevel(log4cplus::INFO_LOG_LEVEL);
			logLevel = ConfigController::Instance()->GetLogLevel();
			//GNC::GCS::ConfigurationController::Instance()->readIntGeneral("/DeepWiseCore/Log", "LogLevel", logLevel, log4cplus::INFO_LOG_LEVEL);
			#endif
			log4cplus::Logger::getRoot().setLogLevel(logLevel);

//#ifndef _GINKGO_DEBUG
//		}
//#endif
	}

	GNC::ControladorLog::~ControladorLog()
	{
		log4cplus::Logger::getRoot().closeNestedAppenders();
		log4cplus::Logger::getRoot().removeAllAppenders();
		log4cplus::Logger::getRoot().shutdown();
		gnkLog4cplus::GUIAppender::FreeInstance();
		
	}
	
	void GNC::ControladorLog::GetLogLevelsMap(GNC::GCS::IControladorLog::MapaLogLevels& levels)
	{			
		levels[log4cplus::FATAL_LOG_LEVEL] = "FATAL";
		levels[log4cplus::ERROR_LOG_LEVEL] = "ERROR";
		levels[log4cplus::WARN_LOG_LEVEL] = "WARN";
		levels[log4cplus::INFO_LOG_LEVEL] = "INFO";
		levels[log4cplus::DEBUG_LOG_LEVEL] = "DEBUG";
		levels[log4cplus::TRACE_LOG_LEVEL] = "TRACE";
	}
	
	int GNC::ControladorLog::GetLogLevelCode(const std::string& level)
	{
		if (level == "FATAL") {
			return log4cplus::FATAL_LOG_LEVEL;
		}
		else if (level == "ERROR") {
			return log4cplus::ERROR_LOG_LEVEL;
		}
		else if (level == "WARN") {
			return log4cplus::WARN_LOG_LEVEL;
		}
		else if (level == "INFO") {
			return log4cplus::INFO_LOG_LEVEL;
		}
		else if (level == "DEBUG") {
			return log4cplus::DEBUG_LOG_LEVEL;
		}
		else {
			return log4cplus::TRACE_LOG_LEVEL;
		}
	}
	
	int GNC::ControladorLog::GetActiveLogLevel()
	{
		return log4cplus::Logger::getRoot().getLogLevel();
	}
	
	void GNC::ControladorLog::SetActiveLogLevel(int ll)
	{

		log4cplus::Logger::getRoot().setLogLevel(ll);
	}

	
	GNC::ControladorLog* GNC::ControladorLog::Instance()
	{
		if (m_pInstance == NULL) {
			m_pInstance = new GNC::ControladorLog();
		}
		return m_pInstance;
	}

	void GNC::ControladorLog::FreeInstance()
	{
		if (m_pInstance != NULL) {
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	inline void LogearInline(log4cplus::Logger& log,const std::string& mensaje, GNC::ControladorLog::LogLevel logLevel) 
	{
        string strLog = mensaje;
        //wxString tryUtf8 = wxString::FromUTF8(strLog.c_str());
        //if (tryUtf8.size() > 0)
        //{
        //    strLog = tryUtf8.ToStdString();
        //}
		switch(logLevel) {
			case GNC::ControladorLog::FatalLog:
				LOG4CPLUS_FATAL(log, strLog.c_str());
				break;
			case GNC::ControladorLog::ErrorLog:
				LOG4CPLUS_ERROR(log, strLog.c_str());
				break;
			case GNC::ControladorLog::WarnLog:
				LOG4CPLUS_WARN(log, strLog.c_str());
				break;
			case GNC::ControladorLog::InfoLog:
				LOG4CPLUS_INFO(log, strLog.c_str());
				break;
			case GNC::ControladorLog::DebugLog:
				LOG4CPLUS_DEBUG(log, strLog.c_str());
				break;
			case GNC::ControladorLog::TraceLog:
				LOG4CPLUS_TRACE(log, strLog.c_str());
				break;
			case GNC::ControladorLog::NoLog:
				break;
		}
	}

	void GNC::ControladorLog::Log(const std::string& mensaje, GNC::ControladorLog::LogLevel logLevel)
	{
		log4cplus::Logger log = log4cplus::Logger::getInstance(L4C_LITERAL(LOGGER_DEFECTO));
		LogearInline(log,mensaje,logLevel);
	}

	void GNC::ControladorLog::Log(const std::string& logger, const std::string& mensaje, GNC::ControladorLog::LogLevel logLevel)
	{
		log4cplus::Logger log = log4cplus::Logger::getInstance(logger.c_str());
		LogearInline(log, mensaje, logLevel);
	}

	void GNC::ControladorLog::LogConContexto(const std::string& contexto, const std::string& mensaje, GNC::ControladorLog::LogLevel logLevel)
	{
		log4cplus::Logger log = log4cplus::Logger::getInstance(L4C_LITERAL(LOGGER_DEFECTO));
		log4cplus::NDCContextCreator _context(contexto.c_str());
		LogearInline(log, mensaje, logLevel);
	}

	void GNC::ControladorLog::LogConContexto(const std::string& logger, const std::string& contexto, const std::string& mensaje, GNC::ControladorLog::LogLevel logLevel)
	{
		log4cplus::Logger log = log4cplus::Logger::getInstance(logger.c_str());
		log4cplus::NDCContextCreator _context(contexto.c_str());
		LogearInline(log,mensaje,logLevel);
	}
	
	bool GNC::ControladorLog::IsEnabledFor(GNC::GCS::IControladorLog::LogLevel logLevel)
	{
		switch(logLevel) {
			case GNC::ControladorLog::FatalLog:
				return log4cplus::Logger::getRoot().isEnabledFor(log4cplus::FATAL_LOG_LEVEL);
			case GNC::ControladorLog::ErrorLog:
				return log4cplus::Logger::getRoot().isEnabledFor(log4cplus::ERROR_LOG_LEVEL);
			case GNC::ControladorLog::WarnLog:
				return log4cplus::Logger::getRoot().isEnabledFor(log4cplus::WARN_LOG_LEVEL);
			case GNC::ControladorLog::InfoLog:
				return log4cplus::Logger::getRoot().isEnabledFor(log4cplus::INFO_LOG_LEVEL);
			case GNC::ControladorLog::DebugLog:
				return log4cplus::Logger::getRoot().isEnabledFor(log4cplus::DEBUG_LOG_LEVEL);
			case GNC::ControladorLog::TraceLog:
				return log4cplus::Logger::getRoot().isEnabledFor(log4cplus::TRACE_LOG_LEVEL);
			case GNC::ControladorLog::NoLog:
				return false;
		}
		return false;
	}
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
