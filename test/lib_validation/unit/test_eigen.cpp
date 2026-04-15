#include <gtest/gtest.h>

#include <Eigen/Dense>

TEST(Eigen, MatrixMultiplyByIdentity)
{
	Eigen::Matrix2d A;
	A << 1, 2, 3, 4;
	Eigen::Matrix2d result = A * Eigen::Matrix2d::Identity();
	EXPECT_EQ(result, A);
}

TEST(Eigen, Determinant)
{
	Eigen::Matrix2d M;
	M << 1, 2, 3, 4;
	EXPECT_DOUBLE_EQ(M.determinant(), -2.0);
}

TEST(Eigen, LinearSolve)
{
	// Solve A*x = b  where A = [[4,3],[6,3]], b = [10,12]
	Eigen::Matrix2d A;
	A << 4, 3, 6, 3;
	Eigen::Vector2d b(10.0, 12.0);
	Eigen::Vector2d x = A.lu().solve(b);
	Eigen::Vector2d residual = A * x - b;
	EXPECT_NEAR(residual.norm(), 0.0, 1e-10);
}

TEST(Eigen, NormComputation)
{
	Eigen::Vector3d v(1.0, 2.0, 2.0);
	EXPECT_DOUBLE_EQ(v.norm(), 3.0);
}
