#ifndef __REPORDER_HPP
#define __REPORDER_HPP

#include <cstddef>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

enum class ReporterType {
  Warning,
  Error,
  SemanticError,
  TypeError,
  Info,
};

struct ReportLocation {
  std::size_t line_number;
  std::size_t column;
  std::size_t at;
  std::size_t len = 0;
  char caret = '^';
  const std::string &message;

  ReportLocation(std::size_t line_number, std::size_t column, std::size_t at,
                 const std::string &message);
  ReportLocation(std::size_t line_number, std::size_t column, std::size_t at,
                 std::size_t len, const std::string &message);
  ReportLocation(std::size_t line_number, std::size_t column, std::size_t at,
                 std::size_t len, char caret, const std::string &message);
};

class Reporter {
public:
  Reporter();
  Reporter(const std::string_view content, std::vector<std::string_view> *lines,
           std::string file_source_location);

  void report(const ReporterType report_type, const std::string &title,
              const std::initializer_list<ReportLocation> locations);

private:
  std::vector<std::string_view> *m_lines;
  std::string_view m_content;
  std::string m_file_source_location;

  std::string report_type_as_string(const ReporterType &type) const;
  std::basic_ostream<char>
  get_report_type_color(const ReporterType &type) const;

  std::string_view get_lines(std::size_t line, std::size_t padding = 1);
};

#endif // !__REPORDER_HPP
