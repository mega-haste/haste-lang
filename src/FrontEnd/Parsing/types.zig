const std = @import("std");
const TokenMod = @import("../../Token.zig");
const AST = @import("../../AST.zig");
const Meta = @import("../../Meta.zig");
const ParserMod = @import("../Parser.zig");

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

pub fn parse_type(self: *Parser) ParserError!*AST.Type {
    const start = self.peek();
    const tp = AST.create_primitive(self.allocator, switch (self.peek().kind) {
        .Uint => AST.Primitive.UInt,
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
