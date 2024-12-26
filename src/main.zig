const std = @import("std");
const Compiler = @import("./Compiler.zig").Compiler;
const ReporterMod = @import("./Reporter.zig");

const unicode = std.unicode;

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();

    const alloc = arena.allocator();

    const stdout_file = std.io.getStdOut().writer();
    // var bw = std.io.bufferedWriter(stdout_file);
    // const stdout = bw.writer();

    var compiler = Compiler.init(allocator, alloc, stdout_file);
    defer compiler.deinit();

    const cwd = try std.fs.path.join(allocator, &[_][]const u8{ try std.fs.realpathAlloc(alloc, "."), "main.haste" });
    // std.log.info("{s}", .{cwd});
    try compiler.compile(cwd);
}

test {
    // stdout is for the actual output of your application, for example if you
    // are implementing gzip, then only the compressed bytes should be sent to
    // stdout, not any debugging messages.
    const stdout_file = std.io.getStdOut().writer();
    var bw = std.io.bufferedWriter(stdout_file);
    const stdout = bw.writer();

    try stdout.print("Run `zig build test` to run the tests.\n", .{});

    try bw.flush(); // don't forget to flush!
}
