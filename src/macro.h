#ifndef __MACRO_H__
#define __MACRO_H__

#define SUCCESS_CHK(f, msg) do { \
                                bool ret = (f); \
                                if(!ret){ \
                                    while(1){ Serial.println(msg); } \
                                } \
                            } while(0);


#endif
