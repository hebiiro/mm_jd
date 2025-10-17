//----------------------------------------------------------------------------------
//	�T���v���t�B���^�v���O�C��(�t�B���^����) for AviUtl ExEdit2
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
auto luminance = FILTER_ITEM_TRACK(L"���邳", 1.0, 0.0, 2.0, 0.01);
FILTER_ITEM_SELECT::ITEM component_list[] = { { L"R�����̂�", 1 }, { L"G�����̂�", 2 }, { L"B�����̂�", 4 }, { L"RGB����", 7 }, { nullptr } };
auto component = FILTER_ITEM_SELECT(L"�Ώ�", 7, component_list);
auto volume = FILTER_ITEM_TRACK(L"����", 1.0, 0.0, 2.0, 0.01);
auto mono = FILTER_ITEM_CHECK(L"���m������", false);
void* items[] = { &luminance, &component, &volume, &mono, nullptr };

//---------------------------------------------------------------------
//	�t�B���^�v���O�C���\���̒�`
//---------------------------------------------------------------------
FILTER_PLUGIN_TABLE filter_plugin_table = {
	FILTER_PLUGIN_TABLE::FLAG_VIDEO | FILTER_PLUGIN_TABLE::FLAG_AUDIO, //	�t���O
	L"MediaFilter(sample)",							// �v���O�C���̖��O
	L"�T���v��",									// ���x���̏����l (nullptr�Ȃ�f�t�H���g�̃��x���ɂȂ�܂�)
	L"Sample MediaFilter version 2.00 By �j�d�m����",	// �v���O�C���̏��
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
	auto w = video->object->width;
	auto h = video->object->height;
	auto buffer = std::make_unique<PIXEL_RGBA[]>(w * h);
	video->get_image_data(buffer.get());

	// �w���RGB�����̖��邳�𒲐�
	auto r = (component.value & 1) ? luminance.value : 1.0;
	auto g = (component.value & 2) ? luminance.value : 1.0;
	auto b = (component.value & 4) ? luminance.value : 1.0;
	auto p = buffer.get();
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			p->r = (unsigned char)std::clamp(p->r * r, 0.0, 255.0);
			p->g = (unsigned char)std::clamp(p->g * g, 0.0, 255.0);
			p->b = (unsigned char)std::clamp(p->b * b, 0.0, 255.0);
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
	auto num = audio->object->sample_num;
	auto buffer0 = std::make_unique<float[]>(num);
	auto buffer1 = std::make_unique<float[]>(num);
	audio->get_sample_data(buffer0.get(), 0);
	audio->get_sample_data(buffer1.get(), 1);

	// ���ʂ𒲐�
	auto v = (float)volume.value;
	auto p0 = buffer0.get();
	auto p1 = buffer1.get();
	for (int i = 0; i < num; i++) {
		*p0++ *= v;
		*p1++ *= v;
	}

	// ���m������
	p0 = buffer0.get();
	p1 = buffer1.get();
	if (mono.value) {
		for (int i = 0; i < num; i++) {
			*p0++ += *p1++;
		}
		audio->set_sample_data(buffer0.get(), 0);
		audio->set_sample_data(buffer0.get(), 1);
	} else {
		audio->set_sample_data(buffer0.get(), 0);
		audio->set_sample_data(buffer1.get(), 1);
	}
	return true;
}
