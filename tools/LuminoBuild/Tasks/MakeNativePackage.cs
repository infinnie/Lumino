﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace LuminoBuild.Tasks
{
    class MakeNativePackage : BuildTask
    {
        public override string CommandName => "MakeNativePackage";

        //public override List<string> Dependencies => new List<string>() { "BuildEngine_MSVC" };

        public bool fileMoving = false;

        public override void Build(Builder builder)
        {
            string destinationRootDir = Path.Combine(builder.LuminoBuildDir, builder.LocalPackageName);
            string destinationToolDir = Path.Combine(destinationRootDir, "Tools");
            Directory.CreateDirectory(destinationRootDir);
            Directory.CreateDirectory(destinationToolDir);

            foreach (var target in BuildEnvironment.Targets)
            {
                string sourceBuildDir = Path.Combine(builder.LuminoBuildDir, target.Name);
                if (Directory.Exists(sourceBuildDir))
                {
                    Console.WriteLine($"{target.Name}...");

                    string destinationEngineRoot = Path.Combine(destinationRootDir, "Engine", target.Name);
                    string destinationEngineIncludeDir = Path.Combine(destinationEngineRoot, "include");
                    string destinationEngineLibDir = Path.Combine(destinationEngineRoot, "lib");
                    Directory.CreateDirectory(destinationEngineRoot);
                    Directory.CreateDirectory(destinationEngineIncludeDir);
                    Directory.CreateDirectory(destinationEngineLibDir);

                    string engineInstallDir = Path.Combine(builder.LuminoBuildDir, target.Name, BuildEnvironment.EngineInstallDirName);

                    // Engine include files
                    {
                        Utils.CopyDirectory(
                            Path.Combine(engineInstallDir, "include"),
                            Path.Combine(destinationEngineIncludeDir));
                    }

                    // Engine libs
                    {
                        var srcDir = Path.Combine(engineInstallDir, "lib");
                        Utils.CopyDirectory(srcDir, destinationEngineLibDir, recursive: false);
                        if (fileMoving)
                            Directory.Delete(srcDir, true); // FIXME: CI サーバのストレージ不足対策
                    }

                    // External libs
                    var externalInstallDir = Path.Combine(builder.LuminoBuildDir, target.Name, "ExternalInstall");
                    foreach (var dir in Directory.EnumerateDirectories(externalInstallDir))
                    {
                        var srcDir = Path.Combine(dir, "lib");
                        if (Directory.Exists(srcDir))   // copy if directory exists. openal-soft etc are optional.
                        {
                            Console.WriteLine($"Copy {srcDir} to {destinationEngineLibDir}");

                            foreach (var lib in Directory.EnumerateFiles(srcDir, target.LibraryExt, SearchOption.TopDirectoryOnly))
                            {
                                if (fileMoving)
                                    File.Move(lib, Path.Combine(destinationEngineLibDir, Path.GetFileName(lib)));
                                else
                                    File.Copy(lib, Path.Combine(destinationEngineLibDir, Path.GetFileName(lib)), true);
                            }
                        }
                    }

                    // cmake
                    {
                        Utils.CopyFile2(
                            Path.Combine(builder.LuminoRootDir, "src", "LuminoConfig.cmake"),
                            Path.Combine(destinationEngineRoot, "LuminoConfig.cmake"));
                        Utils.CopyFile2(
                            Path.Combine(builder.LuminoRootDir, "src", "LuminoCommon.cmake"),
                            Path.Combine(destinationEngineRoot, "LuminoCommon.cmake"));
                        Utils.CopyFile2(
                            Path.Combine(builder.LuminoRootDir, "external", "ImportExternalLibraries.cmake"),
                            Path.Combine(destinationEngineRoot, "ImportExternalLibraries.cmake"));
                    }
                }
            }

            // Tools
            {
                Console.WriteLine($"Copy tools...");

                if (Utils.IsWin32)
                {
                    var engineInstallDir = Path.Combine(builder.LuminoBuildDir, "MSVC2019-x64-MT", BuildEnvironment.EngineInstallDirName);
                    var sourceBinDir = Path.Combine(engineInstallDir, "bin");
                    if (Directory.Exists(sourceBinDir))
                    {
                        Utils.CopyDirectory(sourceBinDir, destinationToolDir, pattern: "*.exe");
                        Utils.DownloadFile(BuildEnvironment.VSWhereUrl, Path.Combine(destinationToolDir, "vswhere.exe"));
                    }
                }
                else if (Utils.IsMac)
                {
                    var engineInstallDir = Path.Combine(builder.LuminoBuildDir, "macOS", BuildEnvironment.EngineInstallDirName);
                    var sourceBinDir = Path.Combine(engineInstallDir, "bin");

                    if (Directory.Exists(sourceBinDir))
                    {
                        Utils.CopyDirectory(sourceBinDir, destinationToolDir);

                        var setupFile = Path.Combine(destinationToolDir, "setup.sh");
                        File.Copy(Path.Combine(builder.LuminoRootDir, "Tools", "PackageSource", "macOS", "setup.sh"), setupFile, true);
                        Utils.chmod(setupFile, Utils.S_0755);
                    }
                }

                // Templates
                {
                    Utils.CopyDirectory(
                        Path.Combine(builder.LuminoToolsDir, "ProjectTemplates"),
                        Path.Combine(builder.LuminoBuildDir, "", "ProjectTemplates"));
                }
            }

            // Docs
            {
                Console.WriteLine($"Copy docs...");

                // Readme
                Utils.GenerateFile(
                    Path.Combine(destinationRootDir, "Readme.txt"),
                    Path.Combine(builder.LuminoPackageSourceDir, "Readme.txt.template"),
                    new Dictionary<string, string> { { "%%LuminoVersion%%", builder.VersionString } });
            }

            // Engine assets
            {
                var reposDir = Path.Combine(builder.LuminoBuildDir, "ExternalSource");
                using (var cd = CurrentDir.Enter(reposDir))
                {
                    if (!Directory.Exists("noto-emoji"))
                    {
                        Utils.CallProcess("git", "clone --depth 1 -b v2019-11-19-unicode12 https://github.com/googlefonts/noto-emoji.git noto-emoji");
                        File.Copy("noto-emoji/fonts/NotoColorEmoji.ttf", Path.Combine(builder.LuminoToolsDir, "EngineResources", "NotoColorEmoji.ttf"), true);
                    }
                }

                Utils.CopyDirectory(
                    Path.Combine(builder.LuminoToolsDir, "EngineResources"),
                    Path.Combine(destinationToolDir, "EngineResources"));
            }
        }

        public static void CopyEngineLibs(Builder builder, string tempInstallDir, string nativeEngineRoot, bool fileMoving)
        {
#if false
            foreach (var arch in BuildEnvironment.TargetArchs)
            {
                if (Directory.Exists(Path.Combine(tempInstallDir, arch.SourceDirName)))   // copy if directory exists.
                {
                    var targetDir = Path.Combine(nativeEngineRoot, "lib", arch.DestDirName);

                    // Engine libs
                    {
                        var srcDir = Path.Combine(tempInstallDir, arch.SourceDirName, "lib");

                        Console.WriteLine($"Copy {srcDir} to {targetDir}");
                        Utils.CopyDirectory(srcDir, targetDir);
                        if (fileMoving)
                            Directory.Delete(srcDir, true); // FIXME: CI サーバのストレージ不足対策

                        // cmake
                        var cmakeDir = Path.Combine(tempInstallDir, arch.SourceDirName, "cmake");
                        if (Directory.Exists(cmakeDir))
                            Utils.CopyDirectory(cmakeDir, targetDir);
                    }

                    // External libs
                    var externalInstallDir = Path.Combine(builder.LuminoBuildDir, arch.SourceDirName, "ExternalInstall");
                    foreach (var lib in externalLibs)
                    {
                        var srcDir = Path.Combine(externalInstallDir, lib, "lib");
                        if (Directory.Exists(srcDir))   // copy if directory exists. openal-soft etc are optional.
                        {
                            Console.WriteLine($"Copy {srcDir} to {targetDir}");
                            Utils.CopyDirectory(srcDir, targetDir);

                            if (fileMoving)
                                Directory.Delete(srcDir, true); // FIXME: CI サーバのストレージ不足対策
                        }
                    }

                    // .pdb
                    // CMake では static library の PDB 出力先をコントロールできない。https://cmake.org/cmake/help/v3.1/prop_tgt/PDB_OUTPUT_DIRECTORY.html
                    // そのためビルドスクリプト側でコントロールする。
                    // 以下、パスに "Debug" を含む者のうち、lib と同じ名前の pdb ファイルをコピーする。
                    if (arch.PdbCopy)
                    {
                        var libfiles = Directory.GetFiles(targetDir, "*.lib", SearchOption.TopDirectoryOnly);
                        var libnames = new HashSet<string>(libfiles.Select(x => Path.GetFileNameWithoutExtension(x)));
                        var files1 = Directory.GetFiles(Path.Combine(builder.LuminoBuildDir, arch.SourceDirName), "*.pdb", SearchOption.AllDirectories);
                        foreach (var file in files1)
                        {
                            if (file.Contains("Debug") && libnames.Contains(Path.GetFileNameWithoutExtension(file)))
                            {
                                // FIXME: CI サーバのストレージ不足対策
                                if (fileMoving)
                                    File.Move(file, Path.Combine(targetDir, Path.GetFileName(file)));
                                else
                                    File.Copy(file, Path.Combine(targetDir, Path.GetFileName(file)), true);
                                Console.WriteLine(file);
                            }
                        }
                    }
                }
            }
#endif
        }
    }
}
