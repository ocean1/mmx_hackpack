#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define rec_min 3 //min rec
#define rec_max 0x7F //max rec
#define jmp_max 0x100 //jmp max
#define flag_max 0x7F //max size of flag telling how many bytes gets copied
#define end_flag 0x00 //end of compression stream

struct match {
	int posizione; //posizione del match
	int count; //numero di bytes del match
};

void encode(FILE *fin,FILE *fout);
struct match findbestmatch(int poslet, unsigned char* buflet, int filelen);

int main(int argc, char *argv[])
{
   FILE *fin, *fout;
   printf("MEGAMAN XTREME/2 GBC GRAPHICS/MAP COMPRESSOR\n");

   if(argc!=3)
   {
	printf("usage: mmx2cmp input output\n");
	return 1;
   }
   fin = fopen(argv[1],"rb");
   if(!fin)
   {
        printf("Can't open %s\n",argv[1]);
        return 1;
   }
   fout = fopen(argv[2],"wb");
   if(!fout)
   {
        printf("Can't open %s\n",argv[2]);
        return 1;
   }

   encode(fin,fout);

   fclose(fin);
   fclose(fout);
	
   printf("Done!\n");
   return 1;
}

void encode(FILE *fin,FILE *fout)
{
   int filelen; //size of file to compress
   unsigned char *buflet; //pointer to read bufer
   unsigned char flag; //flag, # of compressed bytes
   unsigned char cbuffer[rec_max]; //buffer containing compressed bytes
   int poslet; //read index

   fseek(fin,0,SEEK_END);
   filelen = ftell(fin);
   fseek(fin,0,SEEK_SET);
   buflet = (unsigned char *)malloc(filelen);
   fread(buflet,1,filelen,fin);

   flag = 1; //initialize flag
   poslet = 1; //move read index after 1st byte
   cbuffer[0] = buflet[0]; //init first byte
	
   do
   {
    	struct match bestmatch = findbestmatch(poslet,buflet,filelen);
    	if(bestmatch.count > 0)
    	{
    		//flag!=0 > write buffer of compressed bytes
    		if(flag!=0)
    		{
    			fputc(flag,fout); //write out flag
    			fwrite(cbuffer,1,flag,fout);
    			flag = 0; //zero flag
    		}
            fputc( ((bestmatch.count&0x7F)|0x80) ,fout); //write flag for compressed bytes
    		fputc( ((bestmatch.posizione-poslet)&0xFF) ,fout); //write jump
    		poslet += bestmatch.count; //update pos
    	}
    	else
    	{
    		cbuffer[flag]=buflet[poslet];
    		flag++;
    		poslet++;
		if(flag==flag_max) //never go over max pos
    		{
    			fputc(flag,fout); //write flag
    			fwrite(cbuffer,1,flag,fout);
    			flag = 0; //zero flag
    		}	
    	}
   } while (poslet<=filelen); //until we get to the end of the buffer

   fputc(end_flag,fout);

   free(buflet);
}

struct match findbestmatch(int poslet, unsigned char* buflet, int filelen)
{
   int match_lenght = 0; //# matching bytes
   struct match bestmatch; // best match struct
   bestmatch.count = 0; //init counter

   int slideindex = (poslet - jmp_max); //init sliding window
   if (slideindex < 0) slideindex = 0;

   for(int i=0;i<jmp_max;i++)
   {
             if(buflet[slideindex] == buflet[poslet])
             {
	             match_lenght = 0;
	             while( (buflet[slideindex+match_lenght] == buflet[poslet+match_lenght])&&(match_lenght<rec_max)&&(slideindex<poslet) )
	             ++match_lenght;
             }
             if((match_lenght>bestmatch.count)&&(match_lenght>=rec_min))
             {
		  if( (poslet+match_lenght)>filelen ) match_lenght = filelen - poslet;
                  bestmatch.posizione = slideindex;
                  bestmatch.count = match_lenght;
             }
             ++slideindex;
   }
   return bestmatch;
}
