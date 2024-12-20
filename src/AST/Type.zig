const std = @import("std");
const TokenMod = @import("../Token.zig");

const Token = TokenMod.Token;
const mem = std.mem;
const debug = std.debug;

const ident_size: comptime_int = 2;

pub const Type = struct {
    start: *const Token,
    end: *const Token,
    tp: *Type,

    pub fn print(self: *const @This(), depth: u32) void {
        _ = self;
        _ = depth;
        debug.print("UNIMPLEMENTED", .{});
    }

    pub fn deinit(self: *const @This()) void {
        self.tp.deinit();
    }
};

pub const TypeNode = union(enum) {
    fn get_identation(depth: u32) []const u8 {
        var ident: [100]u8 = undefined;
        @memset(&ident, ' ');
        return ident[0..depth];
    }

    fn print_identation(depth: u32) void {
        debug.print("{s}", .{get_identation(depth)});
    }

    fn print_token(token: *const Token) void {
        std.debug.print("`{s}`\n", .{token.lexem.data});
    }

    pub fn print(self: *const @This(), depth: u32) void {
        _ = self;
        _ = depth;
        debug.print("UNIMPLEMENTED", .{});
    }

    pub fn deinit(self: *const @This()) void {
        _ = self;
    }
};
