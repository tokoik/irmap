#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass1.frag
//
//   ��f�P�ʂɉA�e�t�����s���V�F�[�_
//

// ����
uniform vec4 lamb;                                    // ��������
uniform vec4 ldiff;                                   // �g�U���ˌ�����
uniform vec4 lspec;                                   // ���ʔ��ˌ�����
uniform vec4 lpos;                                    // �ʒu

// �ގ�
uniform vec4 kamb;                                    // �����̔��ˌW��
uniform vec4 kdiff;                                   // �g�U���ˌW��
uniform vec4 kspec;                                   // ���ʔ��ˌW��
uniform float kshi;                                   // �P���W��

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 p;                                            // ���_�̈ʒu
in vec3 n;                                            // ���_�̖@��

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                    // �t���O�����g�̐F

void main(void)
{
  // �A�e
  vec3 v = normalize(p.xyz / p.w);                    // �����x�N�g��
  vec3 l = normalize((lpos * p.w - p * lpos.w).xyz);  // �����x�N�g��
  vec3 h = normalize(l - v);                          // ���ԃx�N�g��

  // �g�U���ˌ�����
  vec4 idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;

  // ���ʔ��ˌ�����
  vec4 ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

  // ��f�̉A�e�����߂�
  fc = idiff + ispec;
}
