﻿using System;
using System.Collections.Generic;
using System.IO;

namespace LuminoBuild.Tasks
{
    class BuildEngine_Linux : BuildTask
    {
        public override string CommandName => "BuildEngine_Linux";

        public override string Description => "BuildEngine_Linux";

        public override void Build(Builder builder)
        {
            var buildDir = Path.Combine(builder.LuminoBuildDir, "Linux-x86_64");
            var installDir = Path.Combine(builder.LuminoBuildDir, "CMakeInstallTemp", "Linux-x86_64");

            Directory.CreateDirectory(buildDir);
            Directory.SetCurrentDirectory(buildDir);

            var args = string.Format(
                "-DCMAKE_INSTALL_PREFIX=\"{0}\" -DLN_BUILD_TESTS=ON ../..",
                installDir);

            Utils.CallProcess("cmake", args);
            Utils.CallProcess("cmake", "--build . --target INSTALL");
        }
    }
}
