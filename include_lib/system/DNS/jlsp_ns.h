#ifndef __NS_H__
#define __NS_H__

/*
gain_floor: 增益的最小值控制,范围0~1,建议值(0~0.2)之间
over_drive: 控制降噪强度:
0 < over_drive < 1，越小降噪强度越轻，太小噪声会很大；
over_drive = 1,正常降噪
over_drive > 1,降噪强度加强，越大降噪强度越强，太大会吃音
建议调节范围0.3~3之间来控制降噪强度的强弱
>>>>>>> 6ad659bce349002a2a7b7ae9b39900a9efcecee4
*/

void *JLSP_ns_init(char *private_buffer, char *shared_buffer, float gain_floor, float over_drive, float loudness, int samplerate, const int is_dual);
int JLSP_ns_get_heap_size(int *private_size, int *shared_size, int samplerate, int is_dual);

int JLSP_ns_reset(void *m);
void JLSP_ns_update_shared_buffer(void *m, char *shared_buffer);
int JLSP_ns_process(void *m, void *input, void *output, int *outsize);
int JLSP_ns_free(void *m);

void JLSP_ns_set_noiselevel(void *m, float noise_level_init);


#endif
