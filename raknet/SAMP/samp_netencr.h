/*
	Updated to 0.3z by P3ti
*/

#ifdef RAKSAMP_CLIENT
extern unsigned char encrBuffer[4092];
#else
extern unsigned char decrBuffer[4092];
#endif

#ifdef RAKSAMP_CLIENT
void kyretardizeDatagram(unsigned char *buf, int len, int port, int unk);
#else
void unKyretardizeDatagram(unsigned char *buf, int len, int port, int unk);
#endif
