const std = @import("std");

fn stripAnsi(reader: anytype, writer: anytype) !void {
    const State = enum {
        Normal,
        Escape,
        Sequence,
    };

    var state = State.Normal;

    while (reader.readByte()) |byte| {
        switch (state) {
            .Normal => {
                if (byte == 0x1b) {
                    state = .Escape;
                } else {
                    try writer.writeByte(byte);
                }
            },
            .Escape => {
                if (byte == '[') {
                    state = .Sequence;
                } else {
                    try writer.writeByte(0x1b);
                    try writer.writeByte(byte);
                    state = .Normal;
                }
            },
            .Sequence => {
                // The final byte of a CSI sequence is in the range 0x40-0x7E.
                // Keep consuming bytes until we find one.
                if (byte >= 0x40 and byte <= 0x7e) {
                    state = .Normal;
                }
            },
        }
    } else |err| switch (err) {
        error.EndOfStream => {},
        else => return err,
    }
}

pub fn main() !void {
    var buffered_stdout = std.io.bufferedWriter(std.io.getStdOut().writer());
    const stdout = buffered_stdout.writer();

    defer buffered_stdout.flush() catch {};

    const gpa = std.heap.page_allocator;
    const args = try std.process.argsAlloc(gpa);
    defer std.process.argsFree(gpa, args);

    if (args.len < 2) {
        var buffered_stdin = std.io.bufferedReader(std.io.getStdIn().reader());
        const stdin = buffered_stdin.reader();
        try stripAnsi(stdin, stdout);
    } else {
        for (args[1..]) |path| {
            const file = std.fs.cwd().openFile(path, .{}) catch |err| {
                std.debug.print("Error opening file '{s}': {s}\n", .{ path, @errorName(err) });
                continue;
            };
            defer file.close();

            var buffered_file_reader = std.io.bufferedReader(file.reader());
            const reader = buffered_file_reader.reader();
            try stripAnsi(reader, stdout);
        }
    }
}

test "removes a complex ansi sequence" {
    const allocator = std.testing.allocator;

    const input = "before \x1b[1;31mhello\x1b[0m after";
    const expected = "before hello after";

    var input_buffer = std.io.fixedBufferStream(input);
    var output_buffer = std.ArrayList(u8).init(allocator);
    defer output_buffer.deinit();

    const writer = output_buffer.writer();
    try stripAnsi(input_buffer.reader(), writer);

    try std.testing.expectEqualStrings(expected, output_buffer.items);
}

test "handles non-ansi escape sequence" {
    const allocator = std.testing.allocator;

    const input = "this is not a sequence: \x1b_hello";
    const expected = "this is not a sequence: \x1b_hello";

    var input_buffer = std.io.fixedBufferStream(input);
    var output_buffer = std.ArrayList(u8).init(allocator);
    defer output_buffer.deinit();

    const writer = output_buffer.writer();
    try stripAnsi(input_buffer.reader(), writer);

    try std.testing.expectEqualStrings(expected, output_buffer.items);
}
