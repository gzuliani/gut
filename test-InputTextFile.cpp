#include "gut.h"
#include "InputTextFile.h"

#include <string>
#include <vector>

const std::vector<std::string> content = {
	"Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
	"Donec laoreet lectus et nibh scelerisque rhoncus in id lacus.",
	"Proin augue dolor, gravida vitae consequat vitae, ullamcorper vitae tortor.",
	"",
	"Morbi imperdiet odio nec felis interdum malesuada.",
	"",
};

TEST {

	// reading file line by line
	{
		InputTextFile file("file1.txt");

		for (size_t i = 0; i < 6; i++) {
			CHECK(file);
			CHECK(file.readLine() == content[i]);
		}

		CHECK(!file);
		THROWS(file.readLine(), std::runtime_error);
	}

	// reading file as a range
	{
		size_t i = 0;

		for (auto line : InputTextFile("file1.txt"))
			CHECK(line == content[i++]);

		CHECK(i == 6);
	}

	// reading file in mixed-mode
	{
		InputTextFile file("file1.txt");
		CHECK(file.readLine() == content[0]);

		auto line = file.begin();

		for (size_t i = 0; i < 3; ++i, ++line) {
			CHECK(*line == content[i + 1]);
		}

		CHECK(file);
		CHECK(*line == content[4]);

		CHECK(file);
		CHECK(file.readLine() == content[5]);

		CHECK(!file);
		THROWS(file.readLine(), std::runtime_error);
	}

	// try to open a file that not exist throws an exception
	{
		InputTextFile file("file_.txt");

		CHECK(!file);
		THROWS(file.readLine(), std::runtime_error);
	}
}
