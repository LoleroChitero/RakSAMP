class SAMP 
{ 
public: 
     typedef int (__cdecl *GEN)(char *out, char *generator, unsigned int factor); //00000378 - key 000003E9 -  serial 

     SAMP(char *dll) 
     { 
         this->hLib = LoadLibrary(dll); 
         if (this->hLib) 
         { 
             *(DWORD*)((DWORD)this->hLib+0x212A38) = (DWORD)this; 
             *(BYTE*)((DWORD)this->hLib+0x1078D4) = 1; 
         } 
     } 
     ~SAMP() 
     { 
         if (this->hLib == NULL) 
             return; 
         FreeLibrary(this->hLib); 
         this->hLib = NULL; 
     } 
     void getSerial(char *out) 
     { 
         if (this->hLib == NULL) 
             return; 

         char random_string[32] = {0}; 
         gen_random(random_string, (rand()%31)+1);

         if (*(DWORD*)((DWORD)hLib+0xBABE) == 0xBC8D0000) 
             ((GEN)(LPVOID)((DWORD)this->hLib+0x2BE46B))(out, random_string, 0x3E9+1); //always add 1 here 
         else 
           ((GEN)(LPVOID)((DWORD)this->hLib+0x291B86))(out, random_string, 0x3E9+1); //always add 1 here 
    
		 //by ????????
	 } 
     void getAuthKey(char *out, char *incomingKey) 
     { 
         if (this->hLib == NULL) 
             return; 
          
         if (*(DWORD*)((DWORD)hLib+0xBABE) == 0xBC8D0000) 
             ((GEN)(LPVOID)((DWORD)this->hLib+0x2B9F3F))(out, incomingKey, 0x378+1); //always add 1 here 
         else 
             ((GEN)(LPVOID)((DWORD)this->hLib+0x27714B))(out, incomingKey, 0x378+1); //always add 1 here 

		 //by ????????
     } 
     HMODULE GetHMODULE() 
     { 
         return this->hLib; 
     } 
private: 
     unsigned char SAMP_INFO[0x1000]; 
     HMODULE hLib; 
};