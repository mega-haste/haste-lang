const std = @import("std");
const TokenMod = @import("../../../Token.zig");
const AST = @import("../../../AST.zig");
const Meta = @import("../../../Meta.zig");
const ParserMod = @import("../../Parser.zig");
const Declaration = @import("../declaration.zig");

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

pub fn block(self: *Parser) ParserError!*StmtNode {
    var stmts = std.ArrayList(Stmt).init(self.page_allocator);
    while (!self.check(TokenType.CloseCurlyParen)) {
        stmts.append(try Declaration.declaration(self)) catch return ParserError.BadAllocation;
    }
    _ = try self.consume(.CloseCurlyParen, "Expected `}` at the end of the block.");

    stmts.shrinkAndFree(stmts.items.len);

    const res = AST.create_block(self.allocator, stmts.items) catch return ParserError.BadAllocation;
    return res;
}
