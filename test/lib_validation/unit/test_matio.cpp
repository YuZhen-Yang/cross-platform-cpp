#include <gtest/gtest.h>

#include <matio.h>

#include <cstdio>
#include <filesystem>

TEST(Matio, WriteAndReadDoubleArray)
{
	const char* filepath = "test_matio_temp.mat";

	// --- Write ---
	{
		mat_t* mat = Mat_CreateVer(filepath, nullptr, MAT_FT_DEFAULT);
		ASSERT_NE(mat, nullptr);

		size_t dims[2] = {3, 1};
		double data[3] = {1.0, 2.0, 3.0};
		matvar_t* var = Mat_VarCreate("x", MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, data, 0);
		ASSERT_NE(var, nullptr);

		EXPECT_EQ(Mat_VarWrite(mat, var, MAT_COMPRESSION_NONE), 0);
		Mat_VarFree(var);
		Mat_Close(mat);
	}

	// --- Read back ---
	{
		mat_t* mat = Mat_Open(filepath, MAT_ACC_RDONLY);
		ASSERT_NE(mat, nullptr);

		matvar_t* var = Mat_VarRead(mat, "x");
		ASSERT_NE(var, nullptr);

		EXPECT_EQ(var->rank, 2);
		EXPECT_EQ(var->dims[0], static_cast<size_t>(3));

		const auto* data = static_cast<const double*>(var->data);
		EXPECT_DOUBLE_EQ(data[0], 1.0);
		EXPECT_DOUBLE_EQ(data[1], 2.0);
		EXPECT_DOUBLE_EQ(data[2], 3.0);

		Mat_VarFree(var);
		Mat_Close(mat);
	}

	std::filesystem::remove(filepath);
}

TEST(Matio, OpenNonexistentFileReturnsNull)
{
	mat_t* mat = Mat_Open("nonexistent_file_xyz.mat", MAT_ACC_RDONLY);
	EXPECT_EQ(mat, nullptr);
}
