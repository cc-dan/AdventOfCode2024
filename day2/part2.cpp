#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

unsigned is_acceptable_monotonic(std::vector<int> report, unsigned from, bool ascending)
{
	if (report.size() == 1) return from;
	if (from == report.size() - 1) return from;
	
	const int a = report[from];
	const int b = report[from + 1];
	if (a < b && !ascending) return from;
	if (a > b && ascending) return from;

	const unsigned difference = std::abs(a - b);
	if (difference < 1 || difference > 3) return from;

	return is_acceptable_monotonic(report, ++from, ascending);
}

bool is_safe(std::vector<int> report, bool ascending)
{
	if (report.size() == 1) return true;

	const unsigned last_good_index = is_acceptable_monotonic(report, 0, ascending);
	if (last_good_index == (report.size() - 1)) return true;

	if (last_good_index == (report.size() - 2)) return true; // last level can be ignored

	if (last_good_index == 0) // first level could be ignored...
	{
		if (is_acceptable_monotonic(report, 1, true) == (report.size() - 1)) return true; // ...if the rest is an ascending monotonic
		if (is_acceptable_monotonic(report, 1, false) == (report.size() - 1)) return true; // ...if the rest is a descending monotonic
	}
	else
	{
		// try to ignore this level
		const int a = report[last_good_index - 1];
		const int b = report[last_good_index + 1];
		const unsigned difference = std::abs(a - b);
		if (difference >= 1 && difference <= 3)
		{
			if (ascending && b > a && is_acceptable_monotonic(report, last_good_index + 1, true) == (report.size() - 1)) return true;
			if (!ascending && b < a && is_acceptable_monotonic(report, last_good_index + 1, false) == (report.size() - 1)) return true;
		}
	}

	// this level can't be ignored. try to ignore the next level
	const int a = report[last_good_index];
	const int b = report[last_good_index + 2];
	const unsigned difference = std::abs(a - b);
	if (difference < 1 || difference > 3) return false;

	if (ascending && b > a && is_acceptable_monotonic(report, last_good_index + 2, true) == (report.size() - 1)) return true;
	return (!ascending && b < a && is_acceptable_monotonic(report, last_good_index + 2, false) == (report.size() - 1));
}

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

	unsigned safe_reports = 0;
	for (auto report : reports)
		if (is_safe(report, true) || is_safe(report, false)) safe_reports++;

	std::cout << "total safe reports: " << safe_reports << std::endl;

	return 0;
}
