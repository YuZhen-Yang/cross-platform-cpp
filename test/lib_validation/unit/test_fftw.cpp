#include <gtest/gtest.h>

#include <cmath>
#include <fftw3.h>
#include <vector>

namespace {
	constexpr double kPi = 3.14159265358979323846;
}

TEST(FFTW3, SinePeakAtCorrectBin)
{
	constexpr int N = 64;
	constexpr int freqBin = 4;

	std::vector<double> in(N);
	for (int i = 0; i < N; ++i)
		in[i] = std::sin(2.0 * kPi * freqBin * i / N);

	fftw_complex* out = fftw_alloc_complex(N / 2 + 1);
	fftw_plan p = fftw_plan_dft_r2c_1d(N, in.data(), out, FFTW_ESTIMATE);
	fftw_execute(p);

	int peakBin = 0;
	double peakAmp = 0.0;
	for (int k = 0; k <= N / 2; ++k)
	{
		double amp = std::hypot(out[k][0], out[k][1]);
		if (amp > peakAmp)
		{
			peakAmp = amp;
			peakBin = k;
		}
	}

	fftw_destroy_plan(p);
	fftw_free(out);

	EXPECT_EQ(peakBin, freqBin);
}

TEST(FFTW3, ZeroInputProducesZeroSpectrum)
{
	constexpr int N = 16;
	std::vector<double> in(N, 0.0);
	fftw_complex* out = fftw_alloc_complex(N / 2 + 1);
	fftw_plan p = fftw_plan_dft_r2c_1d(N, in.data(), out, FFTW_ESTIMATE);
	fftw_execute(p);

	for (int k = 0; k <= N / 2; ++k)
	{
		EXPECT_NEAR(out[k][0], 0.0, 1e-12);
		EXPECT_NEAR(out[k][1], 0.0, 1e-12);
	}

	fftw_destroy_plan(p);
	fftw_free(out);
}

TEST(FFTW3, DCComponentEqualsSum)
{
	constexpr int N = 8;
	std::vector<double> in = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
	double expectedDC = 0.0;
	for (double v : in)
		expectedDC += v;

	fftw_complex* out = fftw_alloc_complex(N / 2 + 1);
	fftw_plan p = fftw_plan_dft_r2c_1d(N, in.data(), out, FFTW_ESTIMATE);
	fftw_execute(p);

	// out[0][0] is the real part of the DC bin = sum of inputs
	EXPECT_NEAR(out[0][0], expectedDC, 1e-10);

	fftw_destroy_plan(p);
	fftw_free(out);
}
