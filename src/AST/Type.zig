const std = @import("std");
const TokenMod = @import("../Token.zig");

const Token = TokenMod.Token;
const mem = std.mem;
const debug = std.debug;

const ident_size: comptime_int = 2;

pub const Type = struct {
    start: *const Token,
    end: *const Token,
    tp: *const TypeNode,

    pub fn print(self: *const @This(), depth: u32) void {
        TypeNode.print_identation(depth);
        debug.print("start: ", .{});
        self.start.print();

        TypeNode.print_identation(depth);
        debug.print("end: ", .{});
        self.end.print();

        self.tp.print(depth);
    }
};

pub const Primitive = enum {
    UInt,
    Int,
    Float,
    Bool,
    Void,

    pub fn stringfy(self: @This()) []const u8 {
        return switch (self) {
            .UInt => "UInt",
            .Int => "Int",
            .Float => "Float",
            .Bool => "Bool",
            .Void => "Void",
        };
    }
};

pub const TypeNode = union(enum) {
    Primitive: Primitive,

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
        print_identation(depth);
        switch (self.*) {
            .Primitive => |v| {
                debug.print("Primitive: {s}\n", .{v.stringfy()});
            },
        }
    }
};

pub fn create_type(alloc: mem.Allocator, start: *const Token, end: *const Token, tp: *const TypeNode) !*Type {
    const result = try alloc.create(Type);

    result.start = start;
    result.end = end;
    result.tp = tp;

    return result;
}

pub fn create_primitive(alloc: mem.Allocator, tp: Primitive) !*TypeNode {
    const result = try alloc.create(TypeNode);

    result.* = .{ .Primitive = tp };

    return result;
}
