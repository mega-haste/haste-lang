const std = @import("std");
const TokenMod = @import("../../Token.zig");
const AST = @import("../../AST.zig");
const Meta = @import("../../Meta.zig");
const ParserMod = @import("../Parser.zig");
const Statemet = @import("./statement.zig");
const function = @import("./declarations/function.zig").function;
const let = @import("./declarations/let.zig").let;

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

pub fn static_declaration(self: *Parser) ParserError!*StmtNode { //
    if (self.match(TokenType.Func)) |_| {
        return function(self);
    }

    return ParserError.UnexpectedToken;
}

pub fn declaration(self: *Parser) ParserError!Stmt {
    const start = self.peek();
    const dec: *const StmtNode = dec_res: {
        if (self.match(TokenType.Let)) |_| {
            break :dec_res try let(self);
        } else if (self.match(TokenType.Func)) |_| {
            break :dec_res try function(self);
        }

        return Statemet.statement(self);
    };

    const end = self.previous();
    return AST.init_stmt(start, end, .Inherited, dec);
}
