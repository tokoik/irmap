#include <cstdlib>
#include <iostream>
#include <vector>

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

// OpenCV �ɂ��r�f�I�L���v�`��
#include "CamCv.h"

// �L���v�`���ɗp����J�����̃f�o�C�X�ԍ�
const int captureDevice(0);

// �L���v�`������t���[���̃T�C�Y (0 �Ȃ�f�t�H���g)
const int captureWidth(1280), captureHeight(720);

// �L���v�`������t���[�����[�g (0 �Ȃ�f�t�H���g)
const int captureFps(0);

// �@�������̃T���v����
const GLsizei diffuseSamples(32);

// �@�������̃~�b�v�}�b�v�̃��x��
const GLint diffuseLod(5);

// �����˕����̃T���v����
const GLsizei specularSamples(16);

// �����˕����̃~�b�v�}�b�v�̃��x��
const GLint specularLod(3);

// �T���v���_�̎U�z���a
const GLfloat radius(0.1f);

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

// �~�b�v�}�b�v�e�N�X�`���̍쐬
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
// ���C��
//
int main()
{
  // �J�����̎g�p���J�n����
  CamCv camera;
  if (!camera.open(captureDevice, captureWidth, captureHeight, captureFps))
  {
    std::cerr << "Can't open capture device.\n";
    return EXIT_FAILURE;
  }
  camera.start();

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
  std::vector<GLuint> color;
  color.push_back(createTexture(GL_RGBA, fboWidth, fboHeight));   // �A���x�h
  color.push_back(createTexture(GL_RGBA, fboWidth, fboHeight));   // �t���l����
  color.push_back(createTexture(GL_RGB32F, fboWidth, fboHeight)); // �ʒu
  color.push_back(createTexture(GL_RGB32F, fboWidth, fboHeight)); // �@��

  // �J���[�o�b�t�@�̐�
  const auto colorCount(static_cast<int>(color.size()));

  // �t���[���o�b�t�@�I�u�W�F�N�g�̃f�v�X�o�b�t�@�ɗp����e�N�X�`�� (�f�v�X�}�b�v) ���쐬����
  const auto depth([] { GLuint t; glGenTextures(1, &t); return t; } ());
  glBindTexture(GL_TEXTURE_2D, depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // �������ރ|���S���̃e�N�X�`�����W�l�̂q�ƃe�N�X�`���Ƃ̔�r���s���悤�ɂ���
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

  // �����q�̒l���e�N�X�`���̒l�ȉ��Ȃ�^ (���Ȃ킿����)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  // �f�v�X�}�b�v�̋��E�F�̓f�v�X�̍ő�l (1) �ɂ���
  static const GLfloat depthBorder[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depthBorder);

  // �t���[���o�b�t�@�I�u�W�F�N�g���쐬����
  const auto fbo([] { GLuint f; glGenFramebuffers(1, &f); return f; } ());
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // �����_�[�^�[�Q�b�g
  std::vector<GLenum> target;

  for (int i = 0; i < colorCount; ++i)
  {
    // �t���[���o�b�t�@�I�u�W�F�N�g�ɃJ���[�o�b�t�@��g�ݍ���
    const GLenum attachment(GL_COLOR_ATTACHMENT0 + i);
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, color[i], 0);

    // �J���[�o�b�t�@��g�ݍ��񂾃A�^�b�`�����g��ۑ����Ă���
    target.push_back(attachment);
  }

  // �t���[���o�b�t�@�I�u�W�F�N�g�Ƀf�v�X�o�b�t�@��g�ݍ���
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0);

  // ���̃e�N�X�`������������
  const auto image(createTexture(GL_RGB, camera.getWidth(), camera.getHeight(), diffuseLod));

  // �x�������_�����O�ɗp�����`���쐬����
  const auto rectangle([] { GLuint vao; glGenVertexArrays(1, &vao); return vao; } ());

  // �x�������_�����O���s���V�F�[�_��ǂݍ���
  const auto pass2(ggLoadShader("pass2.vert", "pass2.frag"));

  // uniform �ϐ��̏ꏊ�𓾂�
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

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (!window.shouldClose())
  {
    // �t���[���o�b�t�@�I�u�W�F�N�g�ɕ`�悷��
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // �����_�[�^�[�Q�b�g���w�肷��
    glDrawBuffers(static_cast<GLsizei>(target.size()), target.data());

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

    // �ϊ��s���ݒ肷��
    simple.loadMatrix(mp, mv * window.getLeftTrackball());

    // �}�`��`�悷��
    object.draw(simple);

    // �ʏ�̃t���[���o�b�t�@�ɕ`�悷��
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // �o�b�N�o�b�t�@���w�肷��
    glDrawBuffer(GL_BACK);

    // �r���[�|�[�g��ݒ肷��
    window.setViewport();

    // �B�ʏ������s��Ȃ�
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // �x�����_�����O���s���V�F�[�_�̎g�p���J�n����
    glUseProgram(pass2);

    // �J���[�o�b�t�@�Ɏg�����e�N�X�`�����w�肷��
    for (int i = 0; i < colorCount; ++i)
    {
      glUniform1i(colorLoc + i, i);
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, color[i]);
    }

    // ���̃e�N�X�`�����w�肷��
    glUniform1i(imageLoc, colorCount);
    glActiveTexture(GL_TEXTURE0 + colorCount);
    glBindTexture(GL_TEXTURE_2D, image);

    // ���̃e�N�X�`���ɉ摜��]������
    camera.transmit();
    glGenerateMipmap(GL_TEXTURE_2D);

    // �@�������̃T���v���_�̐���ݒ肷��
    glUniform1i(diffuseSamplesLoc, diffuseSamples);

    // �~�b�v�}�b�v�̃��x����ݒ肷��
    glUniform1i(diffuseLodLoc, diffuseLod);

    // �����˕����̃T���v���_�̐���ݒ肷��
    glUniform1i(specularSamplesLoc, specularSamples);

    // �����˕����̃~�b�v�}�b�v�̃��x����ݒ肷��
    glUniform1i(specularLodLoc, specularLod);

    // �f�v�X�}�b�v���w�肷��
    glUniform1i(depthLoc, colorCount + 1);
    glActiveTexture(GL_TEXTURE0 + colorCount + 1);
    glBindTexture(GL_TEXTURE_2D, depth);

    // �T���v���_�̎U�z���a��ݒ肷��
    glUniform1f(radiusLoc, radius);

    // ���e�ϊ��s���ݒ肷��
    glUniformMatrix4fv(mpLoc, 1, GL_FALSE, mp.get());

    // �r���[�|�[�g�̒��S�ʒu�ƃX�N���[���̃X�P�[�����w�肷��
    const GLfloat centerX(static_cast<GLfloat>(window.getWidth()) * 0.5f);
    const GLfloat centerY(static_cast<GLfloat>(window.getHeight()) * 0.5f);
    const GLfloat scale((1.0f + window.getWheel() * 0.05f) / static_cast<GLfloat>(window.getHeight()));
    glUniform4f(viewportLoc, centerX, centerY, scale, scale);

    // �e�N�X�`���̉�]�s���ݒ肷��
    glUniformMatrix4fv(mtLoc, 1, GL_FALSE, window.getRightTrackball().get());

    // ��`��`��
    glBindVertexArray(rectangle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
