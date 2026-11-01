#include "Diagnostics.h"
#include "Log.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <string_view>

namespace diagnostics
{
namespace
{
constexpr std::size_t maxNotesShown = 200;
constexpr std::size_t maxTokenNamesShown = 30;

// Returns the key of a save line such as "\t\tjens_179=..." when the key has the placeholder form letters_digits. Otherwise returns an empty view.
std::string_view placeholderKey(std::string_view line)
{
	std::size_t pos = 0;
	while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t'))
		++pos;
	const auto keyStart = pos;
	while (pos < line.size() && std::islower(static_cast<unsigned char>(line[pos])))
		++pos;
	if (pos == keyStart || pos >= line.size() || line[pos] != '_')
		return {};
	++pos;
	const auto digitStart = pos;
	while (pos < line.size() && std::isdigit(static_cast<unsigned char>(line[pos])))
		++pos;
	if (pos == digitStart || pos >= line.size() || line[pos] != '=')
		return {};
	return line.substr(keyStart, pos - keyStart);
}

std::string severityName(Severity severity)
{
	switch (severity)
	{
		case Severity::Error:
			return "ERROR";
		case Severity::Warning:
			return "WARNING";
		case Severity::Info:
			return "INFO";
	}
	return "INFO";
}
} // namespace

Report& Report::instance()
{
	static Report theReport;
	return theReport;
}

void Report::recordIssue(Severity severity, const std::string& stage, const std::string& message)
{
	issues.push_back(Issue{severity, stage, message});
	const auto level = severity == Severity::Error ? LogLevel::Error : (severity == Severity::Warning ? LogLevel::Warning : LogLevel::Info);
	Log(level) << "[" << stage << "] " << message;
}

void Report::addCount(const std::string& name, long long delta)
{
	counts[name] += delta;
}

void Report::addNote(const std::string& section, const std::string& note)
{
	notes[section].push_back(note);
}

void Report::recordTiming(const std::string& stage, std::chrono::milliseconds duration)
{
	timings.emplace_back(stage, duration);
}

void Report::scanUnresolvedTokens(const std::string& gamestate)
{
	const std::string_view text(gamestate);
	std::size_t lineStart = 0;
	while (lineStart < text.size())
	{
		auto lineEnd = text.find('\n', lineStart);
		if (lineEnd == std::string_view::npos)
			lineEnd = text.size();
		const auto key = placeholderKey(text.substr(lineStart, lineEnd - lineStart));
		if (!key.empty())
			++unresolvedTokens[std::string(key)];
		lineStart = lineEnd + 1;
	}

	const auto total = std::accumulate(unresolvedTokens.begin(), unresolvedTokens.end(), 0LL, [](long long sum, const auto& token) {
		return sum + token.second;
	});
	// Some normal save keys also look like placeholders (for example trinket_1 or spouse_1), so this is information, not an error.
	// Compare the top names in the report with a known-good save to see what is new.
	Log(LogLevel::Info) << "<> Found " << total << " placeholder-style keys across " << unresolvedTokens.size() << " names.";
}

void Report::write(const std::filesystem::path& outputPath) const
{
	std::error_code ec;
	if (outputPath.has_parent_path())
		std::filesystem::create_directories(outputPath.parent_path(), ec);
	std::ofstream report(outputPath);
	if (!report.is_open())
	{
		Log(LogLevel::Warning) << "Could not write conversion report to " << outputPath.string();
		return;
	}

	report << "CK3ToEU4 conversion report\n\n";

	report << "== Timings (ms) ==\n";
	for (const auto& [stage, duration]: timings)
		report << std::left << std::setw(40) << stage << duration.count() << "\n";

	report << "\n== Counts ==\n";
	for (const auto& [name, value]: counts)
		report << name << ": " << value << "\n";

	report << "\n== Issues ==\n";
	if (issues.empty())
		report << "none\n";
	for (const auto& issue: issues)
		report << severityName(issue.severity) << " [" << issue.stage << "] " << issue.message << "\n";

	std::vector<std::pair<std::string, long long>> tokens(unresolvedTokens.begin(), unresolvedTokens.end());
	std::sort(tokens.begin(), tokens.end(), [](const auto& a, const auto& b) {
		return a.second != b.second ? a.second > b.second : a.first < b.first;
	});
	report << "\n== Unresolved save tokens (top " << maxTokenNamesShown << " of " << tokens.size() << ") ==\n";
	for (std::size_t i = 0; i < tokens.size() && i < maxTokenNamesShown; ++i)
		report << tokens[i].first << ": " << tokens[i].second << "\n";

	report << "\n== Notes ==\n";
	for (const auto& [section, entries]: notes)
	{
		report << "[" << section << "] " << entries.size() << " entries\n";
		for (std::size_t i = 0; i < entries.size() && i < maxNotesShown; ++i)
			report << "  " << entries[i] << "\n";
		if (entries.size() > maxNotesShown)
			report << "  ... " << entries.size() - maxNotesShown << " more\n";
	}
}
} // namespace diagnostics
