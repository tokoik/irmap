#include <cstdlib>
#include <iostream>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// �ŏ��ɊJ���E�B���h�E�̃T�C�Y
const GLsizei width(960), height(540);

// �t���[���o�b�t�@�I�u�W�F�N�g�̃T�C�Y
const GLsizei fboWidth(width), fboHeight(height);

// ���E�F
const GLfloat border[] = { 0.0f, 0.0f, 0.0f, 0.0f };

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

// �e�N�X�`���̍쐬
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

  // �t���[���o�b�t�@�I�u�W�F�N�g�̃J���[�o�b�t�@�ɗp����e�N�X�`�����쐬����
  const auto color(createTexture(GL_RGBA, fboWidth, fboHeight));

  // �t���[���o�b�t�@�I�u�W�F�N�g�̃f�v�X�o�b�t�@�ɗp����e�N�X�`�� (�f�v�X�}�b�v) ���쐬����
  const auto depth([] { GLuint t; glGenTextures(1, &t); return t; } ());
  glBindTexture(GL_TEXTURE_2D, depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // �t���[���o�b�t�@�I�u�W�F�N�g���쐬����
  const auto fbo([] { GLuint f; glGenFramebuffers(1, &f); return f; } ());
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // �t���[���o�b�t�@�I�u�W�F�N�g�ɃJ���[�o�b�t�@��g�ݍ���
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color, 0);

  // �t���[���o�b�t�@�I�u�W�F�N�g�Ƀf�v�X�o�b�t�@��g�ݍ���
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0);

  // �x�������_�����O�ɗp�����`���쐬����
  const auto rectangle([] { GLuint vao; glGenVertexArrays(1, &vao); return vao; } ());

  // �x�������_�����O���s���V�F�[�_��ǂݍ���
  const auto pass2(ggLoadShader("pass2.vert", "pass2.frag"));

  // uniform �ϐ��̏ꏊ�𓾂�
  const auto colorLoc(glGetUniformLocation(pass2, "color"));

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (!window.shouldClose())
  {
    // �t���[���o�b�t�@�I�u�W�F�N�g�ɕ`�悷��
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // �t���[���o�b�t�@�I�u�W�F�N�g�ɑ΂���r���[�|�[�g��ݒ肷��
    glViewport(0, 0, fboWidth, fboHeight);

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

    // �ʏ�̃t���[���o�b�t�@�ɕ`�悷��
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // �r���[�|�[�g��ݒ肷��
    window.setViewport();

    // �B�ʏ������s��Ȃ�
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // �x�����_�����O���s���V�F�[�_�̎g�p���J�n����
    glUseProgram(pass2);

    // �J���[�o�b�t�@�Ɏg�����e�N�X�`�����w�肷��
    glUniform1i(colorLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color);

    // ��`��`��
    glBindVertexArray(rectangle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
