const std = @import("std");
const c = @cImport({
    @cInclude("termcolor-c.h");
});

pub const Report = struct {
    at: usize = 0,
    line_number: usize = 1,
    column: usize = 1,
    len: usize = 1,
    caret: u21 = '~',
    msg: []const u8 = "",

    pub fn init(line_number: usize, column: usize, at: usize, msg: []const u8) Report {
        return Report{ //
            .line_number = line_number,
            .column = column,
            .at = at,
            .msg = msg,
        };
    }
};

pub const ReportType = enum { //
    Warning,
    Error,
    SemanticError,
    SyntaxError,
    TypeError,
    Info,

    pub fn to_string(self: ReportType) []const u8 {
        return switch (self) {
            .Warning => "warning",
            .Error => "error",
            .SemanticError => "sematic error",
            .SyntaxError => "syntax error",
            .TypeError => "type error",
            .Info => "info",
        };
    }
};

pub const reporter = struct {
    pub fn report(content: []const u8, title: []const u8, reports: []const Report) void {
        _ = content;
        _ = title;
        _ = reports;
    }

    fn get_lines(content: []const u8, index: usize, padding: usize) void {
        _ = content;
        _ = index;
        _ = padding;
    }
};
