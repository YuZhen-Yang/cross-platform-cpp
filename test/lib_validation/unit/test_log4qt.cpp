#include <gtest/gtest.h>

#include <log4qt/consoleappender.h>
#include <log4qt/logger.h>
#include <log4qt/logmanager.h>
#include <log4qt/patternlayout.h>

TEST(Log4Qt, RootLoggerIsNotNull)
{
	auto* logger = Log4Qt::Logger::rootLogger();
	ASSERT_NE(logger, nullptr);
}

TEST(Log4Qt, HandleQtMessagesDoesNotCrash)
{
	Log4Qt::LogManager::setHandleQtMessages(true);
	qDebug("Log4Qt::LogManager::setHandleQtMessages test");
	Log4Qt::LogManager::setHandleQtMessages(false);
	SUCCEED();
}
