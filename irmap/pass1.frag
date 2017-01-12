#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass1.frag
//
//   �}���`�v�������_�[�^�[�Q�b�g�ɕ`�悷��V�F�[�_
//

// �ގ�
uniform vec4 kamb;                                    // �����̔��ˌW��
uniform vec4 kdiff;                                   // �g�U���ˌW��
uniform vec4 kspec;                                   // ���ʔ��ˌW��
uniform float kshi;                                   // �P���W��

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 p;                                            // ���_�̈ʒu
in vec3 n;                                            // ���_�̖@��

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 fresnel;
layout (location = 2) out vec3 position;
layout (location = 3) out vec3 normal;

void main(void)
{
  color = vec4(kdiff.rgb, kamb.a);
  fresnel = vec4(kspec.rgb, kshi * 0.0078125);
  position = p.xyz / p.w;
  normal = normalize(n);
}
