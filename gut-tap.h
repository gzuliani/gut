#ifndef GUT_TAP_H
#define GUT_TAP_H

namespace gut {

class TapReport {
    std::ostream& os_;
    int testCount_;
    std::string testName_;
    std::vector<std::string> log_;
    bool quit_;
public:
    TapReport(std::ostream& os = std::cout) : os_(os) { }
    void start() {
        testCount_ = 0;
        quit_ = false;
    }
    void end(int tests, int failedTests, int /*failures*/, double /*duration*/) {
        if (quit_)
            return;
        os_ << "1.." << tests << std::endl;
        if (tests > 0) {
            float okRatio = (tests - failedTests) * 100. / tests;
            os_
                << "# failed " << failedTests << "/" << tests << " test(s), "
                << std::fixed << std::setprecision(1)
                << okRatio << "% ok" << std::endl;
        }
    }
    void startTest(const std::string& name) {
        ++testCount_;
        testName_ = name;
    }
    void endTest(bool failed, double /*duration*/) {
        std::ostringstream oss;
        if (failed)
            oss << "not ";
        oss << "ok " << testCount_ << " - " << testName_;
        for (const auto& entry : log_) {
            oss << "\n# " << entry;
        }
        os_ << oss.str() << std::endl;
        log_.clear();
    }
    void failure(const char* file, int line, int /*level*/, const std::string& what) {
        append(file, line, what);
    }
    void info(const char* file, int line, int /*level*/, const std::string& what) {
        append(file, line, what);
    }
    void quit(const std::string& reason) {
        quit_ = true;
        std::ostringstream oss;
        oss << "Bail out!";
        if (!reason.empty ())
            oss << " Reason: " << reason;
        os_ << oss.str() << std::endl;
    }
protected:
    void append(const char* file, int line, const std::string& what) {
        std::ostringstream oss;
        oss << file << "(" << line << ") : " << what;
        log_.push_back(oss.str());
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
