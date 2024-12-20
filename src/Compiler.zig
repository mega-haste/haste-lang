const std = @import("std");

const TranslationUnit = @import("./TranslationUnit.zig").TranslationUnit;
const Reporter = @import("./Reporter.zig").Reporter;

const mem = std.mem;
const StringHashMap = std.StringHashMap;

pub const Compiler = struct {
    allocator: mem.Allocator,
    arena_alloc: mem.Allocator,
    reporter: Reporter,
    translation_units: StringHashMap(TranslationUnit),

    pub fn init(allocator: mem.Allocator, arena_alloc: mem.Allocator) Compiler {
        return Compiler{ //
            .allocator = allocator,
            .arena_alloc = arena_alloc,
            .reporter = Reporter.init(),
            .translation_units = StringHashMap(TranslationUnit).init(allocator),
        };
    }

    pub fn compile(self: *@This(), path: []const u8) !void {
        var tu = try TranslationUnit.init(self.allocator, self.arena_alloc, path);
        try tu.frontend();
        try self.translation_units.put(path, tu);
    }

    pub fn deinit(self: *@This()) void {
        self.reporter.deinit();
        var itr = self.translation_units.iterator();
        while (itr.next()) |entry| {
            entry.value_ptr.deinit();
        }
    }
};
