#ifndef __REPORDER_HPP
#define __REPORDER_HPP

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>

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
  Reporter(std::ifstream &file, std::filesystem::path file_source_location);

  void report(const ReporterType report_type, const std::string &title,
              const std::initializer_list<ReportLocation> locations);

  void reseek();

private:
  std::ifstream &m_file;
  std::filesystem::path m_file_source_location;

  std::string report_type_as_string(const ReporterType &type) const;
  std::basic_ostream<char>
  get_report_type_color(const ReporterType &type) const;

  std::string get_line(std::size_t line);
};

#endif // !__REPORDER_HPP
