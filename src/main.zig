const std = @import("std");
const Compiler = @import("./Compiler.zig").Compiler;

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();

    const alloc = arena.allocator();

    var compiler = Compiler.init(allocator, alloc);
    defer compiler.deinit();

    const cwd = try std.fs.path.join(allocator, &[_][]const u8{ try std.fs.realpathAlloc(alloc, "."), "main.haste" });
    std.log.info("{s}", .{cwd});
    try compiler.compile(cwd);
}
