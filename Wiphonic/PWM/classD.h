#ifndef CLASSD_H_
#define CLASSD_H_

void audioManager(
    /* streaming chanend cSOFGen, streaming chanend cAudioMixer, streaming chanend cAudioDFU,*/ streaming chanend PCM_IN,
    out port LEFT, out port RIGHT, out port TEST);

#endif /*CLASSD_H_*/
