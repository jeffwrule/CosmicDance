#ifndef GENERICMP3_H
#define GENERICMP3_H


class GenericMP3 {

  public:
    virtual void mp3_stop() =0;
    virtual void mp3_fade(int fade_seconds) =0;
    virtual void mp3_normal_volume() =0;
    virtual boolean mp3_play_track(const char *track_name) =0;
    virtual void mp3_set_volume(int volume) =0;
    virtual int mp3_is_playing() =0;
    // virtual boolean track_is_complete() =0;
    virtual void do_beep(int num_millis, int delay_after) =0;
    virtual void status(boolean do_print) =0;
};



#endif
