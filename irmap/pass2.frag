#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass2.frag
//
//   �t���[���o�b�t�@�I�u�W�F�N�g�̓��e��`�悷��V�F�[�_
//

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec2 texcoord;                                     // �e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                    // �t���O�����g�̐F

// �J���[�̃����_�[�^�[�Q�b�g�̃e�N�X�`��
uniform sampler2D color;

void main(void)
{
  // ��f�̉A�e�����߂�
  fc = texture(color, texcoord);
}
