#ifndef GUT_TIMING_H
#define GUT_TIMING_H

#define GUT_HAS_CHRONO

#if defined (_MSC_VER) && (_MSC_VER <= 1600)
#undef GUT_HAS_CHRONO
#endif

#ifdef GUT_HAS_CHRONO

    #include "timing-chrono.h"

#else

    #include "timing-custom.h"

#endif

#define GUT_DURATION(expr_) \
    [&] { \
        gut::Timer timer_; \
        (void)(expr_); \
        return timer_.elapsedTime(); \
    }()

#define SHOULD_LAST_AT_MOST(expr_, limit_) \
    GUT_BEGIN \
        auto duration_ = GUT_DURATION(expr_); \
        if (duration_ > limit_) \
            WARN( \
                std::string(#expr_) \
                    + " took " \
                    + gut::toString(gut::match_duration(duration_, limit_)) \
                    + " (expected less than " \
                    + gut::toString(gut::match_duration(limit_, duration_)) \
                    + ")"); \
    GUT_END

#define LASTS_AT_MOST(expr_, limit_) \
    GUT_BEGIN \
        auto duration_ = GUT_DURATION(expr_); \
        if (duration_ > limit_) \
            gut::theListener.failure( \
                gut::DurationFailure( \
                    #expr_, \
                    gut::match_duration(duration_, limit_), \
                    gut::match_duration(limit_, duration_), \
                    __FILE__, \
                    __LINE__)); \
    GUT_END

#define REQUIRE_LASTS_AT_MOST(expr_, limit_) \
    GUT_BEGIN \
        auto duration_ = GUT_DURATION(expr_); \
        if (duration_ > limit_) { \
            gut::theListener.failure( \
                gut::FatalDurationFailure( \
                    #expr_, \
                    gut::match_duration(duration_, limit_), \
                    gut::match_duration(limit_, duration_), \
                    __FILE__, \
                    __LINE__)); \
            throw gut::AbortTest(); \
        } \
    GUT_END

#endif // GUT_TIMING_H
