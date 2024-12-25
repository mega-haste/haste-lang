const std = @import("std");
const TokenMod = @import("../Token.zig");
const AST = @import("../AST.zig");
const Meta = @import("../Meta.zig");
const Declaration = @import("./Parsing/declaration.zig");
const Expression = @import("./Parsing/expression.zig");
const Statement = @import("./Parsing/statement.zig");

const Token = TokenMod.Token;
const TokenType = TokenMod.Type;
const TokenList = TokenMod.TokenList;

const Expr = AST.Expr;
const ExprNode = AST.ExprNode;

const Stmt = AST.Stmt;
const StmtNode = AST.StmtNode;

const mem = std.mem;
const debug = std.debug;

pub const ParserError = error{ TooMuchArguments, UnexpectedToken, BadAllocation };

pub const Parser = struct {
    allocator: mem.Allocator,
    page_allocator: mem.Allocator,
    content: []const u8,
    tokens: []const Token,
    program: *AST.ProgramAST,
    index: usize = 0,

    pub fn init(allocator: mem.Allocator, content: []const u8, tokens: []const Token, program: *AST.ProgramAST) Parser {
        return Parser{ //
            .allocator = allocator,
            .page_allocator = std.heap.page_allocator,
            .content = content,
            .tokens = tokens,
            .program = program,
        };
    }

    pub fn parse(self: *@This()) ParserError!void {
        while (!self.is_at_end()) {
            const start = self.peek();
            const item = Declaration.static_declaration(self) catch |err| {
                if (err == ParserError.BadAllocation) return ParserError.BadAllocation;
                self.static_synchronize();
                continue;
            };
            const end = self.previous();

            const st = AST.init_stmt(start, end, .Inherited, item);
            self.program.add_item(st) catch return ParserError.BadAllocation;
        }
        self.program.items.shrinkAndFree(self.program.items.items.len);
    }

    fn static_synchronize(self: *@This()) void {
        _ = self.advance();
        while (!self.is_at_end()) {
            switch (self.peek().kind) {
                .Func, .NotFunc => return,
                else => {},
            }
            _ = self.advance();
        }
    }

    pub fn match(self: *@This(), comptime kinds: anytype) ?*const TokenMod.Token {
        const kinds_type = @TypeOf(kinds);
        if (comptime kinds_type == TokenMod.Type) {
            const kind = @as(TokenType, kinds);
            if (self.check(kind)) {
                return self.advance();
            }
            return null;
        } else {
            var entries: [kinds.len]TokenType = undefined;
            inline for (kinds, 0..) |value, i| {
                const v = @as(TokenType, value);
                entries[i] = v;
            }
            for (entries) |value| {
                if (self.check(value)) {
                    return self.advance();
                }
            }
            return null;
        }
    }

    pub fn check(self: *const @This(), kind: TokenType) bool {
        if (self.is_at_end()) return false;
        return self.peek().kind == kind;
    }

    pub fn is_at_end(self: *const @This()) bool {
        return self.tokens[self.index].kind == TokenType.EOF;
    }

    pub fn consume_iden(self: *@This(), message: []const u8) !*const Token {
        if (!(self.check(.Ident) or self.check(.SpecialIdent))) {
            std.debug.print("{s}", .{message});
            return ParserError.UnexpectedToken;
        }
        return self.advance();
    }

    pub fn consume(self: *@This(), kind: TokenType, message: []const u8) !*const Token {
        if (!self.check(kind)) {
            std.debug.print("{s}", .{message});
            return ParserError.UnexpectedToken;
        }
        return self.advance();
    }

    pub fn advance(self: *@This()) *const Token {
        if (!self.is_at_end()) self.index += 1;
        return &self.tokens[self.index - 1];
    }

    pub fn peek(self: *const @This()) *const Token {
        return &self.tokens[self.index];
    }

    pub fn previous(self: *const @This()) *const Token {
        return &self.tokens[self.index - 1];
    }
};
