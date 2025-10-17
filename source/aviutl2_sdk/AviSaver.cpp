//----------------------------------------------------------------------------------
//	�T���v��AVI(vfw�o�R)�o�̓v���O�C�� for AviUtl ExEdit2
//----------------------------------------------------------------------------------
#include <windows.h>
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

#include "output2.h"

bool func_output(OUTPUT_INFO* oip);
bool func_config(HWND hwnd, HINSTANCE dll_hinst);
LPCWSTR func_get_config_text();

//---------------------------------------------------------------------
//	�o�̓v���O�C���\���̒�`
//---------------------------------------------------------------------
OUTPUT_PLUGIN_TABLE output_plugin_table = {
	OUTPUT_PLUGIN_TABLE::FLAG_VIDEO | OUTPUT_PLUGIN_TABLE::FLAG_AUDIO, //	�t���O
	L"AVI File Saver2 (sample)",					// �v���O�C���̖��O
	L"AviFile (*.avi)\0*.avi\0",					// �o�̓t�@�C���̃t�B���^
	L"Sample AVI File Saver2 version 2.01 By �j�d�m����",	// �v���O�C���̏��
	func_output,									// �o�͎��ɌĂ΂��֐��ւ̃|�C���^
	func_config,									// �o�͐ݒ�̃_�C�A���O��v�����ꂽ���ɌĂ΂��֐��ւ̃|�C���^ (nullptr�Ȃ�Ă΂�܂���)
	func_get_config_text,							// �o�͐ݒ�̃e�L�X�g�����擾���鎞�ɌĂ΂��֐��ւ̃|�C���^ (nullptr�Ȃ�Ă΂�܂���)
};

//---------------------------------------------------------------------
//	�v���O�C��DLL�������֐� (����`�Ȃ�Ă΂�܂���)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) bool InitializePlugin(DWORD version) { // version�͖{�̂̃o�[�W�����ԍ�
	return true;
}

//---------------------------------------------------------------------
//	�v���O�C��DLL�I���֐� (����`�Ȃ�Ă΂�܂���)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void UninitializePlugin() {
}

//---------------------------------------------------------------------
//	�o�̓v���O�C���\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) OUTPUT_PLUGIN_TABLE* GetOutputPluginTable(void)
{
	return &output_plugin_table;
}

//---------------------------------------------------------------------
//	AVI�t�@�C���n���h���\����
//---------------------------------------------------------------------
struct AVI_HANDLE {
	PAVIFILE	pfile = nullptr;
	PAVISTREAM	pvideo = nullptr;
	PAVISTREAM	paudio = nullptr;
	~AVI_HANDLE() {
		if (paudio) AVIStreamRelease(paudio);
		if (pvideo) AVIStreamRelease(pvideo);
		if (pfile) AVIFileRelease(pfile);
	}
};

//---------------------------------------------------------------------
//	�o�̓v���O�C���o�͊֐�
//---------------------------------------------------------------------
bool func_output(OUTPUT_INFO* oip) {
	AVI_HANDLE avi;
	if (AVIFileOpen(&avi.pfile, oip->savefile, OF_WRITE | OF_CREATE, NULL) != S_OK) {
		return false;
	}

	// �r�f�I�X�g���[���̐ݒ�
	AVISTREAMINFO video{};
	video.fccType = streamtypeVIDEO;
	video.fccHandler = BI_RGB;
	video.dwRate = oip->rate;
	video.dwScale = oip->scale;
	video.rcFrame.right = oip->w;
	video.rcFrame.bottom = oip->h;
	if (AVIFileCreateStream(avi.pfile, &avi.pvideo, &video) != S_OK) {
		return false;
	}
	BITMAPINFOHEADER bmih{};
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = oip->w;
	bmih.biHeight = oip->h;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = oip->w * oip->h * 3;
	if (AVIStreamSetFormat(avi.pvideo, 0, &bmih, sizeof(bmih)) != S_OK) {
		return false;
	}

	// �I�[�f�B�I�X�g���[���̐ݒ�
	AVISTREAMINFO audio{};
	audio.fccType = streamtypeAUDIO;
	audio.fccHandler = WAVE_FORMAT_PCM;
	audio.dwSampleSize = oip->audio_ch * 2;
	audio.dwRate = oip->audio_rate * audio.dwSampleSize;
	audio.dwScale = audio.dwSampleSize;
	if (AVIFileCreateStream(avi.pfile, &avi.paudio, &audio) != S_OK) {
		return false;
	}
	WAVEFORMATEX wf{};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = oip->audio_ch;
	wf.nSamplesPerSec = oip->audio_rate;
	wf.wBitsPerSample = 16;
	wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	if (AVIStreamSetFormat(avi.paudio, 0, &wf, sizeof(wf)) != S_OK) {
		return false;
	}

	oip->func_set_buffer_size(5, 10); // �f�[�^�擾�o�b�t�@����ύX

	for (int frame = 0; frame < oip->n; frame++) {
		oip->func_rest_time_disp(frame, oip->n); // �c�莞�Ԃ�\��
		if (oip->func_is_abort()) break; // ���f�̊m�F

		// �r�f�I�̏�������
		void* data = oip->func_get_video(frame, BI_RGB);
		if (AVIStreamWrite(avi.pvideo, frame, 1, data, bmih.biSizeImage, AVIIF_KEYFRAME, NULL, NULL) != S_OK) {
			break;
		}

		// �I�[�f�B�I�̏�������
		int audioPos = (int)((double)frame / video.dwRate * video.dwScale * oip->audio_rate);
		int audioNum = (int)((double)(frame + 1) / video.dwRate * video.dwScale * oip->audio_rate) - audioPos;
		int audioReaded = 0;
		data = oip->func_get_audio(audioPos, audioNum, &audioReaded, WAVE_FORMAT_PCM);
		if (audioReaded == 0) continue;
		if (AVIStreamWrite(avi.paudio, audioPos, audioReaded, data, audioReaded * wf.nBlockAlign, 0, NULL, NULL) != S_OK) {
			break;
		}
	}

	return true;
}

//---------------------------------------------------------------------
//	�ݒ�_�C�A���O
//---------------------------------------------------------------------
bool func_config(HWND hwnd, HINSTANCE dll_hinst) {
	MessageBox(hwnd, L"�T���v���_�C�A���O", L"�o�͐ݒ�", MB_OK);

	// DLL���J������Ă��ݒ肪�c��悤�ɕۑ����Ă����Ă�������

	return true;
}

//---------------------------------------------------------------------
//	�o�͐ݒ�̃e�L�X�g��� (�ݒ�{�^���ׂ̗ɕ\�������)
//---------------------------------------------------------------------
LPCWSTR func_get_config_text() {
	return L"�T���v���ݒ���";
}
