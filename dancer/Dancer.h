#ifndef DANCER_H
#define DANCER_H


class Dancer {

  public:
    void status(boolean do_print);  // dump our current status 
    void update();                  // check for fob and director
    void stop_all();                // stop all the devices and the dance
    void start_dancing();           // start the dance sequence
//    void reply_status();          // update the director with a dancing or fnished message (based on current dancing state)
//    void signal_if_done();        // let the director know we finished
    void signal_every_so_often(); // send back a message to the director from time to time that we are dancing
//    void clear_pending_direction();            // clear out all pending remote direction()
    void reset();                   // reset the dancer state back to not dancing
    void send_dancing();            // notify director we are dancing
    void send_finished();           // notify director we are finished
    void dance();                   // keep the dancer dancing

    //  PrintTimer *pt                    time used by this dancer
    //  Fob *fob                          the fob used by this Dancer
    //  Director *director                the associated director structure
    //  GenericMP3 *mp3                   mp3 device (used to play musice)
    //  GenericDevice *d_list             list of connected devices (pins)
    //  unsigned int num_devices          number of devices in the list
    //  const char *solo_track_name       Name of the solo track on the microSD card
    //  const char *ensembl_track_name    Name of the ensemble track on the microSD card
    //  unsigned int dance_delay_seconds  number of seconds to delay between director/fob start and music start
    //  unsigned int fob_delay_seconds    number of seconds to delay between plays when in fob mode
    Dancer(
        PrintTimer *pt,
        Fob *fob, 
        Director *director, 
        GenericMP3 *mp3, 
        GenericDevice **d_list, 
        unsigned int num_devices,
        const char *solo_track_name,
        const char *ensembl_track_name,
        unsigned int dance_delay_seconds,
        unsigned int fob_delay_seconds ) :
      pt(pt),
      fob(fob),
      director(director),
      mp3(mp3),
      d_list(d_list),
      num_devices(num_devices),
      solo_track_name(solo_track_name),
      ensembl_track_name(ensembl_track_name),
      dance_delay_millis(dance_delay_seconds*1000),
      fob_delay_millis(fob_delay_seconds * 1000),
      num_dances(0), 
      num_plays(0),
      current_direction(empty_request), 
      has_new_direction(false), 
      checkin_millis(CHECK_IN_SECONDS * 1000), 
      current_track_name(solo_track_name),
      dance_type(solo),
      dance_started_millis(0), 
      fob_delay_start_millis(0),
      last_checkin_millis(0)      
      {

        Serial.print(F("Dancer::CONSTRUCTOR num_devices="));
        Serial.print(num_devices);
        Serial.print(F(", solo_track_name="));
        Serial.print(solo_track_name);
        Serial.print(F(", ensembl_track_name="));
        Serial.print(ensembl_track_name);
        Serial.print(F(", dance_delay_seconds="));
        Serial.print(dance_delay_seconds);
        Serial.print(F(", fob_delay_seconds="));
        Serial.println(fob_delay_seconds);
        
      }

  private:
    // progression of states for the dancer
    boolean is_dancing;           // we have a dance request from the director or the fob
    boolean is_pre_delayed;       // we are in pre delay time (delay before music starts)
    boolean is_play_started;      // solo or ensembl music playback has started
    boolean is_play_complete;     // has the music finished
    boolean is_fob_delayed;       // fob post delay has started

    boolean fob_is_dancing;         // current state of the fob when we last checked
    
    PrintTimer *pt;                 // our print and global timer
    Fob *fob;                       // the fob switch
    Director *director;             // our pointer to the director
    GenericMP3 *mp3;                // our mp3 device
    GenericDevice **d_list;          // pointer to our devices
    unsigned int num_devices;       // number of devices in the list
      
    unsigned char current_direction;    // the most recent direction we got from the director or fob
    boolean has_new_direction;      //
    unsigned int num_dances;                 // increment for a director or fob request
    unsigned int num_plays;                  // how many times have we danced in total incremented when music starts
    unsigned char dance_type;       // s (solo) or e (ensembl)
    const char *solo_track_name;         // solo track name
    const char *ensembl_track_name;      // ensembl track name
    char *current_track_name;     // pointer to the name of the track that is playing
    
    //  pre dance delay timers
    unsigned long dance_delay_millis;       // the amount of time to delay the start of the dance
    unsigned long dance_started_millis;     // what time the dance started (not the music play but the overall dance)
    
    unsigned long fob_delay_millis;         // amout to delay between solo plays
    unsigned long fob_delay_start_millis;   // when did the fob_delay start

    unsigned long checkin_millis;           // how long to delay between checkings (when dancing)
    unsigned long last_checkin_millis;      // when did we last send our status to the director.      
    
    // commands from the director
    static constexpr unsigned char solo = 's';           // dance a solo
    static constexpr unsigned char ensembl = 'e';        // everyone dance
    static constexpr unsigned char halt = 'h';           // everyone stop what they are doing
    static constexpr unsigned char whatchadoing = 'w';   // are you dancing or not?
    static constexpr unsigned char depricated_mini_track = 'm';     // play the sort track, track001 and track 002
    static constexpr unsigned char depricated_long_track = 'l';     // play the full version of the songs track010 and track020

    // dancer responses 
    static constexpr unsigned char finished = 'f';       // peice has been completed
    static constexpr unsigned char acknowledge = 'a';    // I head you!
    static constexpr unsigned char dancing = 'd';        // dancing

    // no command 
    static constexpr unsigned char empty_request = '\0';  // not a request

};

// keep the dancer dancing
// when this block is finished we can have is_dancing set or cleared if the fob was pushed
// we can also have new direction from 
void Dancer::update() {

  // bounds checking on the timer...
  if (dance_started_millis > pt->current_millis) { dance_started_millis = 0; }
  if (fob_delay_start_millis > pt->current_millis) { fob_delay_start_millis = 0; }
  if (last_checkin_millis > pt->current_millis) { last_checkin_millis = 0; }
  
  // state change fob switched ON
  if (!fob_is_dancing && fob->fob_is_dancing) {
    Serial.print(F("Dancer::update() fob pushed, fob_is_dancing=true"));
    fob_is_dancing = true;
    current_direction = solo;
    has_new_direction = true;
    if (!fob->get_seat_switch()) {
      mp3->do_beep(200, 50);
    }    
    delay(1500); // stop the bouncy switch
  }

  // state change fob switched OFF
  if (fob_is_dancing && !fob->fob_is_dancing) {
    Serial.println(F("Dancer::update() fob pushed, fob_is_dancing=false"));
    director->ignore_pending();
    fob_is_dancing = false;
    current_direction = halt;
    has_new_direction = true;
    if (!fob->get_seat_switch()) {
      mp3->do_beep(200, 50);
      mp3->do_beep(200, 50);
    }
    delay(1500); // stop the bouncy switch
  }

  // ignore direction when the fob is playing
  if (!fob_is_dancing && !fob->get_seat_switch()) {
    // has something new arrived?
    if (director->has_new_direction()) {
      if (has_new_direction == false) {
        current_direction = director->get_latest_direction();
        has_new_direction = true;
      } else {
        if (pt->get_do_print()) {
          // new direction available but old direction not consumed...
          Serial.print(F("WARNING: Dancer::update() new direction available but last direction not processed has_new_direction="));
          Serial.print(bool_tostr(has_new_direction));
          Serial.print(F(", current_direction="));
          Serial.println((char)current_direction);
        }
      } // empty_request
    } // has_new_direction
  }  // !fob_is_dancing
}  
    
void Dancer::dance() {    

  if (has_new_direction) {
    Serial.print(F("Dancer::dance() new_direction="));
    Serial.println((char)current_direction);
    
    switch (current_direction) {
      case halt: 
                      stop_all(); reset(); break;
      case solo: 
                      dance_type = solo; start_dancing(); break;
      case ensembl: 
                      dance_type = ensembl; start_dancing(); break;
      case whatchadoing:
                      if (is_dancing) { send_dancing(); } else { send_finished(); }
                      break;
      case depricated_mini_track:
      case depricated_long_track:
                      break;  // now ignoring mini/long
    }
    // make sure we clear both of these so we can process the next one
    has_new_direction = false;  
  }

  // if dancing && not pre_delayed; check for delay
  if (is_dancing && !is_pre_delayed) {
    if (pt->current_millis - dance_started_millis >= dance_delay_millis) {
      is_pre_delayed = true;
      Serial.println("Dancer::dance() PRE_DELAY COMPLETE"); 
    }
  }

  // delay is complete but not playing, start the MP3
  if (is_pre_delayed && !is_play_started) {
    Serial.print(F("Dancer::dance() STARTING MP3 PLAYING track_name="));
    Serial.println(current_track_name);
    boolean mp3_started = mp3->mp3_play_track(current_track_name);
    if (!mp3_started) {
      Serial.println(F("ERROR: starting mp3, resetting"));
      stop_all();
      reset();
      send_finished(); 
    }
    is_play_started = true;
    num_plays++;
  }

  // We are playing, Is the MP3 complete yet?
  if (is_play_started && !is_play_complete) {
    // try to start the track
    if (!mp3->mp3_is_playing()) {
      Serial.println(F("Dancer::dance() mp3 finished playing"));
      is_play_complete = true;
      fob_delay_start_millis = pt->current_millis;
      if (!fob_is_dancing) { // don't halt the devices if we are in fob play mode; they only halt when the fob is turned off
        stop_all();
        send_finished();
      }
    }
    
    // checkin from time to time while dancing, and not on fob play mode
    if (!fob_is_dancing && !fob->get_seat_switch()) {
      if (pt->current_millis - last_checkin_millis >= checkin_millis) {
        Serial.println(F("Dancer:dance() DANCER CHECKING IN..."));
        send_dancing();
      }      
    }
    
  }

  if (is_play_complete && !is_fob_delayed) {
    if (pt->current_millis - fob_delay_start_millis > fob_delay_millis) {
      is_fob_delayed = true;
      Serial.println(F("Dancer::dance() FOB DELAYED COMPLETED"));
    }
  }

  // we are done wrap it up one of two ways.
  if (is_fob_delayed) {
    if (fob_is_dancing) {
      reset();
      current_direction = solo;
      has_new_direction = true; 
      Serial.println(F("Dancer::dance() dance completed RESETTING FOR NEW DANCE"));
    } else { // fob is not dancing time to wrap it up
      reset();
      send_finished();
    }
  }
}


void Dancer::reset() {
        is_dancing = false;
        is_pre_delayed = false;
        is_play_started = false;
        is_play_complete = false;
        is_fob_delayed=false;
}

// stop the music and motor
void Dancer::stop_all() {
  
  #if defined IS_BRIEF
    Serial.println(F("stop_all begin..."));
  #endif
  
  // stop any attached devices
  for (int i=0; i<num_devices; i++) {
    d_list[i]->stop();
  }
  reset();                // clear our Dance state(s) back to not dancing
  mp3->mp3_stop();            // stop the music
  
  #if defined IS_CHATTY
    Serial.println(F("stop_all end..."));
  #endif
}


// start playing the solo or the ensembl
void Dancer::start_dancing() {
  
  #if defined IS_BRIEF
    Serial.print(F("start_dancing starting: dance_type="));
    Serial.print((char)dance_type);
    num_dances += 1;
    Serial.print(F(" total dances: "));
    Serial.println(num_dances);
  #endif

  stop_all();
  reset();
  is_dancing=true;

  // setup the dance track name to make the play eaiser
  if (dance_type == 'e' ) { 
      current_track_name = ensembl_track_name;
  } else {
    current_track_name = solo_track_name;
  }

  dance_started_millis = pt->current_millis;
  last_checkin_millis = 0;
  send_dancing();
  
  for (int i=0; i<num_devices; i++) {
    d_list[i]->start();
  }

  #if defined IS_CHATTY
    Serial.print(F(", dance_type="));
    Serial.print((char)dance_type);
    Serial.print(F(", current_dance_track="));
    Serial.println(current_track_name);
    Serial.println(F("start_dancing end"));
  #endif
  
//  // don't write a response about starting file fobA is on
//  if (fob_is_dancing) {
//    #if defined IS_CHATTY
//      Serial.println(F("fobA is on, skip sending dancing status to director"));
//    #endif
//  } else {
//    xbee_write(dancing);
//  }
//  switch (dance_piece) {
//    case solo:     my_track = 1 * play_mode; break;
//    case ensembl:  my_track = 2 * play_mode; break;
//    default: my_track = 0;
//  }
//  sprintf(track_name, "track%03d.%s", my_track, file_type);
//  #if defined IS_BRIEF
//    Serial.print(F("Starting Track:"));
//    Serial.println(track_name);
//  #endif
//  int result = mp3_play_track(dance_piece);
//  #ifdef HAS_ADAFRUIT_MP3
//    if (result != 1 ) {
//      Serial.print(F("Failed to start mp3track"));    
//    }
//  #else
//    if(result != 0) {
//      Serial.print(F("Error code: "));
//      Serial.print(result);
//      Serial.println(F(" when trying to play track"));
//    }
//  #endif // HAS_ADAFRUIT_MP3
//  delay(300); // this seems to stick sometimes when we start dancing
//  if (fob_is_dancing && ENHANCED_STANDALONE && fob_next_volume == FOB_QUIET_VOLUME) {
//    #if defined IS_BRIEF
//      Serial.print(F("ENHANCED_STANDALONE=true, quiet and skipping motors this run"));
//    #endif
//    // do nothing we don't want to start motors when fob mode and the quite play
//  } else {
//    digitalWrite(v12Switch, HIGH);
//    digitalWrite(v5Switch, HIGH);
//    #ifdef VARY_SPEED
//      analogWrite(VARY_PIN, vary_init);
//      delay(500);
//      vary_current_speed=vary_init;
//      vary_jumped = true;
//      if (vary_init < vary_min ) { 
//        vary_direction = 'i';
//      } else {
//        vary_direction = 'd';
//      }
//    #endif
//  }
//  is_dancing = true;
//  dance_type = dance_piece;
//  dance_started = millis();
//  last_checkin = dance_started;

}


//// are we currently dancing?
//void reply_status() {
//  #if defined IS_BRIEF
//    Serial.print(F("sending status: "));
//  #endif
//  if (is_dancing && !fob->fob_is_dancing) {
//    xbee->xbee_write(dancing);
//    #if defined IS_BRIEF
//      Serial.println(F("dancing"));
//    #endif
//  } else {
//    xbee_write(finished); 
//    #if defined IS_BRIEF
//      Serial.println(F("finished"));
//    #endif
//  }
//}

// notify the director from time to time we are still dancing
void Dancer::send_dancing() {
    #if defined IS_BRIEF
      Serial.println(F("Dancer::send_dancing() sending dancing status..."));
    #endif
    last_checkin_millis = pt->current_millis;
    director->send(dancing);
}

// notify the director we have just finished
void Dancer::send_finished() {
  #if defined IS_BRIEF
    Serial.println(F("Dancer::send_finished() sending finished status..."));
  #endif
  last_checkin_millis = pt->current_millis;
  director->send(finished);
}

//// are we done?
//void signal_if_done() {
//  // simulate the MP3 stopping after 13 seconds
//  #if defined IS_CHATTY 
//    Serial.print(F("signal_if_done track status: "));
//  #endif
//  if (track_is_complete()) {
//    #if defined IS_CHATTY
//      Serial.println(F("signal_if_done track status: complete"));
//    #endif
//    if (fob_is_dancing == false) {
//      #if defined IS_CHATTY
//        Serial.println(F("Fob is off, sending finished message to director"));
//      #endif
//      xbee_write(finished);
//    } else {
//      #if defined IS_CHATTY
//        Serial.println(F("Fob is on, skipping 'finished' message to director"));
//      #endif
//    }
//    stop_all();
//    if (fob_is_dancing) {
//      // simulate the sleep between tracks
//      #if defined IS_CHATTY
//        Serial.print(F("FobA is on, pausing for "));
//        Serial.print(solo_delay_seconds);
//        Serial.println(F(" between plays"));
//      #endif
//      delay(1000 * solo_delay_seconds);
//      // if enhanced_mode, switch volume for next play
//      if (ENHANCED_STANDALONE == true ) {
//        if (fob_next_volume == NORMAL_VOLUME) {
//          fob_next_volume = FOB_QUIET_VOLUME;
//        } else {
//          fob_next_volume = NORMAL_VOLUME;
//        }
//      }
//    }
//  }
//  #if defined IS_CHATTY 
//    else { Serial.println(F("track not done!")); }
//  #endif
//}


// are we done?
void Dancer::signal_every_so_often() {

  #if defined IS_CHATTY 
    Serial.println(F("starting signal_every_so_often")); 
  #endif

  if (!fob_is_dancing) {
    if (pt->current_millis - last_checkin_millis > checkin_millis) {
      #if defined IS_CHATTY
        Serial.println(F("signal_every_so_often checking in..."));
      #endif
      send_dancing();   
    }
  }
}

//
//  // fob is pushed override any incomming direction  
//  if (fob_is_dancing) {
//    drain_serial();        // don't let this build up too much...
//    current_direction = empty_request;  // default to no incomming command
//    // always be dancing...
//    if (is_dancing == false) {
//      #if defined IS_CHATTY
//        Serial.println(F("fobA is on and not dancing, restarting dance"));
//      #endif
//      current_direction = solo;  // simulate a new request to do a solo 
//    }
//  }
//      // for fob off
//        // do_beep(200, 50);
//      // do_beep(200, 30); 
//
//      // fob on 
//             // fob_next_volume = NORMAL_VOLUME;
//      // do_beep(200, 30);
//
//      // if fob was pushed 
//          delay(2000);  // this keeps us from getting bouncy pin signal 
//


//void Dancer::clear_pending_direction() {
//  // drain any outstanding input queries....
//  while (xbee_available() > 0) {
//    # if defined IS_CHATTY
//      char x = xbee_read();
//      Serial.print(F("Draining Serial got: "));
//      Serial.println(x);
//    #else
//      Serial.println(F("draining..."));
//      xbee_read();
//    #endif  
//  }
//} 

void Dancer::status(boolean do_print) {
  if (! do_print) { return; }

  Serial.print(F("Dancer::status() is_dancing="));
  Serial.print(bool_tostr(is_dancing));
  Serial.print(F(", is_pre_delayed="));
  Serial.print(bool_tostr(is_pre_delayed));
  Serial.print(F(", is_play_started="));
  Serial.print(bool_tostr(is_play_started));
  Serial.print(F(", is_play_complete="));
  Serial.print(bool_tostr(is_play_complete));
  Serial.print(F(", is_fob_delayed="));
  Serial.println(bool_tostr(is_fob_delayed));
  Serial.print(F("    current_direction="));
  Serial.print((char)current_direction);
  Serial.print(F(", has_new_direction="));
  Serial.print(bool_tostr(has_new_direction));
  Serial.print(F(", num_dances="));
  Serial.print(num_dances);
  Serial.print(F(", num_plays="));
  Serial.print(num_plays);
  Serial.print(F(" dance_type="));
  Serial.print((char)dance_type);
  Serial.print(F(" current_track_name="));
  Serial.println(current_track_name);
  Serial.print(F("    dance_started_millis="));;
  Serial.print(dance_started_millis);
  Serial.print(F(", dance_delay_duration="));
  Serial.print(pt->current_millis - dance_started_millis);
  Serial.print(F(", dance_started_millis="));
  Serial.print(dance_started_millis);
  Serial.print(F(", fob_delay_start_millis="));
  Serial.print(pt->current_millis - fob_delay_start_millis);
  Serial.print(F(", last_checkin_millis="));
  Serial.print(last_checkin_millis);
  Serial.print(F(", last_checkin_duration="));
  Serial.println(pt->current_millis - last_checkin_millis);
  
}
#endif
