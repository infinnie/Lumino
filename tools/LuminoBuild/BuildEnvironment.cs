﻿using System;
using System.Collections.Generic;
using System.IO;

namespace LuminoBuild
{
    public class TargetArch
    {
        public string SourceDirName { get; set; }
        public string DestDirName { get; set; }
        public bool PdbCopy { get; set; } = false;
    }

    public class BuildEnvironment
    {
        public const string VSWhereUrl = @"https://github.com/Microsoft/vswhere/releases/download/2.5.2/vswhere.exe";

        public const string CMakeTargetInstallDir = "CMakeInstallTemp";
        public const string emsdkVer = "sdk-1.38.12-64bit";
        public const string emVer = "1.38.12";

        public static string BuildToolsDir { get; set; }

        public static string EmsdkDir { get; set; }
        public static string EmscriptenDir { get; set; }
        public static string emcmake { get; set; }


        public const string AndroidTargetPlatform = "android-24";
        public static bool AndroidStudioFound { get; set; }
        public static string AndroidSdkRootDir { get; set; }
        public static string AndroidSdkCMake { get; set; }
        public static string AndroidSdkNinja { get; set; }
        public static string AndroidNdkRootDir { get; set; }
        public static string AndroidCMakeToolchain { get; set; }



        public static TargetArch[] TargetArchs = new TargetArch[]
        {
            new TargetArch(){ SourceDirName = "MSVC2017-x86-MT", DestDirName = "MSVC2017-x86-MT", PdbCopy = true },
            new TargetArch(){ SourceDirName = "Emscripten", DestDirName = "Emscripten" },
            new TargetArch(){ SourceDirName = "Android-arm64-v8a-Debug", DestDirName = "Android-arm64-v8a" },
            new TargetArch(){ SourceDirName = "Android-arm64-v8a-Release", DestDirName = "Android-arm64-v8a" },
            new TargetArch(){ SourceDirName = "Android-armeabi-v7a-Debug", DestDirName = "Android-armeabi-v7a" },
            new TargetArch(){ SourceDirName = "Android-armeabi-v7a-Release", DestDirName = "Android-armeabi-v7a" },
            new TargetArch(){ SourceDirName = "Android-x86-Debug", DestDirName = "Android-x86" },
            new TargetArch(){ SourceDirName = "Android-x86-Release", DestDirName = "Android-x86" },
            new TargetArch(){ SourceDirName = "Android-x86_64-Debug", DestDirName = "Android-x86_64" },
            new TargetArch(){ SourceDirName = "Android-x86_64-Release", DestDirName = "Android-x86_64" },
            new TargetArch(){ SourceDirName = "macOS", DestDirName = "macOS" },
            new TargetArch(){ SourceDirName = "iOS-SIMULATOR64-Debug", DestDirName = "iOS-SIMULATOR64-Debug" },
            new TargetArch(){ SourceDirName = "iOS-SIMULATOR64-Release", DestDirName = "iOS-SIMULATOR64-Release" },
            new TargetArch(){ SourceDirName = "iOS-OS-Debug", DestDirName = "iOS-OS-Debug" },
            new TargetArch(){ SourceDirName = "iOS-OS-Release", DestDirName = "iOS-OS-Release" },
        };

        public static void Initialize(string repoRootDir)
        {
            BuildToolsDir = Path.Combine(repoRootDir, "build", "BuildTools");

            EmsdkDir = Path.Combine(BuildToolsDir, "emsdk");
            EmscriptenDir = Path.Combine(EmsdkDir, "emscripten", emVer);
            emcmake = Path.Combine(EmscriptenDir, Utils.IsWin32 ? "emcmake.bat" : "emcmake");



            if (Utils.IsWin32)
            {
                //string localAppDir = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
                //AndroidSdkRootDir = Path.Combine(localAppDir, @"Android\Sdk");
                AndroidSdkRootDir = Path.Combine(BuildToolsDir, "android-sdk");

                AndroidSdkCMake = Path.Combine(AndroidSdkRootDir, @"cmake\3.6.4111459\bin\cmake.exe");
                AndroidSdkNinja = Path.Combine(AndroidSdkRootDir, @"cmake\3.6.4111459\bin\ninja.exe");

                AndroidNdkRootDir = Path.Combine(AndroidSdkRootDir, "ndk-bundle");
                AndroidCMakeToolchain = Path.Combine(AndroidNdkRootDir, @"build\cmake\android.toolchain.cmake");

                AndroidStudioFound = true;
            }
            else
            {
                AndroidStudioFound = false;
            }


            InstallTools();
        }

        private static void InstallTools()
        {
            Directory.CreateDirectory(BuildToolsDir);

            // Install emsdk
            {
                if (!Directory.Exists(EmsdkDir))
                {
                    Directory.SetCurrentDirectory(BuildToolsDir);
                    Utils.CallProcess("git", "clone https://github.com/juj/emsdk.git");
                }
                if (!Directory.Exists(EmscriptenDir))
                {
                    Directory.SetCurrentDirectory(Path.GetFullPath(EmsdkDir));

                    if (Utils.IsWin32)
                        Utils.CallProcess("emsdk.bat", "install " + emsdkVer);
                    else
                        Utils.CallProcess("emsdk", "install " + emsdkVer);
                }
            }

            // Install Android SDK
            {
                var androidSdk = Path.Combine(BuildToolsDir, "android-sdk");
                if (!Directory.Exists(androidSdk))
                {
                    var zip = Path.Combine(BuildToolsDir, "android-sdk-tools.zip");
                    if (Utils.IsWin32)
                        Utils.DownloadFile("https://dl.google.com/android/repository/sdk-tools-windows-4333796.zip", zip);
                    else if (Utils.IsMac)
                        Utils.DownloadFile("https://dl.google.com/android/repository/sdk-tools-darwin-4333796.zip", zip);

                    Utils.ExtractZipFile(zip, androidSdk);

                    var javaHome = Path.Combine(BuildToolsDir, "emsdk", "java", "8.152_64bit", "bin");
                    var skdmanager = Path.Combine(androidSdk, "tools", "bin", (Utils.IsWin32) ? "sdkmanager.bat" : "sdkmanager");

                    var env = new Dictionary<string, string>()
                    {
                        { "PATH", javaHome + ";"+ Environment.GetEnvironmentVariable("PATH") },
                    };
                    
                    Utils.CallProcess(skdmanager, "cmake;3.6.4111459", env, (stdin) => stdin.WriteLine("y"));
                    Utils.CallProcess(skdmanager, "ndk-bundle", env, (stdin) => stdin.WriteLine("y"));
                    //Utils.CallProcess(skdmanager, "lldb;3.1", env, (stdin) => stdin.WriteLine("y"));
                }
            }
        }
    }
}
