﻿using System;
using System.Collections.Generic;
using System.IO;

namespace LuminoBuild.Tasks
{
    class MakeNuGetPackage_Core : BuildTask
    {
        public override string CommandName => "MakeNuGetPackage_Core";

        public override string Description => "MakeNuGetPackage_Core";

        public override void Build(Builder builder)
        {
            // Build project
            foreach (var t in MakeVSProjects.Targets)
            {
                if (t.MSVCStaticRuntime == "OFF")
                {
                    BuildEngine_MSVC.BuildTarget(builder, t);
                }
            }


            var nugetroot = Path.Combine(builder.LuminoBuildDir, "NuGetPackage-LuminoCore");
            var nativeDir = Path.Combine(builder.LuminoBuildDir, "NuGetPackage-LuminoCore", "build", "native");
            Directory.CreateDirectory(nativeDir);

            var files = new List<(string src, string dst)>()
            {
                (
                    Path.Combine(builder.LuminoBuildDir, BuildEnvironment.CMakeTargetInstallDir, "MSVC2017-x86-MD-Debug", "LuminoCored.lib"),
                    Path.Combine(nativeDir, "lib", "MSVC2017-x86-MD", "Debug", "LuminoCore.lib")
                ),
                (
                    Path.Combine(builder.LuminoBuildDir, BuildEnvironment.CMakeTargetInstallDir, "MSVC2017-x86-MD-Release", "LuminoCore.lib"),
                    Path.Combine(nativeDir, "lib", "MSVC2017-x86-MD", "Release", "LuminoCore.lib")
                ),
                (
                    Path.Combine(builder.LuminoBuildDir, BuildEnvironment.CMakeTargetInstallDir, "MSVC2017-x64-MD-Debug", "LuminoCored.lib"),
                    Path.Combine(nativeDir, "lib", "MSVC2017-x64-MD", "Debug", "LuminoCore.lib")
                ),
                (
                    Path.Combine(builder.LuminoBuildDir, BuildEnvironment.CMakeTargetInstallDir, "MSVC2017-x64-MD-Release", "LuminoCore.lib"),
                    Path.Combine(nativeDir, "lib", "MSVC2017-x64-MD", "Release", "LuminoCore.lib")
                ),
                // nuget --------
                (
                    Path.Combine(builder.LuminoRootDir, "src", "LuminoCore", "NuGet", "Lumino.Core.props"),
                    Path.Combine(nativeDir, "Lumino.Core.props")
                ),
                (
                    Path.Combine(builder.LuminoRootDir, "src", "LuminoCore", "NuGet", "Lumino.Core.targets"),
                    Path.Combine(nativeDir, "Lumino.Core.targets")
                ),
                (
                    Path.Combine(builder.LuminoRootDir, "src", "LuminoCore", "NuGet", "Lumino.Core.nuspec"),
                    Path.Combine(nugetroot, "Lumino.Core.nuspec")
                ),
            };

            Directory.CreateDirectory(Path.Combine(nativeDir, "include"));
            Utils.CopyDirectory(
                Path.Combine(builder.LuminoRootDir, "include", "LuminoCore"),
                Path.Combine(nativeDir, "include", "LuminoCore"));
            File.Copy(
                Path.Combine(builder.LuminoRootDir, "include", "LuminoCore.hpp"),
                Path.Combine(nativeDir, "include", "LuminoCore.hpp"), true);

            foreach (var pair in files)
            {
                Directory.CreateDirectory(Path.GetDirectoryName(pair.dst));
                File.Copy(pair.src, pair.dst, true);
            }

            Directory.SetCurrentDirectory(nugetroot);
            Utils.CallProcess("nuget", "pack Lumino.Core.nuspec");
        }
    }
}
