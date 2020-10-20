#include "ArgParser.h"

#include <cstring>

void splitInputOptions(int argc, char* argv[], std::vector<char*>* observer_options, std::vector<char*>* sc2_options) {
	const char* delimiter = "--";

	observer_options->push_back(argv[0]);
	sc2_options->push_back(argv[0]);

	std::vector<char*>* marker = observer_options;

	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], delimiter) == 0) {
			marker = sc2_options;
			continue;
		}

		marker->push_back(argv[i]);
	}
}
