#include "src/ArgParser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

TEST(splitInputOptions, ProperlySplitsObserverAndSC2Args) {
	std::vector<char*> observer_options;
	std::vector<char*> sc2_options;

	const char* argv [] = {
		"./bin/Observer",
		"--Path",
		"/Users/alkurbatov/Downloads/358809_TyrZ_DoogieHowitzer_IceandChromeLE.SC2Replay",
		"--",
		"-d",
		"B89B5D6FA7CBF6452E721311BFBC6CB2",
		"-e",
		"/Applications/StarCraft II/Versions/Base75689/SC2.app/Contents/MacOS/SC2"
	};
	int argc = sizeof argv / sizeof argv[0];

	splitInputOptions(argc, const_cast<char**>(argv), &observer_options, &sc2_options);

	ASSERT_THAT(
		observer_options,
		testing::ElementsAre(
			"./bin/Observer",
			"--Path",
			"/Users/alkurbatov/Downloads/358809_TyrZ_DoogieHowitzer_IceandChromeLE.SC2Replay"
		)
	);

	ASSERT_THAT(
		sc2_options,
		testing::ElementsAre(
			"./bin/Observer",
			"-d",
			"B89B5D6FA7CBF6452E721311BFBC6CB2",
			"-e",
			"/Applications/StarCraft II/Versions/Base75689/SC2.app/Contents/MacOS/SC2"
		)
	);
}

TEST(splitInputOptions, DoesntSplitAnythingIfDelimiterNotProvided) {
	std::vector<char*> observer_options;
	std::vector<char*> sc2_options;

	const char* argv [] = {
		"./bin/Observer",
		"--Path",
		"/Users/alkurbatov/Downloads/358809_TyrZ_DoogieHowitzer_IceandChromeLE.SC2Replay"
	};
	int argc = sizeof argv / sizeof argv[0];

	splitInputOptions(argc, const_cast<char**>(argv), &observer_options, &sc2_options);

	ASSERT_THAT(
		observer_options,
		testing::ElementsAre(
			"./bin/Observer",
			"--Path",
			"/Users/alkurbatov/Downloads/358809_TyrZ_DoogieHowitzer_IceandChromeLE.SC2Replay"
		)
	);

	ASSERT_THAT(
		sc2_options,
		testing::ElementsAre(
			"./bin/Observer"
		)
	);
}

TEST(splitInputOptions, ReturnsObserverArgsIfDelimiterPassedWithoutSC2Options) {
	std::vector<char*> observer_options;
	std::vector<char*> sc2_options;

	const char* argv [] = {
		"./bin/Observer",
		"--Path",
		"/Users/alkurbatov/Downloads/358809_TyrZ_DoogieHowitzer_IceandChromeLE.SC2Replay",
		"--"
	};
	int argc = sizeof argv / sizeof argv[0];

	splitInputOptions(argc, const_cast<char**>(argv), &observer_options, &sc2_options);

	ASSERT_THAT(
		observer_options,
		testing::ElementsAre(
			"./bin/Observer",
			"--Path",
			"/Users/alkurbatov/Downloads/358809_TyrZ_DoogieHowitzer_IceandChromeLE.SC2Replay"
		)
	);

	ASSERT_THAT(
		sc2_options,
		testing::ElementsAre(
			"./bin/Observer"
		)
	);
}

TEST(splitInputOptions, ReturnsSC2ArgsIfDelimiterPassedWithoutObserverOptions) {
	std::vector<char*> observer_options;
	std::vector<char*> sc2_options;

	const char* argv [] = {
		"./bin/Observer",
		"--",
		"-d",
		"B89B5D6FA7CBF6452E721311BFBC6CB2"
	};
	int argc = sizeof argv / sizeof argv[0];

	splitInputOptions(argc, const_cast<char**>(argv), &observer_options, &sc2_options);

	ASSERT_THAT(
		observer_options,
		testing::ElementsAre(
			"./bin/Observer"
		)
	);

	ASSERT_THAT(
		sc2_options,
		testing::ElementsAre(
			"./bin/Observer",
			"-d",
			"B89B5D6FA7CBF6452E721311BFBC6CB2"
		)
	);
}

TEST(splitInputOptions, DoesntFailIfNoOptionsSpecified) {
	std::vector<char*> observer_options;
	std::vector<char*> sc2_options;

	const char* argv [] = {
		"./bin/Observer"
	};
	int argc = sizeof argv / sizeof argv[0];

	splitInputOptions(argc, const_cast<char**>(argv), &observer_options, &sc2_options);

	ASSERT_THAT(
		observer_options,
		testing::ElementsAre(
			"./bin/Observer"
		)
	);

	ASSERT_THAT(
		sc2_options,
		testing::ElementsAre(
			"./bin/Observer"
		)
	);
}
