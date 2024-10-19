#ifndef LIMITER_API_H
#define LIMITER_API_H



int need_limiter_buf();
void limiter_init(void *work_buf, int *attackTime, int *releaseTime, int *threshold, int sample_rate, int channel);
void limiter_update_para(void *work_buf, int *attackTime, int *releaseTime, int *threshold, int sample_rate, int channel);

int limiter_run_16(void *work_buf, short *in_buf, short *out_buf, int per_channel_npoint);
int limiter_run_32(void *work_buf, int *in_buf, int *out_buf, int per_channel_npoint);


#endif // !LIMITER_API_H
