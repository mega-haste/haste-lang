const std = @import("std");
const TokenMod = @import("./Token.zig");
const FrontEnd = @import("./FrontEnd.zig");
const AST = @import("./AST.zig");

const fs = std.fs;
const mem = std.mem;

pub const TranslationUnit = struct {
    allocator: mem.Allocator,
    arena_alloc: mem.Allocator,
    path: []const u8,
    content: []const u8,
    tokens: TokenMod.TokenList,
    program: AST.ProgramAST,

    pub fn init(allocator: mem.Allocator, arena_alloc: mem.Allocator, path: []const u8) anyerror!TranslationUnit {
        const file = try fs.openFileAbsolute(path, .{ .mode = .read_only });
        defer file.close();

        const size = try file.getEndPos();
        const content = try allocator.alloc(u8, size + 1);

        const byte_read = try file.readAll(content);
        if (byte_read < size)
            return fs.File.ReadError.Unexpected;
        content[size] = 0x0;

        return TranslationUnit{ //
            .allocator = allocator,
            .arena_alloc = arena_alloc,
            .path = path,
            .tokens = TokenMod.TokenList.init(allocator),
            .content = content,
            .program = AST.ProgramAST.init(allocator),
        };
    }

    pub fn frontend(self: *@This()) !void {
        try self.lexical_analysis();
        try self.syntax_analysis();
        try self.semantic_analysis();
    }

    fn lexical_analysis(self: *@This()) !void {
        var scanner = FrontEnd.Scanner.init(&self.tokens, self.content);
        try scanner.scan();

        self.tokens.shrinkAndFree(self.tokens.items.len);

        // for (self.tokens.items) |token| {
        //     token.print();
        // }
    }

    fn syntax_analysis(self: *@This()) !void {
        var parser = FrontEnd.Parser.init(self.arena_alloc, self.content, self.tokens.items, &self.program);
        try parser.parse();

        std.debug.print("\n\n", .{});
        self.program.print();
        std.debug.print("\n\n", .{});
    }

    fn semantic_analysis(self: *@This()) !void {
        try self.hoist();
    }

    fn hoist(self: *@This()) !void {
        _ = self;
    }

    pub fn deinit(self: *@This()) void {
        self.tokens.deinit();
        self.allocator.free(self.content);
        // self.allocator.destroy(self.program);
    }
};
