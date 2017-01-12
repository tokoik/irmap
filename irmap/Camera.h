#pragma once

//
// �J�����֘A�̏���
//

// �⏕�v���O����
#include "gg.h"
using namespace gg;

// �L���v�`����񓯊��ōs��
#include <thread>
#include <mutex>

//
// �J�����֘A�̏�����S������N���X
//
class Camera
{
  // �R�s�[�R���X�g���N�^�𕕂���
  Camera(const Camera &c);

  // ����𕕂���
  Camera &operator=(const Camera &w);

protected:

  // �L���v�`�������摜
  GLubyte *buffer;

  // �L���v�`�������摜�̕��ƍ���
  GLsizei width, height;

  // �L���v�`�������摜�̃t�H�[�}�b�g
  GLenum format;

  // �X���b�h
  std::thread thr;

  // �~���[�e�b�N�X
  std::mutex mtx;

  // ���s���
  bool run;

  // �t���[�����L���v�`������
  virtual void capture() {};

public:

  // �R���X�g���N�^
  Camera()
  {
    // �摜���܂��擾����Ă��Ȃ����Ƃ��L�^���Ă���
    buffer = nullptr;

    // �X���b�h����~��Ԃł��邱�Ƃ��L�^���Ă���
    run = false;
  }

  // �f�X�g���N�^
  virtual ~Camera()
  {
  }

  // �X���b�h���N������
  void start()
  {
    // �X���b�h���N����Ԃł��邱�Ƃ��L�^���Ă���
    run = true;

    // �X���b�h���N������
    thr = std::thread([this](){ this->capture(); });
  }

  // �X���b�h���~����
  void stop()
  {
    // �L���v�`���X���b�h�����s���Ȃ�
    if (run)
    {
      // �L���v�`���f�o�C�X�����b�N����
      mtx.lock();

      // �L���v�`���X���b�h�̃��[�v���~�߂�
      run = false;

      // ���b�N��������
      mtx.unlock();

      // ��������
      thr.join();
    }
  }

  // �摜�̕��𓾂�
  int getWidth() const
  {
    return width;
  }

  // �摜�̍����𓾂�
  int getHeight() const
  {
    return height;
  }

  // Ovrvision Pro �̘I�o���グ��
  virtual void increaseExposure() {};

  // Ovrvision Pro �̘I�o��������
  virtual void decreaseExposure() {};

  // Ovrvision Pro �̗������グ��
  virtual void increaseGain() {};

  // Ovrvision Pro �̗�����������
  virtual void decreaseGain() {};

  // �J���������b�N���ĉ摜���e�N�X�`���ɓ]������
  void transmit()
  {
    // �J�����̃��b�N�����݂�
    if (mtx.try_lock())
    {
      // �V�����f�[�^���������Ă�����
      if (buffer)
      {
        // �f�[�^���e�N�X�`���ɓ]������
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, buffer);

        // �f�[�^�̓]���������L�^����
        buffer = nullptr;
      }

      // ���J�����̃��b�N����������
      mtx.unlock();
    }
  }
};
