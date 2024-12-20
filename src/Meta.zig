pub fn is_array_of(@"type": type, el_type: type) bool {
    const type_info = @typeInfo(@"type");
    switch (type_info) {
        .Array => |v| {
            return v.child == el_type;
        },
        else => return false,
    }
}
