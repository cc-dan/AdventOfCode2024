#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "usage: " << argv[0] << " input_file" << std::endl;
		return 1;
	}

	std::ifstream f(argv[1]);
	if (!f.is_open())
	{
		std::cout << "could not open " << argv[1] << std::endl;
		return 2;
	}

	std::vector<std::vector<int>> reports;

	std::string line;
	while (std::getline(f, line))
	{
		std::vector<int> numbers;
		int number;

		std::istringstream stream(line);
		while (stream >> number)
			numbers.push_back(number);
		reports.push_back(numbers);
	}

	unsigned index = 0;
	unsigned safe_reports = 0;
	for (auto report : reports)
	{
		bool ascending = *report.begin() < *(report.begin() + 1);

		bool monotonic = true, safe = true;
		unsigned errors = 0;
		for (auto i = report.begin() + 1; i < report.end(); i++)
		{
			const int level = *i;
			const int neighbor = *(i - 1);
			const unsigned difference = std::abs(level - neighbor);
			monotonic = monotonic && ((ascending && (neighbor < level)) || (!ascending && (neighbor > level)));
			safe = safe && (monotonic && difference >= 1 && difference <= 3);
		}
		std::cout << "report " << ++index << " is " << (safe? "" : "not ") << "safe" << std::endl;
		if (safe) safe_reports++;
	}

	std::cout << "total safe reports: " << safe_reports << std::endl;

	return 0;
}
