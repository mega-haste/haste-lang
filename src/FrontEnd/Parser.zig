const std = @import("std");
const TokenMod = @import("../Token.zig");
const AST = @import("../AST.zig");
const Meta = @import("../Meta.zig");

const Token = TokenMod.Token;
const TokenType = TokenMod.Type;
const TokenList = TokenMod.TokenList;

const Expr = AST.Expr;
const ExprNode = AST.ExprNode;

const mem = std.mem;
const debug = std.debug;

pub const ParserError = error{ UnexpectedToken, BadAllocation };

pub const Parser = struct {
    allocator: mem.Allocator,
    content: []const u8,
    tokens: []const Token,
    program: *AST.ProgramAST,
    index: usize = 0,

    pub fn init(allocator: mem.Allocator, content: []const u8, tokens: []const Token, program: *AST.ProgramAST) Parser {
        return Parser{ //
            .allocator = allocator,
            .content = content,
            .tokens = tokens,
            .program = program,
        };
    }

    pub fn parse(self: *@This()) ParserError!void {
        self.program.expr = try self.parse_expr();
    }

    fn parse_expr(self: *@This()) ParserError!*Expr {
        return self.parse_inline_if();
    }

    fn parse_inline_if(self: *@This()) ParserError!*Expr {
        if (self.match(TokenType.If) == null) return self.parse_assignment();
        const start = self.previous();
        const condition = try self.parse_expr();

        const then_token = try self.consume(.Then, "Expected 'then'.");
        const consequent = try self.parse_expr();

        const else_token = try self.consume(.Else, "Expected 'else'.");
        const alternate = try self.parse_expr();

        const end = self.previous();
        const tmp = AST.create_inline_if(self.allocator, condition, consequent, alternate) catch return ParserError.BadAllocation;
        tmp.InlineIf.if_token = start;
        tmp.InlineIf.then_token = then_token;
        tmp.InlineIf.else_token = else_token;

        const result = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;

        return result;
    }

    fn parse_assignment(self: *@This()) ParserError!*Expr {
        var start = self.peek();
        var left = try self.parse_or();

        while (self.match(TokenType.Eq)) |op| {
            const right = try self.parse_expr();
            const tmp = AST.create_assignment(self.allocator, left, op, right) catch return ParserError.BadAllocation;
            const end = self.previous();
            left = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;

            start = self.peek();
        }

        return left;
    }

    fn parse_or(self: *@This()) ParserError!*Expr {
        var start = self.peek();
        var left = try self.parse_and();

        while (self.match(TokenType.Or)) |op| {
            const right = try self.parse_and();
            const tmp = AST.create_logical_binary(self.allocator, left, op, right) catch return ParserError.BadAllocation;
            const end = self.previous();
            left = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;

            start = self.peek();
        }

        return left;
    }

    fn parse_and(self: *@This()) ParserError!*Expr {
        var start = self.peek();
        var left = try self.parse_relational();

        while (self.match(TokenType.And)) |op| {
            const right = try self.parse_relational();
            const tmp = AST.create_logical_binary(self.allocator, left, op, right) catch return ParserError.BadAllocation;
            const end = self.previous();
            left = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;

            start = self.peek();
        }

        return left;
    }

    fn parse_relational(self: *@This()) ParserError!*Expr {
        var start = self.peek();
        var left = try self.parse_addition();

        while (self.match(.{ .EqEq, .NotEq, .LessThan, .LessThanEq, .GreaterThan, .GreaterThanEq })) |op| {
            const right = try self.parse_addition();
            const tmp = AST.create_binary(self.allocator, left, op, right) catch return ParserError.BadAllocation;
            const end = self.previous();
            left = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;

            start = self.peek();
        }

        return left;
    }

    fn parse_addition(self: *@This()) ParserError!*Expr {
        var start = self.peek();
        var left = try self.parse_factor();

        while (self.match(.{ .Plus, .Minus })) |op| {
            const right = try self.parse_factor();
            const tmp = AST.create_binary(self.allocator, left, op, right) catch return ParserError.BadAllocation;
            const end = self.previous();
            left = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;

            start = self.peek();
        }

        return left;
    }

    fn parse_factor(self: *@This()) ParserError!*Expr {
        var start = self.peek();
        var left = try self.parse_power();

        while (self.match(.{ .Star, .FSlash, .PercentSign })) |op| {
            const right = try self.parse_power();
            const tmp = AST.create_binary(self.allocator, left, op, right) catch return ParserError.BadAllocation;
            const end = self.previous();
            left = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;

            start = self.peek();
        }

        return left;
    }

    fn parse_power(self: *@This()) ParserError!*Expr {
        var start = self.peek();
        var left = try self.parse_unary();

        while (self.match(TokenType.DoubleStar)) |op| {
            const right = try self.parse_unary();
            const tmp = AST.create_binary(self.allocator, left, op, right) catch return ParserError.BadAllocation;
            const end = self.previous();
            left = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;

            start = self.peek();
        }

        return left;
    }

    fn parse_unary(self: *@This()) ParserError!*Expr {
        const start = self.peek();
        while (self.match(.{ .Not, .Minus })) |op| {
            const right = try self.parse_unary();
            var tmp: *ExprNode = undefined;
            const end = self.previous();

            if (op.kind == .Not) {
                tmp = AST.create_logical_unary(self.allocator, op, right) catch return ParserError.BadAllocation;
            } else {
                tmp = AST.create_unary(self.allocator, op, right) catch return ParserError.BadAllocation;
            }

            const res = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;
            return res;
        }

        return self.parse_primary();
    }

    fn parse_primary(self: *@This()) ParserError!*Expr {
        const start = self.peek();
        if (self.match(.{ .True, .False, .Ident, .SpecialIdent, .StringLit, .FloatLit, .IntLit })) |token| {
            const tmp = AST.create_primary(self.allocator, token) catch return ParserError.BadAllocation;
            const end = self.previous();
            const res = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;
            return res;
        }

        if (self.match(TokenType.OpenParen)) |_| {
            const expr: *Expr = try self.parse_expr();
            const end = try self.consume(.CloseParen, "Expected ')' as closing.");
            const tmp = AST.create_grouping(self.allocator, expr) catch return ParserError.BadAllocation;
            const res = AST.create_expr(self.allocator, start, end, tmp) catch return ParserError.BadAllocation;
            return res;
        }

        if (self.match(TokenType.If)) |_| {
            self.index -= 1;
            return self.parse_inline_if();
        }

        debug.print("Got '{s}'.\n", .{self.peek().lexem.data});
        return ParserError.UnexpectedToken;
    }

    fn match(self: *@This(), comptime kinds: anytype) ?*const TokenMod.Token {
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

    fn check(self: *const @This(), kind: TokenType) bool {
        if (self.is_at_end()) return false;
        return self.peek().kind == kind;
    }

    fn is_at_end(self: *const @This()) bool {
        return self.tokens[self.index].kind == TokenType.EOF;
    }

    fn consume(self: *@This(), kind: TokenType, message: []const u8) !*const Token {
        if (!self.check(kind)) {
            std.debug.print("{s}", .{message});
            return ParserError.UnexpectedToken;
        }
        return self.advance();
    }

    fn advance(self: *@This()) *const Token {
        if (!self.is_at_end()) self.index += 1;
        return &self.tokens[self.index - 1];
    }

    fn peek(self: *const @This()) *const Token {
        return &self.tokens[self.index];
    }

    fn previous(self: *const @This()) *const Token {
        return &self.tokens[self.index - 1];
    }
};
