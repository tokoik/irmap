#include <cstdlib>
#include <iostream>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// �ŏ��ɊJ���E�B���h�E�̃T�C�Y
const GLsizei width(960), height(540);

// �w�i�F
const GLfloat background[] = { 0.0f, 0.0f, 0.0f, 0.0f };

// ����
const GgSimpleShader::Light light =
{
  { 0.2f, 0.2f, 0.2f, 1.0f }, // ��������
  { 1.0f, 1.0f, 1.0f, 0.0f }, // �g�U���ˌ�����
  { 1.0f, 1.0f, 1.0f, 0.0f }, // ���ʌ�����
  { 0.0f, 0.0f, 1.0f, 0.0f }  // �ʒu
};

//
// ���C��
//
int main()
{
  // �E�B���h�E���쐬����
  Window window(width, height, "realtime irradiance mapping");

  // �E�B���h�E���J�������ǂ����m���߂�
  if (!window.get())
  {
    // �E�B���h�E���J���Ȃ�����
    std::cerr << "Can't open GLFW window.\n";
    return EXIT_FAILURE;
  }

  // �\������}�`�̌`��f�[�^�t�@�C����ǂݍ���
  const GgObj object("bunny.obj", true);

  // �}�`�̕\���ɗp����V�F�[�_��ǂݍ���
  const GgSimpleShader simple("pass1.vert", "pass1.frag");

  // �}�`�\���p�̎���ϊ��s���ݒ肷��
  const auto mv(ggLookat(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // �w�i�F��ݒ肷��
  glClearColor(background[0], background[1], background[2], background[3]);

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (!window.shouldClose())
  {
    // �r���[�|�[�g��ݒ肷��
    window.setViewport();

    // �B�ʏ������s��
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // �J���[�o�b�t�@�ƃf�v�X�o�b�t�@����������
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // �}�`�\���p�̓��e�ϊ��s������݂̃E�B���h�E�̃A�X�y�N�g�ɍ��킹�Đݒ肷��
    const auto mp(ggPerspective(1.0f, window.getAspect(), 1.0f, 5.0f));

    // �}�`�`��p�̃V�F�[�_�v���O�����̎g�p���J�n����
    simple.use();

    // ������ݒ肷��
    simple.setLight(light);

    // �ϊ��s���ݒ肷��
    simple.loadMatrix(mp, mv * window.getLeftTrackball());

    // �}�`��`�悷��
    object.draw(simple);

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
