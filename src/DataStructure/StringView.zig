const std = @import("std");

pub const StringView = struct {
    data: []const u8,

    /// Create a StringView from a full string.
    pub fn from_string(input: []const u8) StringView {
        return StringView{ .data = input };
    }

    /// Create a StringView from a subsection of a string.
    pub fn from_slice(input: []const u8, start: usize, len: usize) !StringView {
        if (start + len > input.len) {
            return error.OutOfBounds;
        }
        return StringView{ .data = input[start .. start + len] };
    }

    /// Returns the length of the StringView.
    pub fn get_len(self: *const @This()) usize {
        return self.data.len;
    }

    /// Returns true if the StringView is empty.
    pub fn is_empty(self: *const @This()) bool {
        return self.data.len == 0;
    }

    /// Returns a pointer to the first character in the view.
    pub fn get_ptr(self: *const @This()) ?*const u8 {
        return if (self.data.len == 0) null else &self.data[0];
    }

    /// Provides access to a character at a specific index.
    pub fn at(self: *const @This(), index: usize) !u8 {
        if (index >= self.data.len) {
            return error.OutOfBounds;
        }
        return self.data[index];
    }

    /// Returns a slice of the current StringView.
    pub fn slice(self: *const @This(), start: usize, len: usize) !StringView {
        return StringView.fromSlice(self.data, start, len);
    }

    /// Print the string view for debugging.
    pub fn print(self: *const @This()) void {
        std.debug.print("{s}\n", .{self.data});
    }
};
