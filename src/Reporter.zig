pub const ReportLocation = struct {};

pub const Reporter = struct {
    pub fn init() Reporter {
        return Reporter{};
    }

    pub fn deinit(self: *@This()) void {
        _ = self;
    }
};
