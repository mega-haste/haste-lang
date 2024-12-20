const std = @import("std");
const TokenMod = @import("../Token.zig");
const TypeMod = @import("./Type.zig");
const ExprMod = @import("./Expr.zig");

const Token = TokenMod.Token;
const mem = std.mem;
const debug = std.debug;

const ident_size: comptime_int = 2;

pub const ArgNode = struct {
    start: *const Token,
    end: *const Token,
    ident: *const Token,
    tp: *const TypeMod.Type,
    default: ?*const ExprMod.Expr,

    pub fn print(self: *const @This(), depth: u32) void {
        StmtNode.print_identation(depth);
        debug.print("start: ", .{});
        self.start.print();

        StmtNode.print_identation(depth);
        debug.print("end: ", .{});
        self.end.print();

        StmtNode.print_identation(depth);
        debug.print("ident: ", .{});
        StmtNode.print_token(self.ident);
        // TODO:
    }

    pub fn deinit(self: *const @This()) void {
        self.tp.deinit();
        if (self.default) |default| {
            default.deinit();
        }
    }
};

pub const Visibility = enum { //
    Public,
    Private,

    pub fn stringfy(self: @This()) []const u8 {
        switch (self) {
            .Public => "public",
            .Private => "private",
        }
    }
};

pub const Stmt = struct {
    visibility: Visibility = .Private,
    start: *const Token,
    end: *const Token,
    stmt: *const StmtNode,

    pub fn print(self: *const @This(), depth: u32) void {
        StmtNode.print_identation(depth);
        debug.print("start: ", .{});
        self.start.print();

        StmtNode.print_identation(depth);
        debug.print("end: ", .{});
        self.end.print();

        StmtNode.print_identation(depth);
        debug.print("visibility: {s}\n", .{self.visibility.stringfy()});

        self.stmt.print(depth);
    }

    pub fn deinit(self: *const @This()) void {
        self.stmt.deinit();
    }
};

pub const StmtNode = union(enum) {
    ItemFunc: struct {
        identifier: *const Token,
        arguments: []const ArgNode,
        return_type: ?*TypeMod.Type,
        body: []const Stmt,

        pub fn print(self: *const @This(), depth: u32) void {
            print_identation(depth);
            debug.print("identifier: ", .{});
            print_token(self.identifier);

            for (self.arguments, 0..) |argument, i| {
                print_identation(depth);
                debug.print("{}:\n", .{i});
                argument.print(depth + ident_size);
            }
        }

        pub fn deinit(self: *const @This()) void {
            self.return_type.deinit();
            for (self.arguments) |argument| {
                argument.deinit();
            }
            for (self.body) |stmt| {
                stmt.deinit();
            }
        }
    },
    Let: struct {
        identifier: *const Token,
        tp: ?*const TypeMod.Type = null,
        eq: ?*const Token = null,
        init: ?*const ExprMod.Expr = null,

        pub fn print(self: *const @This(), depth: u32) void {
            print_identation(depth);
            debug.print("identifier: ", .{});
            print_token(self.identifier);

            print_identation(depth);
            debug.print("eq: ", .{});
            if (self.eq) |eq| {
                eq.print();
            } else {
                debug.print("NONE\n", .{});
            }

            print_identation(depth);
            debug.print("tp:\n", .{});
            if (self.tp) |tp| {
                tp.print(depth + ident_size);
            } else {
                print_identation(depth + ident_size);
                debug.print("NONE\n", .{});
            }

            print_identation(depth);
            debug.print("init:\n", .{});
            if (self.init) |init| {
                init.print(depth + ident_size);
            } else {
                print_identation(depth + ident_size);
                debug.print("NONE\n", .{});
            }
        }

        pub fn deinit(self: *const @This()) void {
            if (self.init) |init| {
                init.deinit();
            }
            if (self.tp) |tp| {
                tp.deinit();
            }
        }
    },

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
            .ItemFunc => |v| v.print(depth + ident_size),
            .Let => |v| v.print(depth + ident_size),
        }
    }

    pub fn deinit(self: *const @This()) void {
        switch (self.*) {
            .ItemFunc => |v| v.deinit(),
            .Let => |v| v.deinit(),
        }
    }
};
