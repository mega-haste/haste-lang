const std = @import("std");
const termcolor = @import("./termcolor.zig");

pub const Report = struct {
    kind: ReportType = .Info,
    at: usize = 0,
    line_number: usize = 1,
    column: usize = 1,
    len: usize = 1,
    caret: u8 = '~',
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
            .Warning => "Warning",
            .Error => "Error",
            .SemanticError => "Sematic Error",
            .SyntaxError => "Syntax Error",
            .TypeError => "Type Error",
            .Info => "Info",
        };
    }
};

pub const reporter = struct {
    pub fn report(out: std.fs.File, content: []const u8, kind: ReportType, title: []const u8, reports: []const Report) void {
        const out_writer = out.writer();
        resolve(termcolor.text_bold(out));
        switch (kind) {
            .Warning => resolve(termcolor.text_yellow(out)),
            .TypeError, .SyntaxError, .SemanticError, .Error => resolve(termcolor.text_red(out)),
            .Info => resolve(termcolor.text_blue(out)),
        }
        resolve(out.writeAll(kind.to_string()));

        resolve(termcolor.text_white(out));
        resolve(out.writeAll(": "));
        resolve(out.writeAll(title));
        resolve(out.writeAll("\n"));
        resolve(termcolor.reset(out));

        for (reports) |location| {
            if (location.line_number <= 0 or location.column <= 0)
                continue;

            resolve(out.writeAll(" --> :"));
            // TODO: Working file location
            resolve(out_writer.print("file.haste[{}:{}]\n", .{ location.line_number, location.column }));
            print_blank(out, location.line_number);
            resolve(out.writeAll(" |\n"));

            resolve(out_writer.print("{} | {s}\n", .{ location.line_number, get_lines(content, location.at, 1) }));

            print_blank(out, location.line_number);
            resolve(out.writeAll(" | "));

            var caret_line = std.heap.page_allocator.alloc(u8, location.column + location.len) catch std.debug.panic("Bad allocation.\n", .{});
            defer std.heap.page_allocator.free(caret_line);
            for (0..location.column - 1) |i| {
                caret_line[i] = ' ';
            }
            for (location.column..(location.column + location.len)) |i| {
                caret_line[i] = location.caret;
            }

            resolve(out_writer.print("{s} {s}", .{ caret_line, location.msg }));
        }

        resolve(out.writeAll("\n"));
        resolve(termcolor.reset(out));
    }

    fn get_lines(content: []const u8, index: usize, padding: usize) []const u8 {
        if (index >= content.len)
            return "";

        var start: usize = index;
        var end: usize = index;
        var lines: usize = 0;

        while (start > 0) : (start -= 1) {
            if (content[start - 1] == '\n') {
                lines += 1;
            }

            if (lines == padding) {
                lines = 0;
                break;
            }
        }

        while (end < content.len) : (end += 1) {
            if (content[end] == '\n') {
                lines += 1;
            }

            if (lines == padding) {
                break;
            }
        }

        return content[start..end];
    }
};

fn print_blank(out: std.fs.File, num: usize) void {
    resolve(out.writeAll(switch (num) {
        0...9 => " ",
        10...99 => "  ",
        100...999 => "   ",
        else => unreachable,
    }));
}
fn resolve(res: anyerror!void) void {
    res catch std.debug.panic("Something bad happened.", .{});
}
