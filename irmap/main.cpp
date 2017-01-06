#include <cstdlib>
#include <iostream>
#include <vector>

// ウィンドウ関連の処理
#include "Window.h"

// 最初に開くウィンドウのサイズ
const GLsizei width(960), height(540);

// フレームバッファオブジェクトのサイズ
const GLsizei fboWidth(width), fboHeight(height);

// 境界色
const GLfloat border[] = { 0.0f, 0.0f, 0.0f, 0.0f };

// 背景色
const GLfloat background[] = { 0.0f, 0.0f, 0.0f, 0.0f };

// OpenCV によるビデオキャプチャ
#include "CamCv.h"

// キャプチャに用いるカメラのデバイス番号
const int captureDevice(0);

// キャプチャするフレームのサイズ (0 ならデフォルト)
const int captureWidth(1280), captureHeight(720);

// キャプチャするフレームレート (0 ならデフォルト)
const int captureFps(0);

// 法線方向のサンプル数
const GLsizei diffuseSamples(32);

// 法線方向のミップマップのレベル
const GLint diffuseLod(5);

// 正反射方向のサンプル数
const GLsizei specularSamples(16);

// 正反射方向のミップマップのレベル
const GLint specularLod(3);

// サンプル点の散布半径
const GLfloat radius(0.1f);

// テクスチャの作成
GLuint createTexture(GLenum internalFormat, GLsizei width, GLsizei height)
{
  GLuint t;

  glGenTextures(1, &t);
  glBindTexture(GL_TEXTURE_2D, t);
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

  return t;
}

// ミップマップテクスチャの作成
GLuint createTexture(GLenum internalFormat, GLsizei width, GLsizei height, GLint levels)
{
  GLuint t;

  glGenTextures(1, &t);
  glBindTexture(GL_TEXTURE_2D, t);
  for (GLint level = 0; level <= levels; ++level)
  {
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
    width = std::max(1, (width / 2));
    height = std::max(1, (height / 2));
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

  return t;
}

//
// メイン
//
int main()
{
  // カメラの使用を開始する
  CamCv camera;
  if (!camera.open(captureDevice, captureWidth, captureHeight, captureFps))
  {
    std::cerr << "Can't open capture device.\n";
    return EXIT_FAILURE;
  }
  camera.start();

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

  // フレームバッファオブジェクトのカラーバッファに用いるテクスチャを作成する
  std::vector<GLuint> color;
  color.push_back(createTexture(GL_RGBA, fboWidth, fboHeight));   // アルベド
  color.push_back(createTexture(GL_RGBA, fboWidth, fboHeight));   // フレネル項
  color.push_back(createTexture(GL_RGB32F, fboWidth, fboHeight)); // 位置
  color.push_back(createTexture(GL_RGB32F, fboWidth, fboHeight)); // 法線

  // カラーバッファの数
  const auto colorCount(static_cast<int>(color.size()));

  // フレームバッファオブジェクトのデプスバッファに用いるテクスチャ (デプスマップ) を作成する
  const auto depth([] { GLuint t; glGenTextures(1, &t); return t; } ());
  glBindTexture(GL_TEXTURE_2D, depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // 書き込むポリゴンのテクスチャ座標値のＲとテクスチャとの比較を行うようにする
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

  // もしＲの値がテクスチャの値以下なら真 (すなわち日向)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  // デプスマップの境界色はデプスの最大値 (1) にする
  static const GLfloat depthBorder[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depthBorder);

  // フレームバッファオブジェクトを作成する
  const auto fbo([] { GLuint f; glGenFramebuffers(1, &f); return f; } ());
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // レンダーターゲット
  std::vector<GLenum> target;

  for (int i = 0; i < colorCount; ++i)
  {
    // フレームバッファオブジェクトにカラーバッファを組み込む
    const GLenum attachment(GL_COLOR_ATTACHMENT0 + i);
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, color[i], 0);

    // カラーバッファを組み込んだアタッチメントを保存しておく
    target.push_back(attachment);
  }

  // フレームバッファオブジェクトにデプスバッファを組み込む
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0);

  // 環境のテクスチャを準備する
  const auto image(createTexture(GL_RGB, camera.getWidth(), camera.getHeight(), diffuseLod));

  // 遅延レンダリングに用いる矩形を作成する
  const auto rectangle([] { GLuint vao; glGenVertexArrays(1, &vao); return vao; } ());

  // 遅延レンダリングを行うシェーダを読み込む
  const auto pass2(ggLoadShader("pass2.vert", "pass2.frag"));

  // uniform 変数の場所を得る
  const auto colorLoc(glGetUniformLocation(pass2, "color"));
  const auto imageLoc(glGetUniformLocation(pass2, "image"));
  const auto depthLoc(glGetUniformLocation(pass2, "depth"));
  const auto radiusLoc(glGetUniformLocation(pass2, "radius"));
  const auto mpLoc(glGetUniformLocation(pass2, "mp"));
  const auto diffuseSamplesLoc(glGetUniformLocation(pass2, "diffuseSamples"));
  const auto diffuseLodLoc(glGetUniformLocation(pass2, "diffuseLod"));
  const auto specularSamplesLoc(glGetUniformLocation(pass2, "specularSamples"));
  const auto specularLodLoc(glGetUniformLocation(pass2, "specularLod"));
  const auto viewportLoc(glGetUniformLocation(pass2, "viewport"));
  const auto mtLoc(glGetUniformLocation(pass2, "mt"));

  // ウィンドウが開いている間繰り返す
  while (!window.shouldClose())
  {
    // フレームバッファオブジェクトに描画する
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // レンダーターゲットを指定する
    glDrawBuffers(static_cast<GLsizei>(target.size()), target.data());

    // フレームバッファオブジェクトに対するビューポートを設定する
    glViewport(0, 0, fboWidth, fboHeight);

    // 隠面消去を行う
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // カラーバッファとデプスバッファを消去する
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 図形表示用の投影変換行列を現在のウィンドウのアスペクトに合わせて設定する
    const auto mp(ggPerspective(1.0f, window.getAspect(), 1.0f, 5.0f));

    // 図形描画用のシェーダプログラムの使用を開始する
    simple.use();

    // 変換行列を設定する
    simple.loadMatrix(mp, mv * window.getLeftTrackball());

    // 図形を描画する
    object.draw(simple);

    // 通常のフレームバッファに描画する
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // バックバッファを指定する
    glDrawBuffer(GL_BACK);

    // ビューポートを設定する
    window.setViewport();

    // 隠面消去を行わない
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // 遅延レダリングを行うシェーダの使用を開始する
    glUseProgram(pass2);

    // カラーバッファに使ったテクスチャを指定する
    for (int i = 0; i < colorCount; ++i)
    {
      glUniform1i(colorLoc + i, i);
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, color[i]);
    }

    // 環境のテクスチャを指定する
    glUniform1i(imageLoc, colorCount);
    glActiveTexture(GL_TEXTURE0 + colorCount);
    glBindTexture(GL_TEXTURE_2D, image);

    // 環境のテクスチャに画像を転送する
    camera.transmit();
    glGenerateMipmap(GL_TEXTURE_2D);

    // 法線方向のサンプル点の数を設定する
    glUniform1i(diffuseSamplesLoc, diffuseSamples);

    // ミップマップのレベルを設定する
    glUniform1i(diffuseLodLoc, diffuseLod);

    // 正反射方向のサンプル点の数を設定する
    glUniform1i(specularSamplesLoc, specularSamples);

    // 正反射方向のミップマップのレベルを設定する
    glUniform1i(specularLodLoc, specularLod);

    // デプスマップを指定する
    glUniform1i(depthLoc, colorCount + 1);
    glActiveTexture(GL_TEXTURE0 + colorCount + 1);
    glBindTexture(GL_TEXTURE_2D, depth);

    // サンプル点の散布半径を設定する
    glUniform1f(radiusLoc, radius);

    // 投影変換行列を設定する
    glUniformMatrix4fv(mpLoc, 1, GL_FALSE, mp.get());

    // ビューポートの中心位置とスクリーンのスケールを指定する
    const GLfloat centerX(static_cast<GLfloat>(window.getWidth()) * 0.5f);
    const GLfloat centerY(static_cast<GLfloat>(window.getHeight()) * 0.5f);
    const GLfloat scale((1.0f + window.getWheel() * 0.05f) / static_cast<GLfloat>(window.getHeight()));
    glUniform4f(viewportLoc, centerX, centerY, scale, scale);

    // テクスチャの回転行列を設定する
    glUniformMatrix4fv(mtLoc, 1, GL_FALSE, window.getRightTrackball().get());

    // 矩形を描く
    glBindVertexArray(rectangle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
