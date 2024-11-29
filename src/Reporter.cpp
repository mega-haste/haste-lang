#include "Reporter.hpp"
#include "AST/Statments.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <termcolor/termcolor.hpp>

ReportLocation::ReportLocation(std::size_t line_number, std::size_t column,
                               std::size_t at, const std::string &message)
    : line_number(line_number), column(column), at(at), message(message) {}
ReportLocation::ReportLocation(std::size_t line_number, std::size_t column,
                               std::size_t at, std::size_t len,
                               const std::string &message)
    : line_number(line_number), column(column), at(at), len(len),
      message(message) {}
ReportLocation::ReportLocation(std::size_t line_number, std::size_t column,
                               std::size_t at, std::size_t len, char caret,
                               const std::string &message)
    : line_number(line_number), column(column), at(at), len(len), caret(caret),
      message(message) {}

Reporter::Reporter() {}
Reporter::Reporter(const std::string_view content,
                   std::vector<std::string_view> *lines,
                   std::string file_source_location)
    : m_lines(lines), m_content(content),
      m_file_source_location(file_source_location) {}

void Reporter::report(const ReporterType report_type, const std::string &title,
                      const std::initializer_list<ReportLocation> locations) {
  switch (report_type) {
  case ReporterType::Warning:
    std::cout << termcolor::yellow << termcolor::bold
              << report_type_as_string(report_type);
    break;
  case ReporterType::TypeError:
  case ReporterType::SemanticError:
  case ReporterType::Error:
    std::cout << termcolor::red << termcolor::bold
              << report_type_as_string(report_type);
    break;
  case ReporterType::Info:
    std::cout << termcolor::blue << termcolor::bold
              << report_type_as_string(report_type);
    break;
  }
  std::cout << termcolor::white << ": " << title << termcolor::reset << '\n';

  for (const auto &location : locations) {
    if (location.line_number <= 0 or location.column <= 0)
      continue;

    std::cout << " --> " << m_file_source_location << ":"
              << location.line_number << ":" << location.column << std::endl;
    std::cout << "  |" << std::endl;

    std::cout << location.line_number << " | "
              << get_lines(location.line_number - 1) << std::endl;

    std::string caretLine = "  | ";
    for (std::size_t i = 0; i < location.column - 1; ++i) {
      caretLine += " "; // Add spaces to move caret to the correct column
    }
    caretLine += AST::repeat_char(location.caret,
                                  location.len); // The caret under the error
    std::cout << caretLine << "  " << location.message;
  }

  std::cout << std::endl;
}

std::string Reporter::report_type_as_string(const ReporterType &type) const {
  switch (type) {
  case ReporterType::Warning:
    return "Warning";
  case ReporterType::Error:
    return "Error";
  case ReporterType::SemanticError:
    return "Semantic Error";
  case ReporterType::Info:
    return "Info";
  case ReporterType::TypeError:
    return "Type Error";
  }
  return "Unknown";
}

std::string_view Reporter::get_lines(std::size_t index, std::size_t padding) {
  // if (index >= m_content.size())
  //   return {};
  // std::size_t start = index, end = index, lines = 0;
  //
  // while (start > 0) {
  //   if (m_content[start - 1] == '\n')
  //     lines++;
  //   if (lines == padding) {
  //     lines = 0;
  //     break;
  //   }
  //   --start;
  // }
  // while (end < m_content.size()) {
  //   if (m_content[end] == '\n')
  //     lines++;
  //   if (lines == padding)
  //     break;
  //   ++end;
  // }
  //
  // return std::string_view(m_content.data() + start, end - start);
  return (*m_lines)[index];
}
