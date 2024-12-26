const std = @import("std");
const Compiler = @import("./Compiler.zig").Compiler;

const unicode = std.unicode;

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();

    const alloc = arena.allocator();

    var compiler = Compiler.init(allocator, alloc);
    defer compiler.deinit();

    const cwd = try std.fs.path.join(allocator, &[_][]const u8{ try std.fs.realpathAlloc(alloc, "."), "main.haste" });
    // std.log.info("{s}", .{cwd});
    try compiler.compile(cwd);
}

test {
    const str = "Wsup السلام عليكم";

    std.debug.print("str.len: {}, txt: {s}\n", .{ str.len, str });

    const uni_view = try unicode.Utf8View.init(str);
    var iter = uni_view.iterator();
    var prev_i: usize = iter.i;

    while (iter.nextCodepoint()) |b| {
        std.debug.print("b: {u}, i: {}, len: {}\n", .{ b, iter.i, iter.i - prev_i });
        prev_i = iter.i;
    }
}
