#ifndef INPUTTEXTFILE_H
#define INPUTTEXTFILE_H

#include <fstream>
#include <stdexcept>

class InputTextFile {
	std::ifstream stream_;
	int lineNumber_;
public:
	class iterator : public std::iterator<std::input_iterator_tag, std::string> {
		InputTextFile* file_;
		std::string line_;
		int lineNumber_;
	public:
		iterator(InputTextFile* file, const std::string& line, int lineNumber)
		 : file_(file), line_(line), lineNumber_(lineNumber) {
		}
		iterator& operator=(const iterator& i) {
			file_ = i.file_;
			line_ = i.line_;
			lineNumber_ = i.lineNumber_;
			return *this;
		}
		bool operator==(const iterator& i) const {
			return (file_ == i.file_) && (lineNumber_ == i.lineNumber_);
		}
		bool operator!=(const iterator& i) const {
			return !operator==(i);
		}
		iterator& operator++() {
			return operator=(file_->fetchLine());
		}
		iterator operator++(int) {
			iterator me = *this;
			operator=(file_->fetchLine());
			return me;
		}
		const std::string& operator*() const {
			return line_;
		}
	};
	explicit InputTextFile(const std::string& path)
	 : stream_(path.c_str()), lineNumber_(0) {
	}
	operator bool() const {
		return good();
	}
	bool operator!() const {
		return !operator bool();
	}
	iterator begin() {
		std::string line = readLine();
		return line.empty() ? end() : iterator(this, line, lineNumber_);
	}
	iterator end() {
		return iterator(nullptr, "", -1);
	}
	std::string readLine() {
		if (!good())
			throw std::runtime_error("cannot read from file");
		std::string line;
		std::getline(stream_, line);
		++lineNumber_;
		return line;
	}
	bool good() const {
		return stream_.good();
	}
private:
	iterator fetchLine() {
		if (!good())
			return end();
		std::string line = readLine(); // increments lineNumber_
		return iterator(this, line, lineNumber_);
	}
};

#endif // INPUTTEXTFILE_H
