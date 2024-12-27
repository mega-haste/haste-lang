const std = @import("std");

const fs = std.fs;

pub const TermcolorError = fs.File.WriteError;

pub const ANSI_CODE_RESET = "\x1b[00m";
pub const ANSI_CODE_BOLD = "\x1b[1m";
pub const ANSI_CODE_DARK = "\x1b[2m";
pub const ANSI_CODE_UNDERLINE = "\x1b[4m";
pub const ANSI_CODE_BLINK = "\x1b[5m";
pub const ANSI_CODE_REVERSE = "\x1b[7m";
pub const ANSI_CODE_CONCEALED = "\x1b[8m";
pub const ANSI_CODE_GRAY = "\x1b[30m";
pub const ANSI_CODE_GREY = "\x1b[30m";
pub const ANSI_CODE_RED = "\x1b[31m";
pub const ANSI_CODE_GREEN = "\x1b[32m";
pub const ANSI_CODE_YELLOW = "\x1b[33m";
pub const ANSI_CODE_BLUE = "\x1b[34m";
pub const ANSI_CODE_MAGENTA = "\x1b[35m";
pub const ANSI_CODE_CYAN = "\x1b[36m";
pub const ANSI_CODE_WHITE = "\x1b[37m";
pub const ANSI_CODE_BG_GRAY = "\x1b[40m";
pub const ANSI_CODE_BG_GREY = "\x1b[40m";
pub const ANSI_CODE_BG_RED = "\x1b[41m";
pub const ANSI_CODE_BG_GREEN = "\x1b[42m";
pub const ANSI_CODE_BG_YELLOW = "\x1b[43m";
pub const ANSI_CODE_BG_BLUE = "\x1b[44m";
pub const ANSI_CODE_BG_MAGENTA = "\x1b[45m";
pub const ANSI_CODE_BG_CYAN = "\x1b[46m";
pub const ANSI_CODE_BG_WHITE = "\x1b[47m";

pub fn text_bold(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[1m");
    }
}

pub fn text_dark(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[2m");
    }
}

pub fn text_underline(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[4m");
    }
}

pub fn text_gray(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[30m");
    }
}

pub fn text_red(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[31m");
    }
}

pub fn text_yellow(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[33m");
    }
}

pub fn text_blue(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[34m");
    }
}

pub fn text_white(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[37m");
    }
}

pub fn reset(f: fs.File) TermcolorError!void {
    if (f.supportsAnsiEscapeCodes()) {
        try f.writeAll("\x1b[00m");
    }
}
