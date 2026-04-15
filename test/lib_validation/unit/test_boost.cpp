#include <gtest/gtest.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

TEST(Boost, FilesystemCurrentPathNotEmpty)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	EXPECT_FALSE(p.empty());
}

TEST(Boost, FilesystemTempDirExists)
{
	EXPECT_TRUE(boost::filesystem::exists(boost::filesystem::temp_directory_path()));
}

TEST(Boost, AlgorithmToUpperCopy)
{
	std::string result = boost::algorithm::to_upper_copy(std::string("hello world"));
	EXPECT_EQ(result, "HELLO WORLD");
}

TEST(Boost, AlgorithmTrimCopy)
{
	std::string result = boost::algorithm::trim_copy(std::string("  hello  "));
	EXPECT_EQ(result, "hello");
}
