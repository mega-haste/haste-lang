const std = @import("std");
const TokenMod = @import("../../Token.zig");
const AST = @import("../../AST.zig");
const Meta = @import("../../Meta.zig");
const ParserMod = @import("../Parser.zig");
const Expression = @import("./expression.zig");
const _return = @import("./statements/_return.zig")._return;
const block = @import("./statements/block.zig").block;

const Parser = ParserMod.Parser;
const ParserError = ParserMod.ParserError;

const Token = TokenMod.Token;
const TokenType = TokenMod.Type;
const TokenList = TokenMod.TokenList;

const Expr = AST.Expr;
const ExprNode = AST.ExprNode;

const Stmt = AST.Stmt;
const StmtNode = AST.StmtNode;

const mem = std.mem;
const debug = std.debug;

pub fn statement(self: *Parser) ParserError!Stmt {
    const start = self.peek();
    const stmt: *const StmtNode = stmt_res: {
        if (self.match(TokenType.Return)) |_| {
            break :stmt_res try _return(self);
        } else if (self.match(TokenType.OpenCurlyParen)) |_| {
            break :stmt_res try block(self);
        }

        return statement_expr(self);
    };

    const end = self.previous();
    return AST.init_stmt(start, end, .Inherited, stmt);
}

pub fn statement_expr(self: *Parser) ParserError!Stmt {
    const start = self.peek();
    const expr = try Expression.parse_expr(self);
    _ = try self.consume(.SemiColon, "Expected `;` at the end of the expression.");
    const end = self.previous();

    const stmt = AST.create_expr_stmt(self.allocator, expr) catch return ParserError.BadAllocation;
    return AST.init_stmt(start, end, .Inherited, stmt);
}
