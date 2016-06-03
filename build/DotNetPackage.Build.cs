using System;
using System.Text;
using System.IO;
using System.IO.Compression;
using LuminoBuildTool;

class DotNetPackageRule : ModuleRule
{
    /// <summary>
    /// ルールの名前
    /// </summary>
    public override string Name
    {
        get { return "csp"; }
    }

    /// <summary>
    /// 前提条件の確認
    /// </summary>
    public override void CheckPrerequisite()
    {
        Buildable = true;
    }

    /// <summary>
    /// ビルド実行
    /// </summary>
    public override void Build(Builder builder)
    {
        string dotnetDir = builder.LuminoBindingsDir + "DotNet/";
        string releaseDir = builder.LuminoPackageReleaseDir + "LuminoDotNet_" + builder.VersionString + "/";
        string pkgSrcDir = builder.LuminoPackageDir + "PackageSource/DotNet/";
        string zipFilePath = builder.LuminoPackageReleaseDir + "LuminoDotNet_" + builder.VersionString + ".zip";
        
        Directory.CreateDirectory(releaseDir);

        // lib
        string outputDir = dotnetDir + "LuminoDotNet/bin/x86/Release/";
        string releaseLibDir = releaseDir + "lib/";
        Logger.WriteLine("copy lib files...");
        Directory.CreateDirectory(releaseLibDir);
        Utils.CopyFile(outputDir + "LuminoDotNet.dll", releaseLibDir);
        Utils.CopyFile(outputDir + "LuminoDotNet.XML", releaseLibDir);
        if (Utils.IsWin32)
            File.Copy(builder.LuminoLibDir + "Release/LuminoC_x86uMT.dll", releaseLibDir + "LuminoC.dll", true);
        else
            File.Copy(builder.LuminoLibDir + "Release/LuminoC_x86u.so", releaseLibDir + "LuminoC.so", true);
        
        // Help
        if (Utils.IsWin32)
        {
            Logger.WriteLine("copy Help files...");
            Directory.CreateDirectory(releaseDir + "doc");
            Utils.CopyDirectory(dotnetDir + "Help", releaseDir + "doc", true);
        }

        // Readme.txt (バージョン名を埋め込む)
        string text = File.ReadAllText(pkgSrcDir + "Readme.txt");
        text = text.Replace("$(LuminoVersion)", builder.VersionString);
        File.WriteAllText(releaseDir + "Readme.txt", text, new UTF8Encoding(true));

        // VSプロジェクトテンプレート
        Logger.WriteLine("make project template...");
        string templateProjDir = builder.LuminoToolsDir + "VS2015ProjectTemplate/LuminoProjectCS/";
        Directory.CreateDirectory(releaseDir + "tools");
        Utils.CopyDirectory(releaseLibDir, templateProjDir + "LuminoLibrary");
        File.Delete(releaseDir + "tools/LuminoProjectCS.zip");  // 既に存在する場合は消さないと例外する
        ZipFile.CreateFromDirectory(templateProjDir, releaseDir + "tools/LuminoProjectCS.zip", CompressionLevel.Optimal, false);

        // installer
        Utils.CopyFile(pkgSrcDir + "Lumino_Install.bat", releaseDir);
        Utils.CopyFile(pkgSrcDir + "Lumino_Uninstall.bat", releaseDir);

        // .zip に圧縮する
        Logger.WriteLine("compressing files...");
        File.Delete(zipFilePath);
        ZipFile.CreateFromDirectory(releaseDir, zipFilePath, CompressionLevel.Optimal, true);
    }
}
