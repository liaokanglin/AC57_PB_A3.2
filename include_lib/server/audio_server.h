#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include "fs/fs.h"

enum {
    AUDIO_SERVER_EVENT_CURR_TIME = 0x20,
    AUDIO_SERVER_EVENT_END,
    AUDIO_SERVER_EVENT_ERR,
    AUDIO_SERVER_EVENT_SPEAK_START,
    AUDIO_SERVER_EVENT_SPEAK_STOP,
};

#define AUDIO_SERVER_EVENT_DEC_END   0x20
#define AUDIO_SERVER_EVENT_ENC_ERR   0x30

#define AUDIO_REQ_DEC            0
#define AUDIO_REQ_ENC            1
#define AUDIO_REQ_GET_INFO       2

#define AUDIO_DEC_OPEN                 0
#define AUDIO_DEC_START                1
#define AUDIO_DEC_PAUSE                2
#define AUDIO_DEC_STOP                 3
#define AUDIO_DEC_FF                   4
#define AUDIO_DEC_FR                   5
#define AUDIO_DEC_GET_BREAKPOINT       6
#define AUDIO_DEC_PP                   7
#define AUDIO_DEC_SET_VOLUME           8


#define AUDIO_ENC_OPEN      4
#define AUDIO_ENC_START     5
#define AUDIO_ENC_PAUSE     6
#define AUDIO_ENC_STOP      7
#define AUDIO_ENC_CLOSE     8

struct audio_finfo {
    u8 channel;
    u8 name_code;
    int sample_rate;
    int bit_rate;
    u32 total_time;
};

struct audio_vfs_ops {
    void *(*fopen)(const char *path, const char *mode);
    int (*fread)(void *file, void *buf, u32 len);
    int (*fwrite)(void *file, void *buf, u32 len);
    int (*fseek)(void *file, u32 offset, int seek_mode);
    int (*ftell)(void *file);
    int (*flen)(void *file);
    int (*fclose)(void *file);
};



struct audio_dec_output_ops {
    void *(*get_buf)(void *priv, u32 *len);
    void (*put_buf)(void *priv, void *buf, u32 len);
    const struct audio_vfs_ops  *vfs_ops;
};

struct audio_decoder_ops {
    const char *name;
    void *(*open)(FILE *file, void *priv, const struct audio_dec_output_ops *ops);
    int (*get_audio_info)(void *, struct audio_finfo *info);
    int (*start)(void *);
    int (*close)(void *);

};



struct audio_dec_req {
    u8 cmd;
    u8 channel;
    u8 volume;
    u8 priority;
    u16 output_buf_len;
    u32 sample_rate;
    void *output_buf;
    FILE *file;
    char *type;
    u32 total_time;
    const char *sample_source;
    const struct audio_vfs_ops *vfs_ops;

};


struct audio_server {
    void *private_data;
    u32 data[0];
};

struct audio_enc_req {
    u8 cmd;
    u8 channel;
    u8 volume;
    u8 priority;
    u16 input_buf_len;
    u32 sample_rate;
    void *input_buf;
    char *wformat;//编码格式
    char *name;
    u8 audio_type;
    u32 frame_len;
    const char *sample_source;
    int (*callback)(u8 *buf, u32 len);
};

union audio_req {
    struct audio_dec_req dec;
    struct audio_enc_req enc;
};


#define REGISTER_AUDIO_DECODER(ops) \
        const struct audio_decoder_ops ops sec(.audio_decoder)


extern const struct audio_decoder_ops audio_decoder_begin[];
extern const struct audio_decoder_ops audio_decoder_end[];

#define list_for_each_audio_decoder(p) \
    for (p = audio_decoder_begin; p < audio_decoder_end; p++)
















#endif
