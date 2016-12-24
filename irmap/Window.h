#pragma once

// 補助プログラム
#include "gg.h"
using namespace gg;

//
// ウィンドウ関連の処理を担当するクラス
//
class Window
{
  // ウィンドウの識別子
  GLFWwindow *const window;

  // ウィンドウのサイズ
  GLsizei width, height;

  // ウィンドウのアスペクト比
  GLfloat aspect;

  // シフトキー
  bool shift_key;

  // コントロールキー
  bool control_key;

  // 矢印キー
  int arrow[2];

  // シフトキーを押しながら矢印キー
  int shift_arrow[2];

  // コントロールキーを押しながら矢印キー
  int control_arrow[2];

  // マウスの現在位置
  double mouse_x, mouse_y;

  // マウスホイールの回転量
  double wheel_rotation;

  // シフトを押しながらマウスホイールの回転量
  double shift_wheel_rotation;

  // コントロールを押しながらマウスホイールの回転量
  double control_wheel_rotation;

  // 左ドラッグによるトラックボール
  GgTrackball trackball_left;

  // 右ドラッグによるトラックボール
  GgTrackball trackball_right;

  // コピーコンストラクタを封じる
  Window(const Window &w);

  // 代入を封じる
  Window &operator=(const Window &w);

  // GLFW のウィンドウの作成
  GLFWwindow *createWindow(int width, int height, const char *title,
    GLFWmonitor *monitor, GLFWwindow *share)
  {
    // 初期化済なら true
    static bool initialized(false);

    // GLFW が初期化されていなければ
    if (!initialized)
    {
      // GLFW を初期化する
      if (glfwInit() == GL_FALSE) return nullptr;

      // プログラム終了時の処理を登録する
      atexit(glfwTerminate);

      // OpenGL Version 3.2 Core Profile を選択する
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      // 初期化済みの印をつける
      initialized = true;
    }

    // GLFW のウィンドウを作成する
    return glfwCreateWindow(width, height, title, monitor, share);
  }

public:

  // コンストラクタ
  Window(int width = 640, int height = 480, const char *title = "GLFW Window",
    GLFWmonitor *monitor = nullptr, GLFWwindow *share = nullptr)
    : window(createWindow(width, height, title, monitor, share))
    , arrow{ 0, 0 }, shift_arrow{ 0, 0 }, control_arrow{ 0, 0 }
    , wheel_rotation(0.0), shift_wheel_rotation(0.0), control_wheel_rotation(0.0)
  {
    if (!window) return;

    // 現在のウィンドウを処理対象にする
    glfwMakeContextCurrent(window);

    // 垂直帰線区間を待つ
    glfwSwapInterval(1);

    // ウィンドウのサイズ変更時に呼び出す処理を登録する
    glfwSetFramebufferSizeCallback(window, resize);

    // キーボードを操作した時の処理を登録する
    glfwSetKeyCallback(window, keyboard);

    // マウスボタンを操作したときの処理を登録する
    glfwSetMouseButtonCallback(window, mouse);

    // マウスホイール操作時に呼び出す処理を登録する
    glfwSetScrollCallback(window, wheel);

    // このインスタンスの this ポインタを記録しておく
    glfwSetWindowUserPointer(window, this);

    // ゲームグラフィックス特論の都合による初期化を行う
    ggInit();

    // ビューポートと投影変換行列を初期化する
    resize(window, width, height);
  }

  // デストラクタ
  virtual ~Window()
  {
    // ウィンドウを破棄する
    glfwDestroyWindow(window);
  }

  // ウィンドウの識別子の取得
  const GLFWwindow *get() const
  {
    return window;
  }

  // ウィンドウを閉じるべきかを判定する
  bool shouldClose()
  {
    // ウィンドウを閉じるか ESC キーがタイプされていれば真を返す
    return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
  }

  // ビューポートを設定する
  void setViewport()
  {
    // ウィンドウ全体をビューポートにする
    glViewport(0, 0, width, height);
  }

  // カラーバッファを入れ替えてイベントを取り出す
  void swapBuffers()
  {
    // エラーチェック
    ggError(__FILE__, __LINE__);

    // カラーバッファを入れ替える
    glfwSwapBuffers(window);

    // シフトキーとコントロールキーの状態をリセットする
    shift_key = control_key = false;

    // イベントを取り出す
    glfwPollEvents();

    // マウスの位置を調べる
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    const GLfloat x(static_cast<GLfloat>(mouse_x));
    const GLfloat y(static_cast<GLfloat>(mouse_y));

    // 左ボタンドラッグ
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) trackball_left.motion(x, y);

    // 右ボタンドラッグ
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) trackball_right.motion(x, y);
  }

  // ウィンドウのサイズ変更時の処理
  static void resize(GLFWwindow *window, int width, int height)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      // ウィンドウのサイズを保存する
      instance->width = width;
      instance->height = height;

      // ウィンドウのアスペクト比を保存する
      instance->aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);

      // トラックボール処理の範囲を設定する
      instance->trackball_left.region(width, height);
      instance->trackball_right.region(width, height);
    }
  }

  // キーボードをタイプした時の処理
  static void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      if (action == GLFW_PRESS)
      {
        switch (key)
        {
        case GLFW_KEY_R:
          // マウスホイールの回転量をリセットする
          instance->wheel_rotation = 0.0;
          instance->shift_wheel_rotation = 0.0;
          instance->control_wheel_rotation = 0.0;

          // 矢印キーの設定値をリセットする
          instance->arrow[0] = instance->arrow[1] = 0;
          instance->shift_arrow[0] = instance->shift_arrow[1] = 0;
          instance->control_arrow[0] = instance->control_arrow[1] = 0;

        case GLFW_KEY_O:
          // トラックボールをリセットする
          instance->trackball_left.reset();
          instance->trackball_right.reset();
          break;

        case GLFW_KEY_SPACE:
          break;

        case GLFW_KEY_BACKSPACE:
        case GLFW_KEY_DELETE:
          break;

        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
          instance->shift_key = true;
          break;

        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
          instance->control_key = true;
          break;

        case GLFW_KEY_UP:
          if (instance->shift_key)
            instance->shift_arrow[1]++;
          else if (instance->control_key)
            instance->control_arrow[1]++;
          else
            instance->arrow[1]++;
          break;

        case GLFW_KEY_DOWN:
          if (instance->shift_key)
            instance->shift_arrow[1]--;
          else if (instance->control_key)
            instance->control_arrow[1]--;
          else
            instance->arrow[1]--;
          break;

        case GLFW_KEY_RIGHT:
          if (instance->shift_key)
            instance->shift_arrow[0]++;
          else if (instance->control_key)
            instance->control_arrow[0]++;
          else
            instance->arrow[0]++;
          break;

        case GLFW_KEY_LEFT:
          if (instance->shift_key)
            instance->shift_arrow[0]--;
          else if (instance->control_key)
            instance->control_arrow[0]--;
          else
            instance->arrow[0]--;
          break;

        default:
          break;
        }
      }
    }
  }

  // マウスボタンを操作したときの処理
  static void mouse(GLFWwindow *window, int button, int action, int mods)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      // マウスの現在位置を得る
      const GLfloat x(static_cast<GLfloat>(instance->mouse_x));
      const GLfloat y(static_cast<GLfloat>(instance->mouse_y));

      switch (button)
      {
        case GLFW_MOUSE_BUTTON_1:
          if (action)
          {
            // 左ドラッグ開始
            instance->trackball_left.start(x, y);
          }
          else
          {
            // 左ドラッグ終了
            instance->trackball_left.stop(x, y);
          }
          break;

        case GLFW_MOUSE_BUTTON_2:
          if (action)
          {
            // 右ドラッグ開始
            instance->trackball_right.start(x, y);
          }
          else
          {
            // 右ドラッグ終了
            instance->trackball_right.stop(x, y);
          }
          break;

        case GLFW_MOUSE_BUTTON_3:
          break;

        default:
          break;
      }
    }
  }

  // マウスホイール操作時の処理
  static void wheel(GLFWwindow *window, double x, double y)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      if (instance->shift_key)
        instance->shift_wheel_rotation += y;
      else if (instance->control_key)
        instance->control_wheel_rotation += y;
      else
      {
        instance->wheel_rotation += y;
        if (instance->wheel_rotation < -100.0)
          instance->wheel_rotation = -100.0;
        else if (instance->wheel_rotation > 49.0)
          instance->wheel_rotation = 49.0;
      }
    }
  }

  // ウィンドウの幅を得る
  GLsizei getWidth() const
  {
    return width;
  }

  // ウィンドウの高さを得る
  GLsizei getHeight() const
  {
    return height;
  }

  // ウィンドウのサイズを得る
  void getSize(GLsizei *size) const
  {
    size[0] = getWidth();
    size[1] = getHeight();
  }

  // ウィンドウのアスペクト比を得る
  GLfloat getAspect() const
  {
    return aspect;
  }

  // 矢印キーの現在の X 値を得る
  GLfloat getArrowX() const
  {
    return static_cast<GLfloat>(arrow[0]);
  }

  // 矢印キーの現在の Y 値を得る
  GLfloat getArrowY() const
  {
    return static_cast<GLfloat>(arrow[1]);
  }

  // 矢印キーの現在の値を得る
  void getArrow(GLfloat *arrow) const
  {
    arrow[0] = getArrowX();
    arrow[1] = getArrowY();
  }

  // シフトキーを押しながら矢印キーの現在の X 値を得る
  GLfloat getShiftArrowX() const
  {
    return static_cast<GLfloat>(shift_arrow[0]);
  }

  // シフトキーを押しながら矢印キーの現在の Y 値を得る
  GLfloat getShiftArrowY() const
  {
    return static_cast<GLfloat>(shift_arrow[1]);
  }

  // シフトキーを押しながら矢印キーの現在の値を得る
  void getShiftArrow(GLfloat *shift_arrow) const
  {
    shift_arrow[0] = getShiftArrowX();
    shift_arrow[1] = getShiftArrowY();
  }

  // コントロールキーを押しながら矢印キーの現在の X 値を得る
  GLfloat getControlArrowX() const
  {
    return static_cast<GLfloat>(control_arrow[0]);
  }

  // コントロールキーを押しながら矢印キーの現在の Y 値を得る
  GLfloat getControlArrowY() const
  {
    return static_cast<GLfloat>(control_arrow[1]);
  }

  // コントロールキーを押しながら矢印キーの現在の値を得る
  void getControlArrow(GLfloat *control_arrow) const
  {
    control_arrow[0] = getControlArrowX();
    control_arrow[1] = getControlArrowY();
  }

  // マウスの X 座標を得る
  GLfloat getMouseX() const
  {
    return static_cast<GLfloat>(mouse_x);
  }

  // マウスの Y 座標を得る
  GLfloat getMouseY() const
  {
    return static_cast<GLfloat>(mouse_y);
  }

  // マウスの現在位置を得る
  void getMouse(GLfloat *position) const
  {
    position[0] = getMouseX();
    position[1] = getMouseY();
  }

  // マウスホイールの現在の回転角を得る
  GLfloat getWheel() const
  {
    return static_cast<GLfloat>(wheel_rotation);
  }

  // シフトを押しながらマウスホイールの現在の回転角を得る
  GLfloat getShiftWheel() const
  {
    return static_cast<GLfloat>(shift_wheel_rotation);
  }

  // コントロールを押しながらマウスホイールの現在の回転角を得る
  GLfloat getControlWheel() const
  {
    return static_cast<GLfloat>(control_wheel_rotation);
  }

  // 左ボタンによるトラックボールの回転変換行列を得る
  const GgMatrix getLeftTrackball() const
  {
    return trackball_left.getMatrix();
  }

  // 右ボタンによるトラックボールの回転変換行列を得る
  const GgMatrix getRightTrackball() const
  {
    return trackball_right.getMatrix();
  }
};
