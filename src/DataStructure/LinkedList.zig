const std = @import("std");

const mem = std.mem;

pub fn LinkedList(T: type) type {
    return struct {
        pub const Node = struct {
            value: T,
            prev: ?*Node,
            next: ?*Node,
        };

        pub const Iterator = struct {
            current: ?*Node = null,

            pub fn next(self: *@This()) ?*Node {
                if (self.current) |node| {
                    const value = node;
                    self.current = node.next;
                    return value;
                } else {
                    return null;
                }
            }
        };

        head: ?*Node = null,
        tail: ?*Node = null,
        len: usize = 0,
        allocator: mem.Allocator,

        pub fn init(allocator: mem.Allocator) @This() {
            return @This(){ .allocator = allocator };
        }

        pub fn add(self: *@This(), value: T) !void {
            const node = try self.allocator.create(Node);
            node.* = Node{
                .value = value,
                .prev = self.tail,
                .next = null,
            };

            if (self.tail) |tail_node| {
                tail_node.next = node;
            } else {
                self.head = node;
            }
            self.tail = node;
            self.len += 1;
        }

        pub fn remove(self: *@This(), node: *Node) void {
            if (node.prev) |prev_node| {
                prev_node.next = node.next;
            } else {
                self.head = node.next;
            }

            if (node.next) |next_node| {
                next_node.prev = node.prev;
            } else {
                self.tail = node.prev;
            }

            self.allocator.destroy(node);
            self.len -= 1;
        }

        pub fn iter(self: *const @This()) Iterator {
            return Iterator{ .current = self.head };
        }

        pub fn iterate(self: *@This(), callback: fn (*Node) void) void {
            var current = self.head;
            while (current) |node| {
                callback(node);
                current = node.next;
            }
        }

        pub fn clear(self: *@This()) void {
            var current = self.head;
            while (current) |node| {
                const next = node.next;
                self.allocator.destroy(node);
                current = next;
            }
            self.head = null;
            self.tail = null;
            self.len = 0;
        }

        pub fn deinit(self: *@This()) void {
            self.clear();
        }
    };
}
