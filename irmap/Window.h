#pragma once

// �⏕�v���O����
#include "gg.h"
using namespace gg;

//
// �E�B���h�E�֘A�̏�����S������N���X
//
class Window
{
  // �E�B���h�E�̎��ʎq
  GLFWwindow *const window;

  // �E�B���h�E�̃T�C�Y
  GLsizei width, height;

  // �E�B���h�E�̃A�X�y�N�g��
  GLfloat aspect;

  // �V�t�g�L�[
  bool shift_key;

  // �R���g���[���L�[
  bool control_key;

  // ���L�[
  int arrow[2];

  // �V�t�g�L�[�������Ȃ�����L�[
  int shift_arrow[2];

  // �R���g���[���L�[�������Ȃ�����L�[
  int control_arrow[2];

  // �}�E�X�̌��݈ʒu
  double mouse_x, mouse_y;

  // �}�E�X�z�C�[���̉�]��
  double wheel_rotation;

  // �V�t�g�������Ȃ���}�E�X�z�C�[���̉�]��
  double shift_wheel_rotation;

  // �R���g���[���������Ȃ���}�E�X�z�C�[���̉�]��
  double control_wheel_rotation;

  // ���h���b�O�ɂ��g���b�N�{�[��
  GgTrackball trackball_left;

  // �E�h���b�O�ɂ��g���b�N�{�[��
  GgTrackball trackball_right;

  // �R�s�[�R���X�g���N�^�𕕂���
  Window(const Window &w);

  // ����𕕂���
  Window &operator=(const Window &w);

  // GLFW �̃E�B���h�E�̍쐬
  GLFWwindow *createWindow(int width, int height, const char *title,
    GLFWmonitor *monitor, GLFWwindow *share)
  {
    // �������ςȂ� true
    static bool initialized(false);

    // GLFW ������������Ă��Ȃ����
    if (!initialized)
    {
      // GLFW ������������
      if (glfwInit() == GL_FALSE) return nullptr;

      // �v���O�����I�����̏�����o�^����
      atexit(glfwTerminate);

      // OpenGL Version 3.2 Core Profile ��I������
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      // �������ς݂̈������
      initialized = true;
    }

    // GLFW �̃E�B���h�E���쐬����
    return glfwCreateWindow(width, height, title, monitor, share);
  }

public:

  // �R���X�g���N�^
  Window(int width = 640, int height = 480, const char *title = "GLFW Window",
    GLFWmonitor *monitor = nullptr, GLFWwindow *share = nullptr)
    : window(createWindow(width, height, title, monitor, share))
    , arrow{ 0, 0 }, shift_arrow{ 0, 0 }, control_arrow{ 0, 0 }
    , wheel_rotation(0.0), shift_wheel_rotation(0.0), control_wheel_rotation(0.0)
  {
    if (!window) return;

    // ���݂̃E�B���h�E�������Ώۂɂ���
    glfwMakeContextCurrent(window);

    // �����A����Ԃ�҂�
    glfwSwapInterval(1);

    // �E�B���h�E�̃T�C�Y�ύX���ɌĂяo��������o�^����
    glfwSetFramebufferSizeCallback(window, resize);

    // �L�[�{�[�h�𑀍삵�����̏�����o�^����
    glfwSetKeyCallback(window, keyboard);

    // �}�E�X�{�^���𑀍삵���Ƃ��̏�����o�^����
    glfwSetMouseButtonCallback(window, mouse);

    // �}�E�X�z�C�[�����쎞�ɌĂяo��������o�^����
    glfwSetScrollCallback(window, wheel);

    // ���̃C���X�^���X�� this �|�C���^���L�^���Ă���
    glfwSetWindowUserPointer(window, this);

    // �Q�[���O���t�B�b�N�X���_�̓s���ɂ�鏉�������s��
    ggInit();

    // �r���[�|�[�g�Ɠ��e�ϊ��s�������������
    resize(window, width, height);
  }

  // �f�X�g���N�^
  virtual ~Window()
  {
    // �E�B���h�E��j������
    glfwDestroyWindow(window);
  }

  // �E�B���h�E�̎��ʎq�̎擾
  const GLFWwindow *get() const
  {
    return window;
  }

  // �E�B���h�E�����ׂ����𔻒肷��
  bool shouldClose()
  {
    // �E�B���h�E����邩 ESC �L�[���^�C�v����Ă���ΐ^��Ԃ�
    return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
  }

  // �r���[�|�[�g��ݒ肷��
  void setViewport()
  {
    // �E�B���h�E�S�̂��r���[�|�[�g�ɂ���
    glViewport(0, 0, width, height);
  }

  // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
  void swapBuffers()
  {
    // �G���[�`�F�b�N
    ggError(__FILE__, __LINE__);

    // �J���[�o�b�t�@�����ւ���
    glfwSwapBuffers(window);

    // �V�t�g�L�[�ƃR���g���[���L�[�̏�Ԃ����Z�b�g����
    shift_key = control_key = false;

    // �C�x���g�����o��
    glfwPollEvents();

    // �}�E�X�̈ʒu�𒲂ׂ�
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    const GLfloat x(static_cast<GLfloat>(mouse_x));
    const GLfloat y(static_cast<GLfloat>(mouse_y));

    // ���{�^���h���b�O
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) trackball_left.motion(x, y);

    // �E�{�^���h���b�O
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) trackball_right.motion(x, y);
  }

  // �E�B���h�E�̃T�C�Y�ύX���̏���
  static void resize(GLFWwindow *window, int width, int height)
  {
    // ���̃C���X�^���X�� this �|�C���^�𓾂�
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      // �E�B���h�E�̃T�C�Y��ۑ�����
      instance->width = width;
      instance->height = height;

      // �E�B���h�E�̃A�X�y�N�g���ۑ�����
      instance->aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);

      // �g���b�N�{�[�������͈̔͂�ݒ肷��
      instance->trackball_left.region(width, height);
      instance->trackball_right.region(width, height);
    }
  }

  // �L�[�{�[�h���^�C�v�������̏���
  static void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
  {
    // ���̃C���X�^���X�� this �|�C���^�𓾂�
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      if (action == GLFW_PRESS)
      {
        switch (key)
        {
        case GLFW_KEY_R:
          // �}�E�X�z�C�[���̉�]�ʂ����Z�b�g����
          instance->wheel_rotation = 0.0;
          instance->shift_wheel_rotation = 0.0;
          instance->control_wheel_rotation = 0.0;

          // ���L�[�̐ݒ�l�����Z�b�g����
          instance->arrow[0] = instance->arrow[1] = 0;
          instance->shift_arrow[0] = instance->shift_arrow[1] = 0;
          instance->control_arrow[0] = instance->control_arrow[1] = 0;

        case GLFW_KEY_O:
          // �g���b�N�{�[�������Z�b�g����
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

  // �}�E�X�{�^���𑀍삵���Ƃ��̏���
  static void mouse(GLFWwindow *window, int button, int action, int mods)
  {
    // ���̃C���X�^���X�� this �|�C���^�𓾂�
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      // �}�E�X�̌��݈ʒu�𓾂�
      const GLfloat x(static_cast<GLfloat>(instance->mouse_x));
      const GLfloat y(static_cast<GLfloat>(instance->mouse_y));

      switch (button)
      {
        case GLFW_MOUSE_BUTTON_1:
          if (action)
          {
            // ���h���b�O�J�n
            instance->trackball_left.start(x, y);
          }
          else
          {
            // ���h���b�O�I��
            instance->trackball_left.stop(x, y);
          }
          break;

        case GLFW_MOUSE_BUTTON_2:
          if (action)
          {
            // �E�h���b�O�J�n
            instance->trackball_right.start(x, y);
          }
          else
          {
            // �E�h���b�O�I��
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

  // �}�E�X�z�C�[�����쎞�̏���
  static void wheel(GLFWwindow *window, double x, double y)
  {
    // ���̃C���X�^���X�� this �|�C���^�𓾂�
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

  // �E�B���h�E�̕��𓾂�
  GLsizei getWidth() const
  {
    return width;
  }

  // �E�B���h�E�̍����𓾂�
  GLsizei getHeight() const
  {
    return height;
  }

  // �E�B���h�E�̃T�C�Y�𓾂�
  void getSize(GLsizei *size) const
  {
    size[0] = getWidth();
    size[1] = getHeight();
  }

  // �E�B���h�E�̃A�X�y�N�g��𓾂�
  GLfloat getAspect() const
  {
    return aspect;
  }

  // ���L�[�̌��݂� X �l�𓾂�
  GLfloat getArrowX() const
  {
    return static_cast<GLfloat>(arrow[0]);
  }

  // ���L�[�̌��݂� Y �l�𓾂�
  GLfloat getArrowY() const
  {
    return static_cast<GLfloat>(arrow[1]);
  }

  // ���L�[�̌��݂̒l�𓾂�
  void getArrow(GLfloat *arrow) const
  {
    arrow[0] = getArrowX();
    arrow[1] = getArrowY();
  }

  // �V�t�g�L�[�������Ȃ�����L�[�̌��݂� X �l�𓾂�
  GLfloat getShiftArrowX() const
  {
    return static_cast<GLfloat>(shift_arrow[0]);
  }

  // �V�t�g�L�[�������Ȃ�����L�[�̌��݂� Y �l�𓾂�
  GLfloat getShiftArrowY() const
  {
    return static_cast<GLfloat>(shift_arrow[1]);
  }

  // �V�t�g�L�[�������Ȃ�����L�[�̌��݂̒l�𓾂�
  void getShiftArrow(GLfloat *shift_arrow) const
  {
    shift_arrow[0] = getShiftArrowX();
    shift_arrow[1] = getShiftArrowY();
  }

  // �R���g���[���L�[�������Ȃ�����L�[�̌��݂� X �l�𓾂�
  GLfloat getControlArrowX() const
  {
    return static_cast<GLfloat>(control_arrow[0]);
  }

  // �R���g���[���L�[�������Ȃ�����L�[�̌��݂� Y �l�𓾂�
  GLfloat getControlArrowY() const
  {
    return static_cast<GLfloat>(control_arrow[1]);
  }

  // �R���g���[���L�[�������Ȃ�����L�[�̌��݂̒l�𓾂�
  void getControlArrow(GLfloat *control_arrow) const
  {
    control_arrow[0] = getControlArrowX();
    control_arrow[1] = getControlArrowY();
  }

  // �}�E�X�� X ���W�𓾂�
  GLfloat getMouseX() const
  {
    return static_cast<GLfloat>(mouse_x);
  }

  // �}�E�X�� Y ���W�𓾂�
  GLfloat getMouseY() const
  {
    return static_cast<GLfloat>(mouse_y);
  }

  // �}�E�X�̌��݈ʒu�𓾂�
  void getMouse(GLfloat *position) const
  {
    position[0] = getMouseX();
    position[1] = getMouseY();
  }

  // �}�E�X�z�C�[���̌��݂̉�]�p�𓾂�
  GLfloat getWheel() const
  {
    return static_cast<GLfloat>(wheel_rotation);
  }

  // �V�t�g�������Ȃ���}�E�X�z�C�[���̌��݂̉�]�p�𓾂�
  GLfloat getShiftWheel() const
  {
    return static_cast<GLfloat>(shift_wheel_rotation);
  }

  // �R���g���[���������Ȃ���}�E�X�z�C�[���̌��݂̉�]�p�𓾂�
  GLfloat getControlWheel() const
  {
    return static_cast<GLfloat>(control_wheel_rotation);
  }

  // ���{�^���ɂ��g���b�N�{�[���̉�]�ϊ��s��𓾂�
  const GgMatrix getLeftTrackball() const
  {
    return trackball_left.getMatrix();
  }

  // �E�{�^���ɂ��g���b�N�{�[���̉�]�ϊ��s��𓾂�
  const GgMatrix getRightTrackball() const
  {
    return trackball_right.getMatrix();
  }
};
