#ifndef GUT_TAP_H
#define GUT_TAP_H

namespace gut {

class TapReport {
	int tests_;
	int failedTests_;
	std::string testName_;
	bool testFailed_;
	std::vector<gut::Notice> log_;
	bool quit_;
public:
	int failedTestCount() const { return failedTests_; }
	void onStart() {
		quit_ = false;
		tests_ = 0;
		failedTests_ = 0;
	}
	void onEnd() {
		if (quit_)
			return;
		std::cout << "1.." << tests_ << std::endl;
		if (tests_ > 0) {
			float okRatio = (tests_ - failedTests_) * 100. / tests_;
			std::cout
				<< "# failed " << failedTests_ << "/" << tests_ << " test(s), "
				<< std::fixed << std::setprecision(1)
				<< okRatio << "% ok" << std::endl;
		}
	}
	void onStartTest(const std::string& name) {
		++tests_;
		testName_ = name;
		testFailed_ = false;
	}
	void onEndTest() {
		std::ostringstream oss;
		if (testFailed_) {
			++failedTests_;
			oss << "not ";
		}
		oss << "ok " << tests_ << " - " << testName_;
		for (const auto& entry : log_) {
			oss << "\n# " << entry.toString();
		}
		std::cout << oss.str() << std::endl;
		log_.clear();
	}
	void onFailure(const gut::Notice& failure) {
		log_.push_back(failure);
		testFailed_ = true;
	}
	void onEval(const gut::Notice& eval) {
		log_.push_back(eval);
	}
	void onInfo(const gut::Notice& info) {
		log_.push_back(info);
	}
	void onWarn(const gut::Notice& warn) {
		log_.push_back(warn);
	}
	void onQuit(const std::string& reason) {
		quit_ = true;
		std::ostringstream oss;
		oss << "Bail out!";
		if (!reason.empty ())
			oss << " Reason: " << reason;
		std::cout << oss.str() << std::endl;
	}
};

} // namespace gut

#define TODO(name_, desc_) \
	TEST(name_ " # todo " desc_)

#define SKIP(name_, reason_) \
	TEST(name_ " # skipped, reason: " reason_) { \
	} \
	void skip ## __LINE__()

#define BAIL_OUT(reason_) \
	do { \
		throw gut::AbortSuite(reason_); \
	} while (0)

#endif // GUT_TAP_H
