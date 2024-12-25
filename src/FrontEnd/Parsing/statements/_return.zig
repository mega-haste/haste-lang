const std = @import("std");
const TokenMod = @import("../../../Token.zig");
const AST = @import("../../../AST.zig");
const Meta = @import("../../../Meta.zig");
const ParserMod = @import("../../Parser.zig");
const Expression = @import("../expression.zig");

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

pub fn _return(self: *Parser) ParserError!*StmtNode {
    const res = AST.create_return(self.allocator, null) catch return ParserError.BadAllocation;
    if (self.match(TokenType.SemiColon)) |_| {
        return res;
    }

    const expr = try Expression.parse_expr(self);
    _ = try self.consume(.SemiColon, "Expected `;` at the end of 'return'.");

    res.Return.expr = expr;
    return res;
}
