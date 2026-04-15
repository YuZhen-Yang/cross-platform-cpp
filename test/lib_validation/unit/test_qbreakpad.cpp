#include <gtest/gtest.h>

#include <QBreakpadHandler.h>

TEST(QBreakpad, VersionStringNotEmpty)
{
	EXPECT_FALSE(QBreakpadHandler::version().isEmpty());
}

TEST(QBreakpad, ConstructionAndDestructionDoNotCrash)
{
	QBreakpadHandler handler;
	SUCCEED();
}
