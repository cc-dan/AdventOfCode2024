#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string_view>

constexpr std::string_view word = "XMAS";

bool search(std::vector<std::vector<char>> m, unsigned column, unsigned row, unsigned char_index, int dir_x, int dir_y)
{
	if (char_index >= word.length()) return true;
	if (row + dir_y >= m.size() || row + dir_y < 0) return false;
	if (column + dir_x >= m[0].size() || column + dir_x < 0) return false;
	row += dir_y;
	column += dir_x;
	if (m[row][column] != word[char_index]) return false;
	return search(m, column, row, char_index + 1, dir_x, dir_y);
}

unsigned matches(std::vector<std::vector<char>> m, unsigned column, unsigned row)
{
	if (column < 0 || row < 0 || column > m[0].size() || row > m.size()) throw std::out_of_range("out of range");
	if (m[row][column] != word[0]) return 0;

	unsigned matches = 0;

	// horizontal
	if (search(m, column, row, 1, 1, 0)) matches++;
	if (search(m, column, row, 1, -1, 0)) matches++;
	if (search(m, column, row, 1, 0, 1)) matches++;
	if (search(m, column, row, 1, 0, -1)) matches++;

	// diagonal
	if (search(m, column, row, 1, 1, 1)) matches++;
	if (search(m, column, row, 1, 1, -1)) matches++;
	if (search(m, column, row, 1, -1, 1)) matches++;
	if (search(m, column, row, 1, -1, -1)) matches++;

	return matches;
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

	std::vector<std::vector<char>> matrix;

	std::string line;
	while (std::getline(f, line))
	{
		std::vector<char> characters;
		char c;

		std::istringstream stream(line);
		while (stream >> c)
			characters.push_back(c);
		matrix.push_back(characters);
	}

	unsigned total = 0;
	for (unsigned row = 0; row < matrix.size(); row++)
	{
		for (unsigned column = 0; column < matrix[0].size(); column++)
			if (matrix[row][column] == 'X') total += matches(matrix, column, row);
	}
	std::cout << total << std::endl;
}
