const std = @import("std");
const TokenMod = @import("../Token.zig");
const AST = @import("../AST.zig");
const Meta = @import("../Meta.zig");

const Token = TokenMod.Token;
const TokenType = TokenMod.Type;
const TokenList = TokenMod.TokenList;

const Expr = AST.Expr;
const ExprNode = AST.ExprNode;

const Stmt = AST.Stmt;
const StmtNode = AST.StmtNode;

const mem = std.mem;
const debug = std.debug;

pub const ParserError = error{ UnexpectedToken, BadAllocation };

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
            const item = self.static_declaration() catch |err| {
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

    fn static_declaration(self: *@This()) ParserError!*StmtNode { //
        if (self.match(TokenType.Func)) |_| {
            return self.function();
        }

        return ParserError.UnexpectedToken;
    }

    fn declaration(self: *@This()) ParserError!void { //
        _ = self;
    }

    fn statement(self: *@This()) ParserError!void { //
        _ = self;
    }

    fn statement_expr(self: *@This()) ParserError!Stmt {
        const start = self.peek();
        const expr = try self.parse_expr();
        const end = self.previous();

        const stmt = AST.create_expr_stmt(self.allocator, expr) catch return ParserError.BadAllocation;
        return AST.init_stmt(start, end, .Inherited, stmt);
    }

    fn function(self: *@This()) ParserError!*StmtNode {
        const ident = try self.consume_iden("Expected an identifier of the function name.");
        _ = try self.consume(.OpenParen, "");
        var params = std.ArrayList(AST.ArgNode).init(self.allocator);
        if (!self.check(.CloseParen)) {
            while (true) {
                const param = try self.parse_param();
                params.append(param) catch return ParserError.BadAllocation;
                if (self.match(TokenType.Coma) == null) break;
            }
        }
        _ = try self.consume(.CloseParen, "Expected ')' as closing of function params.");
        params.shrinkAndFree(params.items.len);

        var return_type: ?*const AST.Type = null;
        if (self.match(TokenType.Colon)) |_| {
            return_type = try self.parse_type();
        }

        var body = std.ArrayList(AST.Stmt).init(self.allocator);
        _ = try self.consume(.Eq, "Expected '='.");
        body.append(try self.statement_expr()) catch return ParserError.BadAllocation;

        _ = try self.consume(.SemiColon, "Expected ';'.");

        const stmt = AST.create_func(self.allocator) catch return ParserError.BadAllocation;
        stmt.ItemFunc = .{ //
            .identifier = ident,
            .arguments = params.items,
            .return_type = return_type,
            .body = body.items,
        };

        return stmt;
    }

    fn parse_param(self: *@This()) ParserError!AST.ArgNode {
        const start = self.peek();
        const ident = try self.consume_iden("Expected an identifier for the name of the param.");
        _ = try self.consume(.Colon, "Expected ':' after the param name.");

        const tp = try self.parse_type();
        var default: ?*const AST.Expr = null;
        if (self.match(TokenType.Eq)) |_| {
            default = try self.parse_expr();
        }

        const end = self.previous();
        return AST.ArgNode{ //
            .start = start,
            .end = end,
            .ident = ident,
            .tp = tp,
            .default = default,
        };
    }

    fn parse_type(self: *@This()) ParserError!*AST.Type {
        const start = self.peek();
        const tp = AST.create_primitive(self.allocator, switch (self.peek().kind) {
            .Int => AST.Primitive.Int,
            .Float => AST.Primitive.Float,
            .Bool => AST.Primitive.Bool,
            .Void => AST.Primitive.Void,

            else => return ParserError.UnexpectedToken,
        }) catch return ParserError.BadAllocation;
        _ = self.advance();

        const end = self.previous();

        const res = AST.create_type(self.allocator, start, end, tp) catch return ParserError.BadAllocation;
        return res;
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

    fn consume_iden(self: *@This(), message: []const u8) !*const Token {
        if (!(self.check(.Ident) or self.check(.SpecialIdent))) {
            std.debug.print("{s}", .{message});
            return ParserError.UnexpectedToken;
        }
        return self.advance();
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
