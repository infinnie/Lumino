﻿using System;
using System.Collections.Generic;
using System.IO;

namespace LuminoBuild.Tasks
{
    class MakeReleasePackage : BuildTask
    {
        public override string CommandName => "MakeReleasePackage";

        public override string Description => "MakeReleasePackage";

        public override void Build(Builder builder)
        {
            var tempInstallDir = Path.Combine(builder.LuminoBuildDir, BuildEnvironment.CMakeTargetInstallDir);

            string targetRootDir;
            if (Utils.IsWin32)
                targetRootDir = Path.Combine(builder.LuminoRootDir, "ReleasePackage.Win32");
            else
                targetRootDir = Path.Combine(builder.LuminoRootDir, "ReleasePackage.macOS");
            Directory.CreateDirectory(targetRootDir);

            string nativeEngineRoot = Path.Combine(targetRootDir, "Engine", "Native");
            Directory.CreateDirectory(nativeEngineRoot);

            // docs
            {
                // Readme
                Utils.GenerateFile(
                    Path.Combine(targetRootDir, "Readme.txt"),
                    Path.Combine(builder.LuminoPackageSourceDir, "Readme.txt.template"),
                    new Dictionary<string, string> { { "%%LuminoVersion%%", builder.VersionString } });
            }
            
            // C++ Engine (common)
            {
                File.Copy(
                    Path.Combine(builder.LuminoSourceDir, "LuminoSetup.cmake"),
                    Path.Combine(nativeEngineRoot, "lib", "LuminoSetup.cmake"), true);

                File.Copy(
                    Path.Combine(builder.LuminoExternalDir, "ImportExternalLibraries.cmake"),
                    Path.Combine(nativeEngineRoot, "lib", "ImportExternalLibraries.cmake"), true);

                File.WriteAllText(
                    Path.Combine(nativeEngineRoot, ".gitignore"),
                    "include/\nlib/\n");
            }

            var externalLibs = new string[]
            {
                "glad",
                "glslang",
                "libpng",
                "openal-soft",
                "SDL2",
                "SPIRV-Cross",
                "zlib",
            };
            
            // C++ Engine
            {
                // include files
                {
                    Utils.CopyDirectory(
                        Path.Combine(builder.LuminoRootDir, "include"),
                        Path.Combine(nativeEngineRoot, "include"));
                }

                // lib files
                foreach (var arch in BuildEnvironment.TargetArchs)
                {
                    if (Directory.Exists(Path.Combine(tempInstallDir, arch.SourceDirName)))   // copy if directory exists.
                    {
                        var externalInstallDir = Path.Combine(builder.LuminoBuildDir, arch.SourceDirName, "ExternalInstall");
                        var targetDir = Path.Combine(nativeEngineRoot, "lib", arch.DestDirName);

                        Utils.CopyDirectory(
                            Path.Combine(tempInstallDir, arch.SourceDirName, "lib"),
                            targetDir);

                        foreach (var lib in externalLibs)
                        {
                            var srcDir = Path.Combine(externalInstallDir, lib, "lib");
                            if (Directory.Exists(srcDir))   // copy if directory exists. openal-soft etc are optional.
                            {
                                Utils.CopyDirectory(
                                    srcDir,
                                    targetDir);
                            }
                        }
                    }
                }
            }

        }
    }
}
