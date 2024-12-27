const std = @import("std");

const TranslationUnit = @import("./TranslationUnit.zig").TranslationUnit;

const mem = std.mem;
const StringHashMap = std.StringHashMap;

pub const Compiler = struct {
    stdout: std.fs.File,
    allocator: mem.Allocator,
    arena_alloc: mem.Allocator,
    translation_units: StringHashMap(TranslationUnit),

    pub fn init(allocator: mem.Allocator, arena_alloc: mem.Allocator, stdout: std.fs.File) Compiler {
        return Compiler{ //
            .stdout = stdout,
            .allocator = allocator,
            .arena_alloc = arena_alloc,
            .translation_units = StringHashMap(TranslationUnit).init(allocator),
        };
    }

    pub fn compile(self: *@This(), path: []const u8) !void {
        var tu = try TranslationUnit.init(self.allocator, self.arena_alloc, path, self.stdout);
        try tu.frontend();
        try self.translation_units.put(path, tu);
    }

    pub fn deinit(self: *@This()) void {
        var itr = self.translation_units.iterator();
        while (itr.next()) |entry| {
            entry.value_ptr.deinit();
        }
    }
};
