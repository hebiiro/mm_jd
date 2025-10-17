//----------------------------------------------------------------------------------
//	�T���v���t�B���^�v���O�C��(���f�B�A�I�u�W�F�N�g) for AviUtl ExEdit2
//----------------------------------------------------------------------------------
#include <windows.h>
#include <memory>
#include <algorithm>

#include "filter2.h"

bool func_proc_video(FILTER_PROC_VIDEO* video);
bool func_proc_audio(FILTER_PROC_AUDIO* audio);

//---------------------------------------------------------------------
//	�t�B���^�ݒ荀�ڒ�`
//---------------------------------------------------------------------
auto width = FILTER_ITEM_TRACK(L"��", 100, 1, 1000);
auto height = FILTER_ITEM_TRACK(L"�c", 100, 1, 1000);
auto color = FILTER_ITEM_COLOR(L"�F", 0xffffff);
auto frequency = FILTER_ITEM_TRACK(L"���g��", 1000, 1, 24000);
void* items[] = { &width, &height, &color, &frequency, nullptr };

//---------------------------------------------------------------------
//	�t�B���^�v���O�C���\���̒�`
//---------------------------------------------------------------------
FILTER_PLUGIN_TABLE filter_plugin_table = {
	FILTER_PLUGIN_TABLE::FLAG_VIDEO | FILTER_PLUGIN_TABLE::FLAG_AUDIO | FILTER_PLUGIN_TABLE::FLAG_INPUT, //	�t���O
	L"MediaObject(sample)",							// �v���O�C���̖��O
	L"�T���v��",									// ���x���̏����l (nullptr�Ȃ�f�t�H���g�̃��x���ɂȂ�܂�)
	L"Sample MediaObject version 2.00 By �j�d�m����",	// �v���O�C���̏��
	items,											// �ݒ荀�ڂ̒�` (FILTER_ITEM_XXX�|�C���^��񋓂���null�I�[�������X�g�ւ̃|�C���^)
	func_proc_video,								// �摜�t�B���^�����֐��ւ̃|�C���^ (FLAG_VIDEO���L���̎��̂݌Ă΂�܂�)
	func_proc_audio									// �����t�B���^�����֐��ւ̃|�C���^ (FLAG_AUDIO���L���̎��̂݌Ă΂�܂�)
};

//---------------------------------------------------------------------
//	�v���O�C��DLL�������֐� (����`�Ȃ�Ă΂�܂���)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) bool InitializePlugin(DWORD version) { // version�͖{�̂̃o�[�W�����ԍ�
	return true;
}

//---------------------------------------------------------------------
//	�v���O�C��DLL����֐� (����`�Ȃ�Ă΂�܂���)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void UninitializePlugin() {
}

//---------------------------------------------------------------------
//	�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) FILTER_PLUGIN_TABLE* GetFilterPluginTable(void) {
	return &filter_plugin_table;
}

//---------------------------------------------------------------------
//	�摜�t�B���^����
//---------------------------------------------------------------------
bool func_proc_video(FILTER_PROC_VIDEO* video) {
	auto w = (int)width.value;
	auto h = (int)height.value;
	if (w <= 0 || h <= 0) return false;

	// �w��T�C�Y�A�F�̎l�p�`�̉摜�f�[�^���쐬
	auto col = color.value;
	auto buffer = std::make_unique<PIXEL_RGBA[]>(w * h);
	auto p = buffer.get();
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			p->r = col.r;
			p->g = col.g;
			p->b = col.b;
			p->a = 255;
			p++;
		}
	}

	video->set_image_data(buffer.get(), w, h);
	return true;
}

//---------------------------------------------------------------------
//	�����t�B���^����
//---------------------------------------------------------------------
bool func_proc_audio(FILTER_PROC_AUDIO* audio) {
	auto sample_index = audio->object->sample_index;
	auto sample_num = audio->object->sample_num;
	auto channel_num = audio->object->channel_num;

	// �w����g���̃T�C���g�̉����f�[�^���쐬
	auto step = (3.141592653589793 * 2.0) * frequency.value / audio->scene->sample_rate;
	auto buffer = std::make_unique<float[]>(sample_num);
	auto p = buffer.get();
	for (int i = 0; i < sample_num; i++) {
		*p++ = (float)sin(sample_index++ * step);
	}

	for (int i = 0; i < channel_num; i++) {
		audio->set_sample_data(buffer.get(), i);
	}
	return true;
}
