#include <gtest/gtest.h>

#include <InstrumentorTimer/InstrumentorTimer.h>

#include <chrono>
#include <thread>

TEST(InstrumentorTimer, SessionBeginAndEnd)
{
	// Should not crash and file should be created
	Instrumentor::BeginSession("test_session", "test_instrumentor.json");
	Instrumentor::EndSession();
	SUCCEED();
}

TEST(InstrumentorTimer, TimerMeasuresElapsedTime)
{
	Instrumentor::BeginSession("timer_test", "test_timer.json");

	auto before = std::chrono::high_resolution_clock::now();
	{
		InstrumentationTimer timer("sleep_100ms");
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	auto after = std::chrono::high_resolution_clock::now();

	Instrumentor::EndSession();

	auto elapsed =
		std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
	EXPECT_GE(elapsed, 9);
}

TEST(InstrumentorTimer, MultipleTimersDoNotCrash)
{
	Instrumentor::BeginSession("multi_timer", "test_multi.json");
	{
		InstrumentationTimer t1("op1");
		InstrumentationTimer t2("op2");
	}
	Instrumentor::EndSession();
	SUCCEED();
}
