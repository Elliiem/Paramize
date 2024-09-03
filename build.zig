const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "Parametrize",
        .target = target,
        .optimize = optimize,
    });

    exe.linkLibCpp();

    exe.addCSourceFiles(.{
        .files = &[_][]const u8{
            "src/main.cpp",
        },
        .flags = &[_][]const u8{
            "-std=c++23",
        },
    });

    exe.addIncludePath(b.path("include/"));

    b.installArtifact(exe);
}
