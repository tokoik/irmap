#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass2.frag
//
//   �t���[���o�b�t�@�I�u�W�F�N�g�̓��e��`�悷��V�F�[�_
//

#define NONE      0
#define PANORAMA  1
#define FISHEYE   2
#define THETA     3
#define MODE THETA

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

// ���̃e�N�X�`��
uniform sampler2D image;

// THETA: ���e�N�X�`���̃T�C�Y
vec2 size = textureSize(image, 0);

// THETA: ���e�N�X�`���̌���J�������̃e�N�X�`����ԏ�̔��a�ƒ��S
vec2 radius_b = vec2(-0.25, 0.25 * size.x / size.y);
vec2 center_b = vec2( 0.25, radius_b.t);

// THETA: ���e�N�X�`���̑O���J�������̃e�N�X�`����ԏ�̔��a�ƒ��S
vec2 radius_f = vec2( 0.25, radius_b.t);
vec2 center_f = vec2( 0.75, center_b.t);

// FISHEYE: �~���p�ŕ\������p�̋t��
//const float fisheye_fov = 0.31830989; // 180��
//const float fisheye_fov = 0.27813485; // 206��
const float fisheye_fov = 0.24381183; // 235��
//const float fisheye_fov = 0.21220659; // 270��

// ���}�b�v�̃T���v�����O
vec4 sample(in vec3 vector)
{
#if MODE == PANORAMA

  //
  // �����~���}�@�̃p�m���}�摜�̏ꍇ
  //

  // �o�x�����̃x�N�g��
  vec2 u = vector.xy;

  // �ܓx�����̃x�N�g��
  vec2 v = vec2(vector.z, length(vector.xz));

  // �ܓx�ƌo�x����e�N�X�`�����W�����߂�
  vec2 t = atan(u, v) * vec2(-0.15915494, -0.31830989) + 0.5;
  return texture(image, t);

#elif MODE == FISHEYE

  //
  // �������ˉe�����̋��჌���Y�摜�̏ꍇ
  //

  // �����Y����Ɍ������ꍇ�̃e�N�X�`�����W�����߂�
  vec2 t = fisheye_fov * acos(vector.y) * normalize(vector.xz) + 0.5;
  return texture(image, t);

#elif MODE == THETA

  //
  // RICOH THETA S �̃��C�u�X�g���[�~���O�摜�̏ꍇ
  //

  // ���̕����x�N�g���̑��ΓI�ȋp
  float angle = 1.0 - acos(vector.z) * 0.63661977;

  // �O��̃e�N�X�`���̍�����
  float blend = smoothstep(-0.02, 0.02, angle);

  // ���̕����x�N�g���� yx ��ł̕����x�N�g��
  vec2 orientation = normalize(vector.yx) * 0.885;

  // ���ƕ\�̃e�N�X�`�����W�����߂�
  vec2 t_b = (1.0 - angle) * orientation * radius_b + center_b;
  vec2 t_f = (1.0 + angle) * orientation * radius_f + center_f;

  // ���ƕ\�̊��}�b�v���T���v�����O����
  vec4 color_b = texture(image, t_b);
  vec4 color_f = texture(image, t_f);

  // �T���v�����O�����F���u�����h����
  return mix(color_f, color_b, blend);
#else

  // ���̂܂܃e�N�X�`����\��
  return texture(image, texcoord);

#endif
}

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

  // �����˕���
  vec3 r = reflect(v, n);

  // �����˕����̐F
  vec4 s = sample(r);

  // ��f�̉A�e�����߂�
  fc = mix(idiff, s, kspec) + ispec;
}
