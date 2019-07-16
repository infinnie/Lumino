﻿#pragma once
#include "../Graphics/GeometryStructs.hpp"

namespace ln {
class GraphicsContext;
class UIContext;
class UIFrameWindow;
class UIViewport;
class UIContainerElement;
class WorldRenderView;
class PhysicsWorld;
class PhysicsWorld2D;
class World;
class Camera;
class AmbientLight;
class DirectionalLight;

/** アプリケーション全体にかかわる処理を行います。 */
LN_CLASS(Static)
class LN_API Engine
{
public:

	/** エンジンの初期化処理を行います。 */
	LN_METHOD(RuntimeInitializer)
	static void initialize();

	/** エンジンの終了処理を行います。 */
	LN_METHOD()
	static void finalize();

	/**
	@brief		1フレーム分の更新処理を行います。
	@return		アプリケーションの終了が要求されている場合は false を返します。
	@details	この関数はグラフィックスと入力を更新し、指定されたフレームレートになるように待機します。
	*/
	LN_METHOD()
	static bool update();

    static void quit();

    static void resetFrameDelay();

    /** アプリケーション開始からの経過時間を取得します。この値はタイムスケールの影響を受けます。 */
    static double totalTime();
    
    /** アプリケーション全体のタイムスケールを設定します。 */
    static void setTimeScale(float value);

	/** セーブデータなど、アプリケーションデータを保存するための永続的なデータディレクトリのパスを返します。 */
	static const Path& persistentDataPath();

    /** メインウィンドウのタイトルバーに、秒間の平均 FPS を表示します。簡易的なパフォーマンス測定に利用できます。(default: false) */
    static void setShowDebugFpsEnabled(bool enabled);

	static GraphicsContext* graphicsContext();
    static UIContext* mainUIContext();
	static UIFrameWindow* mainWindow();
    static UIViewport* mainViewport();
	static UIContainerElement* mainUIRoot();
    static Size mainViewSize();
    static World* mainWorld();
    static Camera* mainCamera();
    static AmbientLight* mainAmbientLight();
    static DirectionalLight* mainDirectionalLight();
    static WorldRenderView* mainRenderView();
    static PhysicsWorld* mainPhysicsWorld();
    static PhysicsWorld2D* mainPhysicsWorld2D();

};

} // namespace ln
