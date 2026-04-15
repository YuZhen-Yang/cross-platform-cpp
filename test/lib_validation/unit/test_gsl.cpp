#include <gtest/gtest.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_statistics_double.h>

TEST(GSL, BesselJ0AtZeroIsOne)
{
	EXPECT_NEAR(gsl_sf_bessel_J0(0.0), 1.0, 1e-10);
}

TEST(GSL, BesselJ0FirstZero)
{
	// First zero of J0 is at approximately 2.4048
	EXPECT_NEAR(gsl_sf_bessel_J0(2.4048), 0.0, 1e-3);
}

TEST(GSL, StatisticsMean)
{
	double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
	double mean = gsl_stats_mean(data, 1, 5);
	EXPECT_NEAR(mean, 3.0, 1e-10);
}

TEST(GSL, StatisticsVariance)
{
	double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
	double var = gsl_stats_variance(data, 1, 5);
	EXPECT_NEAR(var, 2.5, 1e-10);
}

TEST(GSL, PowInt)
{
	EXPECT_NEAR(gsl_pow_2(3.0), 9.0, 1e-10);
	EXPECT_NEAR(gsl_pow_3(2.0), 8.0, 1e-10);
}
