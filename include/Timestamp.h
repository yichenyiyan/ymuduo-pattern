#ifndef YMUDUO_TIMESTAMP
#define YMUDUO_TIMESTAMP

#include <sys/time.h>
#include <inttypes.h>

#include <string>

namespace ymuduo {

// 时间戳
class Timestamp {
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    ~Timestamp();

    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    
    static Timestamp now();
    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;
    std::string toFormattedStringWithoutSeconds() const; // YY-MM-DD_HH-MM

    void swap(Timestamp& that) { 
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_); 
    }

    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    /* 返回一个无效的时间戳对象 */
    static Timestamp invalid()    { return Timestamp(); }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

/*after second Version*/
public:
    Timestamp operator+ (int64_t microseconds) const {
        return Timestamp(microSecondsSinceEpoch_ + microseconds);
    }

    Timestamp& operator+= (int64_t microseconds) {
        microSecondsSinceEpoch_ += microseconds;
        return *this;
    }

    bool operator== (const Timestamp& rhs) const {
        return microSecondsSinceEpoch_ == rhs.microSecondsSinceEpoch_;
    }

    bool operator>= (const Timestamp& rhs) const {
        return microSecondsSinceEpoch_ >= rhs.microSecondsSinceEpoch_;
    }

private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator< (const Timestamp& lhs, const Timestamp& rhs) { 
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch(); 
}

inline Timestamp operator+ (const Timestamp& lhs, const Timestamp& rhs) {
    auto result = lhs.microSecondsSinceEpoch() + rhs.microSecondsSinceEpoch();
    return Timestamp(result); 
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

inline double timeDifference(Timestamp high, Timestamp low) {
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}





}  // namespace ymuduo

#endif