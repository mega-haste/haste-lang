const std = @import("std");

const ExprMod = @import("./AST/Expr.zig");
const StmtMod = @import("./AST/Stmt.zig");
const TypeMod = @import("./AST/Type.zig");

const mem = std.mem;

pub const Expr = ExprMod.Expr;
pub const ExprNode = ExprMod.ExprNode;
pub const create_expr = ExprMod.create_expr;
pub const create_grouping = ExprMod.create_grouping;
pub const create_unary = ExprMod.create_unary;
pub const create_binary = ExprMod.create_binary;
pub const create_logical_binary = ExprMod.create_logical_binary;
pub const create_logical_unary = ExprMod.create_logical_unary;
pub const create_assignment = ExprMod.create_assignment;
pub const create_primary = ExprMod.create_primary;
pub const create_inline_if = ExprMod.create_inline_if;

pub const Stmt = StmtMod.Stmt;
pub const StmtNode = StmtMod.StmtNode;

pub const ItemList = std.ArrayList(Stmt);

pub const ProgramAST = struct { //
    items: ItemList,

    pub fn init(allocator: mem.Allocator) ProgramAST {
        return ProgramAST{ .items = ItemList.init(allocator) };
    }

    pub fn add_item(self: *@This(), item: Stmt) !void {
        self.items.append(item);
    }

    pub fn deinit(self: *const @This()) void {
        self.items.deinit();
    }
};
