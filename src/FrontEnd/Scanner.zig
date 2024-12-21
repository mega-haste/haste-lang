const std = @import("std");
const TokenMod = @import("../Token.zig");
const DataStructure = @import("../DataStructure.zig");

const Token = TokenMod.Token;
const TokenType = TokenMod.Type;
const TokenList = TokenMod.TokenList;
const StringView = DataStructure.StringView;

const mem = std.mem;
const StaticStringMap = std.StaticStringMap;

// I'm in love with zig <3
const keywords = create_keywords_map(.{
    .Int,
    .Float,
    .Bool,
    .Void,

    .True,
    .False,

    .Not,
    .And,
    .Or,

    .Public,
    .Private,
    .NotFunc, // Automaticly translates to '!func'
    .Func,
    .Let,
    .Mut,

    .If,
    .NotIf,
    .Then,
    .Else,
});

fn create_keywords_map(comptime keywords_list: anytype) StaticStringMap(TokenType) {
    var entries: [keywords_list.len]struct { @"0": []const u8, @"1": TokenType } = undefined;
    inline for (keywords_list, 0..) |value, index| {
        const name: []const u8 = std.meta.fieldInfo(TokenType, value).name;
        if (name.len > 3) {
            const key =
                if (mem.eql(u8, name[0..3], "Not"))
                ("!" ++ &[1]u8{name[3] + 32} ++ name[4..])
            else
                &[_]u8{name[0] + 32} ++ name[1..];
            entries[index] = .{ .@"0" = key, .@"1" = value };
        } else {
            const key = &[_]u8{name[0] + 32} ++ name[1..];
            entries[index] = .{ .@"0" = key, .@"1" = value };
        }
    }

    return StaticStringMap(TokenType).initComptime(entries);
}

pub const ScannerError = error{InvalidCharacter};

pub const Scanner = struct {
    line: usize = 1,
    column: usize = 1,
    current: usize = 0,
    start: usize = 0,
    tokens: *TokenList,
    content: []const u8,

    const Ident = struct {
        lexem: StringView,
        scanner: *Scanner,

        pub fn revert(self: *@This()) void {
            self.scanner.current -= self.lexem.get_len();
        }
    };

    pub fn init(tokens: *TokenList, content: []const u8) Scanner {
        return .{ .tokens = tokens, .content = content };
    }

    pub fn scan(self: *@This()) !void {
        var has_error: ?anyerror = null;
        while (!self.is_at_end()) {
            self.start = self.current;
            self.scan_lexem() catch |err| {
                has_error = err;
                std.debug.print("InvalidCharacter: '{c}'\n", .{self.previous()});
            };
            self.column += self.current - self.start;
        }

        try self.tokens.append(Token.create(TokenType.EOF, StringView.from_string(""), 0, 0, 0, 0));
        if (has_error) |err| {
            return err;
        }
    }

    fn scan_lexem(self: *@This()) !void {
        const ch = self.advance();
        switch (ch) {
            ';' => try self.add_token(.SemiColon),
            ':' => try self.add_token(.Colon),
            ',' => try self.add_token(.Coma),
            '.' => try self.add_token(.Dot),

            '(' => try self.add_token(.OpenParen),
            ')' => try self.add_token(.CloseParen),
            '{' => try self.add_token(.OpenCurlyParen),
            '}' => try self.add_token(.CloseCurlyParen),
            '[' => try self.add_token(.OpenSquareParen),
            ']' => try self.add_token(.CloseSquareParen),

            '-' => try self.add_token(.Minus),
            '+' => try self.add_token(.Plus),
            '*' => {
                if (self.match('*')) |_| {
                    try self.add_token(.DoubleStar);
                } else {
                    try self.add_token(.Star);
                }
            },
            '/' => try self.add_token(.FSlash),
            '\\' => try self.add_token(.BSlash),
            '%' => try self.add_token(.PercentSign),

            '=' => {
                if (self.match('=')) |_| {
                    try self.add_token(.EqEq);
                } else {
                    try self.add_token(.Eq);
                }
            },
            '!' => {
                if (self.match('=')) |_| {
                    try self.add_token(.NotEq);
                } else if (Scanner.is_alpha(self.peek())) {
                    var next_ident = try self.get_next_ident();
                    if (keywords.get(next_ident.lexem.data)) |kind| {
                        try self.add_token(kind);
                    } else {
                        next_ident.revert();
                        self.current += 1;
                        return ScannerError.InvalidCharacter;
                    }
                } else {
                    return ScannerError.InvalidCharacter;
                }
            },
            '<' => {
                if (self.match('=')) |_| {
                    try self.add_token(.LessThanEq);
                } else {
                    try self.add_token(.LessThan);
                }
            },
            '>' => {
                if (self.match('=')) |_| {
                    try self.add_token(.GreaterThanEq);
                } else {
                    try self.add_token(.GreaterThan);
                }
            },

            '"' => try self.string(),
            '@' => try self.macro_use(),

            '\n' => {
                self.line += 1;
                self.column = 0;
            },
            ' ', '\t', '\r' => {},
            '#' => try self.comment(),

            else => {
                if (Scanner.is_digit(ch)) {
                    try self.number();
                } else if (Scanner.is_alpha(ch)) {
                    try self.ident();
                } else {
                    return ScannerError.InvalidCharacter;
                }
            },
        }
    }

    fn string(self: *@This()) !void {
        while (self.peek() != '"')
            _ = self.advance();

        _ = self.advance();

        try self.add_token(.StringLit);
    }

    fn macro_use(self: *@This()) !void {
        if (is_alpha(self.peek())) {
            _ = try self.get_next_ident();
            try self.add_token(.MacroUse);
            return;
        }
        return ScannerError.InvalidCharacter;
    }

    fn comment(self: *@This()) !void {
        while (self.peek() != '\n') {
            if (self.is_at_end())
                return ScannerError.InvalidCharacter;
            _ = self.advance();
        }
        _ = self.advance();
        self.line += 1;
        self.column = 0;
    }

    fn number(self: *@This()) !void {
        var kind = TokenType.IntLit;
        while (Scanner.is_digit(self.peek())) {
            _ = self.advance();
        }

        if (self.peek() == '.') {
            kind = .FloatLit;
            _ = self.advance();
            while (Scanner.is_digit(self.peek()))
                _ = self.advance();
        }

        try self.add_token(kind);
    }

    fn ident(self: *@This()) !void {
        while (Scanner.is_alpha_num(self.peek())) {
            _ = self.advance();
        }

        const current_lexem = try self.get_lexem();
        const kind = keywords.get(current_lexem.data) orelse .Ident;
        try self.add_token(kind);
    }

    fn get_next_ident(self: *@This()) !Ident {
        while (Scanner.is_alpha_num(self.peek())) {
            _ = self.advance();
        }

        const current_lexem = try self.get_lexem();
        return Ident{
            .lexem = current_lexem,
            .scanner = self,
        };
    }

    fn add_token(self: *const @This(), kind: TokenType) !void {
        try self.tokens.append(Token.create(kind, try self.get_lexem(), self.line, self.column, self.start, self.current));
    }

    fn is_digit(ch: u8) bool {
        return ch >= '0' and ch <= '9';
    }

    fn is_alpha(ch: u8) bool {
        return (ch >= 'a' and ch <= 'z') or ch == '_' or ch == '$';
    }

    fn is_alpha_num(ch: u8) bool {
        return Scanner.is_digit(ch) or Scanner.is_alpha(ch);
    }

    fn match(self: *@This(), ch: u8) ?u8 {
        if (self.is_at_end()) return null;
        if (self.content[self.current] != ch) return null;

        return self.advance();
    }

    fn get_lexem(self: *const @This()) !StringView {
        return StringView.from_slice(self.content, self.start, self.current - self.start);
    }

    fn previous(self: *const @This()) u8 {
        return self.content[self.current - 1];
    }

    fn peek(self: *const @This()) u8 {
        if (self.is_at_end()) return 0x0;
        return self.content[self.current];
    }

    fn advance(self: *@This()) u8 {
        if (self.is_at_end()) return 0x0;
        self.current += 1;
        return self.content[self.current - 1];
    }

    fn is_at_end(self: *const @This()) bool {
        return self.current >= self.content.len or self.content[self.current] == 0x0;
    }
};
