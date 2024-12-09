#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cctype>

struct token
{
	enum types { instruction, parameters, do_dont, eof };

	types type;
	std::string value;

	token(types t, std::string v = "") : type(t), value(v) {}
};

char held_char;

char get_char(std::ifstream &s)
{
	char c;
	if (held_char)
	{
		c = held_char;
		held_char = 0;
		return c;
	}
	if (!s.get(c)) return 0;
	return c;
}

void unget_char(char c)
{
	held_char = c;
}

std::string get_number(std::ifstream &s, char c)
{
	std::string value;
	do
	{
		if (!std::isdigit(c))
		{
			unget_char(c);
			break;
		}
		value += c;
	}
	while (c = get_char(s));
	return value;
}

token get_instruction(std::ifstream &s, char c)
{
	for (unsigned i = 0; i < 2; i++)
	{
		c = get_char(s);
		if ((i == 0 && c != 'u') || (i == 1 && c != 'l'))
		{
			unget_char(c);
			throw std::runtime_error("invalid instruction");
		}
	}
	c = get_char(s);
	if (c != '(') 
	{
		unget_char(c);
		throw std::runtime_error("invalid instruction");
	}
	unget_char(c);
	return token(token::types::instruction);
}

token get_do_dont(std::ifstream &s, char c)
{
	std::string value;
	value += c;
	c = get_char(s);
	if (c != 'o')
	{
		unget_char(c);
		throw std::runtime_error("invalid instruction");
	}
	value += c;
	c = get_char(s);
	if (c != '(')
	{
		for (unsigned i = 0; i < 3; i++)
		{
			if ((i == 0 && c != 'n') || (i == 1 && c != '\'') || (i == 2 && c != 't'))
			{
				unget_char(c);
				throw std::runtime_error("invalid instruction");
			}
			value += c;
			c = get_char(s);
		}
		if (c != '(') throw std::runtime_error("invalid instruction");
	}
	unget_char(c);
	return token(token::types::do_dont, value);
}

token get_parameters(std::ifstream &s, char c)
{
	std::string parameters;
	unsigned numbers_read = 0;
	while (c = get_char(s))
	{
		if (std::isdigit(c))
		{
			numbers_read++;
			if (numbers_read > 2) throw std::runtime_error("invalid parameters");
			std::string number = get_number(s, c);
			if (number.length() > 3) throw std::runtime_error("invalid parameters");
			parameters += number;
		}
		else
		{
			if (c == ',')
			{
				if (numbers_read < 1 || numbers_read == 2) throw std::runtime_error("invalid parameters");
				parameters += c;
			}
			else if (c == ')')
			{
				if (numbers_read < 2) throw std::runtime_error("invalid parameters");
				break;
			}
			else
			{
				unget_char(c);
				throw std::runtime_error("invalid parameters");
			}
		}
	}
	return token(token::types::parameters, parameters);
}

token get_token(std::ifstream &s)
{
	char c;
	while (c = get_char(s))
	{
		switch(c)
		{
		case '\n': continue;
		case 'm': return get_instruction(s, c);
		case 'd': return get_do_dont(s, c);
		case '(': return get_parameters(s, c);
		default: throw std::runtime_error("unexpected");
		}
	}
	return token(token::types::eof);
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
		std::cout << "file could not be opened" << std::endl;
		return 2;
	}

	struct state_machine
	{
		enum class states
		{
			init, read_instruction, read_parameters
		};

		states state;

		state_machine(states s = states::init) : state(s) {}

		states get_next_state(states s)
		{
			switch(s)
			{
			case states::init: return states::read_instruction;
			case states::read_instruction: return states::read_parameters;
			}
			return states::init;
		}

		void advance()
		{
			state = get_next_state(state);
		}

		void reset()
		{
			state = states::init;
		}
	};

	int total = 0;

	state_machine sm;

	bool done = false;
	bool process_instruction = true;
	while (!done)
	{
		try
		{
			token t = get_token(f);
			switch(t.type)
			{
				case token::types::do_dont:
					std::cout << t.value << std::endl;
					process_instruction = t.value == "do";
					break;
				case token::types::instruction: 
					if (process_instruction) sm.state = state_machine::states::read_instruction;
					break;
			case token::types::parameters:
				if (sm.state == state_machine::states::read_instruction) 
				{
					std::istringstream s(t.value);
					int result = 1;
					std::string number;
					while (std::getline(s, number, ','))
						result *= std::stoi(number);
					total += result;
					sm.advance();
				}
				break;
			case token::types::eof: done = true; break;
			default: sm.reset(); break;
			}
		} catch(...) { sm.reset(); }
	}

	std::cout << "result: " << total << std::endl;

	return 0;
}
