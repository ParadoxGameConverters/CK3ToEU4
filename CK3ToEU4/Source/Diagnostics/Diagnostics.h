#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include <chrono>
#include <filesystem>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace diagnostics
{
enum class Severity
{
	Info,
	Warning,
	Error
};

// Collects facts about one conversion run and writes them to debug/conversion_report.txt.
// Everything recorded here is also written to log.txt, so the report is a summary of the log, not the only copy.
class Report
{
  public:
	static Report& instance();

	void recordIssue(Severity severity, const std::string& stage, const std::string& message);
	void addCount(const std::string& name, long long delta);
	void addNote(const std::string& section, const std::string& note);
	void recordTiming(const std::string& stage, std::chrono::milliseconds duration);
	// Counts save lines whose key has the melter placeholder form, such as "jens_179=". Some normal keys share that form, so the
	// report lists the most common names for comparison with a known-good save. CK3 1.20 saves contain a large number of
	// placeholders that CK3 1.19 saves do not, and those fields are misread until rakaly is updated.
	void scanUnresolvedTokens(const std::string& gamestate);
	void write(const std::filesystem::path& outputPath) const;

  private:
	Report() = default;

	struct Issue
	{
		Severity severity;
		std::string stage;
		std::string message;
	};

	std::vector<Issue> issues;
	std::map<std::string, long long> counts;
	std::map<std::string, std::vector<std::string>> notes;
	std::vector<std::pair<std::string, std::chrono::milliseconds>> timings;
	std::map<std::string, long long> unresolvedTokens;
};

// Times a stage and records the duration in the report when it goes out of scope.
class ScopedTimer
{
  public:
	explicit ScopedTimer(std::string stage): name(std::move(stage)), start(std::chrono::steady_clock::now()) {}
	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator=(const ScopedTimer&) = delete;
	~ScopedTimer()
	{
		Report::instance().recordTiming(name, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start));
	}

  private:
	std::string name;
	std::chrono::steady_clock::time_point start;
};
} // namespace diagnostics

#endif // DIAGNOSTICS_H
