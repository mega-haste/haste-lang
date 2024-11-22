#include "Reporter.hpp"
#include "AST/Statments.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
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

Reporter::Reporter(std::ifstream &file,
                   std::filesystem::path file_source_location)
    : m_file(file), m_file_source_location(file_source_location) {}

void Reporter::report(const ReporterType report_type, const std::string &title,
                      const std::initializer_list<ReportLocation> locations) {
  switch (report_type) {
  case ReporterType::Warning:
    std::cout << termcolor::yellow << termcolor::bold
              << report_type_as_string(report_type);
    break;
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

    std::cout << location.line_number << " | " << get_line(location.at)
              << std::endl;

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

void Reporter::reseek() {
  m_file.clear();
  m_file.seekg(0, std::ios::beg);
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
  }
  return "Unknown";
}

std::string Reporter::get_line(std::size_t index) {
  std::string line;
  m_file.seekg(index, std::ios::beg);

  if (m_file.fail()) {
    std::cerr << "Seek failed. Index out of bounds." << std::endl;
    return "";
  }

  char ch;
  while (m_file.tellg() > 0) {
    m_file.get(ch);
    if (ch == '\n') {
      // We've reached the beginning of the line
      break;
    }
    m_file.seekg(-2, std::ios::cur); // Move one byte backward
  }

  std::getline(m_file, line);
  return line;
}
