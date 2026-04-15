#include <gtest/gtest.h>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QByteArray>
#include <QFile>
#include <QTemporaryFile>

TEST(QuaZip, CompressAndDecompress)
{
	QTemporaryFile tmp;
	ASSERT_TRUE(tmp.open());
	const QString zipPath = tmp.fileName();
	tmp.close();
	QFile::remove(zipPath); // QuaZip::mdCreate needs a clean path

	const QByteArray original =
		QByteArrayLiteral("Hello, QuaZip! This is a validation test.");

	// --- Write ---
	{
		QuaZip zip(zipPath);
		ASSERT_TRUE(zip.open(QuaZip::mdCreate));

		QuaZipFile file(&zip);
		QuaZipNewInfo info(QStringLiteral("data.txt"));
		ASSERT_TRUE(file.open(QIODevice::WriteOnly, info));
		EXPECT_EQ(file.write(original), original.size());
		file.close();
		zip.close();
	}

	// --- Read back ---
	{
		QuaZip zip(zipPath);
		ASSERT_TRUE(zip.open(QuaZip::mdUnzip));
		ASSERT_TRUE(zip.setCurrentFile(QStringLiteral("data.txt")));

		QuaZipFile file(&zip);
		ASSERT_TRUE(file.open(QIODevice::ReadOnly));
		const QByteArray content = file.readAll();
		file.close();
		zip.close();

		EXPECT_EQ(content, original);
	}
}
