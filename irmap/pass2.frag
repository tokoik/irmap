#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass2.frag
//
//   �t���[���o�b�t�@�I�u�W�F�N�g�̓��e��`�悷��V�F�[�_
//

// ����
uniform vec4 lamb;                                    // ��������
uniform vec4 ldiff;                                   // �g�U���ˌ�����
uniform vec4 lspec;                                   // ���ʔ��ˌ�����
uniform vec4 lpos;                                    // �ʒu

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec2 texcoord;                                     // �e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                    // �t���O�����g�̐F

// �J���[�̃����_�[�^�[�Q�b�g�̃e�N�X�`��
uniform sampler2D color[4];


void main(void)
{
  vec3 p = texture(color[2], texcoord).xyz;           // ���_�̈ʒu
  vec3 n = texture(color[3], texcoord).xyz;           // ���_�̖@��

  // �A�e
  vec3 v = normalize(p);                              // �����x�N�g��
  vec3 l = normalize(lpos.xyz - p * lpos.w);          // �����x�N�g��
  vec3 h = normalize(l - v);                          // ���ԃx�N�g��

  // �g�U���ˌ�����
  vec4 kdiff = texture(color[0], texcoord);
  vec4 idiff = kdiff * (max(dot(n, l), 0.0) * ldiff + lamb);

  // ���ʔ��ˌ�����
  vec4 kspec = texture(color[1], texcoord);
  float kshi = kspec.a * 128.0;
  vec4 ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

  // ��f�̉A�e�����߂�
  fc = idiff + ispec;
}
