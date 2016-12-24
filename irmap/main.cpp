#include <cstdlib>
#include <iostream>

// ウィンドウ関連の処理
#include "Window.h"

// 最初に開くウィンドウのサイズ
const GLsizei width(960), height(540);

// 背景色
const GLfloat background[] = { 0.0f, 0.0f, 0.0f, 0.0f };

// 光源
const GgSimpleShader::Light light =
{
  { 0.2f, 0.2f, 0.2f, 1.0f }, // 環境光成分
  { 1.0f, 1.0f, 1.0f, 0.0f }, // 拡散反射光成分
  { 1.0f, 1.0f, 1.0f, 0.0f }, // 鏡面光成分
  { 0.0f, 0.0f, 1.0f, 0.0f }  // 位置
};

//
// メイン
//
int main()
{
  // ウィンドウを作成する
  Window window(width, height, "realtime irradiance mapping");

  // ウィンドウが開けたかどうか確かめる
  if (!window.get())
  {
    // ウィンドウが開けなかった
    std::cerr << "Can't open GLFW window.\n";
    return EXIT_FAILURE;
  }

  // 表示する図形の形状データファイルを読み込む
  const GgObj object("bunny.obj", true);

  // 図形の表示に用いるシェーダを読み込む
  const GgSimpleShader simple("pass1.vert", "pass1.frag");

  // 図形表示用の視野変換行列を設定する
  const auto mv(ggLookat(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // 背景色を設定する
  glClearColor(background[0], background[1], background[2], background[3]);

  // ウィンドウが開いている間繰り返す
  while (!window.shouldClose())
  {
    // ビューポートを設定する
    window.setViewport();

    // 隠面消去を行う
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // カラーバッファとデプスバッファを消去する
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 図形表示用の投影変換行列を現在のウィンドウのアスペクトに合わせて設定する
    const auto mp(ggPerspective(1.0f, window.getAspect(), 1.0f, 5.0f));

    // 図形描画用のシェーダプログラムの使用を開始する
    simple.use();

    // 光源を設定する
    simple.setLight(light);

    // 変換行列を設定する
    simple.loadMatrix(mp, mv * window.getLeftTrackball());

    // 図形を描画する
    object.draw(simple);

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
