#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[])
{
    unsigned char i,c;
    short int j;
    unsigned long offset,temp;
    char nome[255];
    FILE *fin, *fout, *foutcompressed;
    printf("MEGAMAN XTREME/2 GBC GRAPHICS/MAP DECOMPRESSOR\n");

    if(argc!=4)
    {
	printf("usage: mmx2dec input offset output\n");
	return 1;
    }

    fin = fopen(argv[1],"rb");
    if(!fin)
    {
        printf("Can't open %s\n",nome);        return 1;
    }
    sscanf(argv[2],"%x",&offset);
    printf("Offset (hex):%X\n",offset);
    fseek(fin,offset,0);
    strcat(nome,".out");
    fout = fopen(argv[3],"wb+");
    foutcompressed = fopen("compressed","wb+");
    if(!fout)
    {
        printf("Can't open %s\n",nome);
        return 1;
    }
        i = fgetc(fin);
	fputc(i,foutcompressed);
          for (;i != 0;) //until flag == 0
          {
              if((i&0x80)==0x80)
              {
                  i &= 0x7F;
                  j = fgetc(fin);
	fputc(j,foutcompressed);
                  j = ((j&0xFF)+0xFF00);
                  offset = ftell(fout);
                  temp = (offset+(j));
                  for (;i!=0;i--)
                  {
                      fseek(fout,temp,0);
                      c = fgetc(fout);
                      temp += 1;
                      fseek(fout,offset,0);
                      fputc(c,fout);
                      offset += 1;
                  }
              }
              else
              {
                  for (;i!=0;i--)
                  {
                      c = fgetc(fin);
	fputc(c,foutcompressed);
                      fputc(c,fout);
                  }
              }
              i = fgetc(fin);
	fputc(i,foutcompressed);
          }

    printf("Done!\n");

fclose(fin);
fclose(fout);
fclose(foutcompressed);
}
