#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct huffNode
{
 int parent,lchild,rchild;
 unsigned long count;
 unsigned char c;
 char bits[256];

}HuffNode;
HuffNode HTree[512],temp;
void compress()
{
 FILE *infile,*outfile;
infile=fopen("yasuo.txt","r+");//读取文件指针
outfile=fopen("yasuofile1.txt","w+");//压缩的写入指针
 int i,j,k;
 int used;                          //用到的字符数
 unsigned long total=0;              //文件长度

 //每个叶子节点赋值
 for(i=0;i<512;i++)
 {
  HTree[i].count=0;
  HTree[i].c=(unsigned char)i;
  HTree[i].lchild=-1;
  HTree[i].parent=-1;
  HTree[i].rchild=-1;
 }

 //统计各个字符出现次数
 unsigned char c;
 while(!feof(infile))
 {
  fread(&c,1,1,infile);
  HTree[c].count++;
  total++;
 }
 total--;
 HTree[c].count--;

 //count为0的不要，按count从大到小排列
 for(i=0;i<255;i++)
 {
  for(j=i+1;j<256;j++)
  {
   if(HTree[i].count<HTree[j].count)
   {
    temp=HTree[i];
    HTree[i]=HTree[j];
    HTree[j]=temp;
   }
  }
 }

 //有用字符数
 for(i=0;i<512;i++)
  if(HTree[i].count==0)
   break;
 used=i-1;
 
 //构建哈夫曼树
 unsigned long min;
 int m=2*i-1;
 int pt;
 for(i;i<m;i++)
 {
  min=999999;
  for(j=0;j<i;j++)
  {
   if(HTree[j].parent!=-1) 
    continue;
   if(min>HTree[j].count)
   {
    pt=j;
    min=HTree[j].count;
   }
  }
  HTree[i].count=min;
  HTree[pt].parent=i;
  HTree[i].lchild=pt;
  min=999999;
  for(j=0;j<i;j++)
  {
   if(HTree[j].parent!=-1) 
    continue;
   if(min>HTree[j].count)
   {
    pt=j;
    min=HTree[j].count;
   }
  }
  HTree[i].count+=min;
  HTree[pt].parent=i;
  HTree[i].rchild=pt;
 }

 //为每个有权值的字符编码
 for(i=0;i<=used;i++)
 {
  k=i;
  HTree[i].bits[0]=0;
  while(HTree[k].parent!=-1)
  {
   j=k;
   k=HTree[k].parent;
   if(HTree[k].lchild==j)
   {
    j=strlen(HTree[i].bits);
    memmove(HTree[i].bits+1,HTree[i].bits,j+1);
    HTree[i].bits[0]='0';
   }
   else
   {
    j=strlen(HTree[i].bits);
    memmove(HTree[i].bits+1,HTree[i].bits,j+1);
    HTree[i].bits[0]='1';
   }
  }
 }
 //写头文件
 char buf[512];
 fseek(infile,0,0);
 fwrite(&total,sizeof(unsigned long),1,outfile);          //原文件总长度
 fwrite(&used,sizeof(int),1,outfile);
 int maxCSize=0;             //最长的字符编码
 for(i=0;i<=used;i++)
 {
  if(maxCSize<strlen(HTree[i].bits))
   maxCSize=strlen(HTree[i].bits);
 }
 fwrite(&maxCSize,sizeof(int),1,outfile);
 for(i=0;i<=used;i++)
 {
  fwrite(&HTree[i].c,sizeof(unsigned char),1,outfile);
  fwrite(&HTree[i].bits,maxCSize,1,outfile);
 }

 //开始压缩主文件
 j=0;             //最大为total
 buf[0]=0;
 pt=12+5*used;
 while(!feof(infile))
 {
  c=fgetc(infile);
  j++;
  for(i=0;i<=used;i++)
  {
   if(HTree[i].c==c)
    break;
  }
  strcat(buf,HTree[i].bits);
  k=strlen(buf);
  c=0;
  while(k>=8)
  {
   for(i=0;i<8;i++)
   {
    if(buf[i]=='1') 
     c=(c<<1)|1;
    else
     c=c<<1;
   }
   fwrite(&c,1,1,outfile);
   pt++;
   strcpy(buf,buf+8);
   k=strlen(buf);
  }
  if(j==total)
   break;
 }
 if(k>0)            //可能还有剩余字符
 {
  strcat(buf,"00000000");
  for(i=0;i<8;i++)
  {
   if(buf[i]=='1') 
    c=(c<<1)|1;
   else
    c=c<<1;
  }
  fwrite(&c,1,1,outfile);
  pt++;
 }
 
 fclose(infile);
 fclose(outfile);
 printf("压缩成功！");
 float s;
 s=(float)pt/(float)total;
 printf("压缩率为:%f",&s);
 return;
}
void reserve(char ch[])//把bits[]数组反序排列
{
  int i;
  int tmp;
  for(i=0;i<strlen(ch)/2;i++)
  {
  	  tmp=ch[i];
  	  ch[i]=ch[strlen(ch)-i-1];
  	  ch[strlen(ch)-i-1]=tmp;
  }
}

void uncompress()
{
	 FILE *infile,*outfile;
outfile=fopen("yasuo1.txt","w+");//读取文件指针
infile=fopen("yasuofile1.txt","r+");//压缩的写入指针
  int i,j,k,l;
 int used;                          //用到的字符数
 unsigned long total=0;              //文件长度
 char buf[256],bx[256];
 unsigned char c; 
 int maxCSize;
 fread(&total,sizeof(unsigned long),1,infile); 
 fread(&used,sizeof(int),1,infile);
 fread(&maxCSize,sizeof(int),1,infile);
 for(i=0;i<=used;i++)
 {
  fread(&HTree[i].c,sizeof(unsigned char),1,infile);
  fread(&HTree[i].bits,maxCSize,1,infile);
 }

 //开始解压
 j=0;
 bx[0]=0;
 int a,b,f;
 while(1)
 {
 	 
  while(strlen(bx)<maxCSize) 
  {
     fread(&c,1,1,infile); 
      k=c; 
      f=0;
       a=k;
          b=a;
          while(a!=0)//把数字转化为字符数组
          {
            b=a%2;
            a=a/2;
            if(b==1)
            {
              buf[f]='1';
              f++;
            }
            else
            {
              buf[f]='0';
              f++;
            }
          }
         reserve(buf);
   k=strlen(buf); 
   for(l=8;l>k;l--) //在单字节内对相应位置补0
   {
    strcat(bx,"0"); 
   }
   strcat(bx,buf); 
    strcpy(buf,buf+strlen(buf));
  }
  for(i=0;i<=used;i++) 
  {
   if(memcmp(HTree[i].bits,bx,strlen(HTree[i].bits))==0) break; 
  }
  strcpy(bx,bx+strlen(HTree[i].bits)); 
  c=HTree[i].c; 
  fwrite(&c,1,1,outfile); 
  j++;   //统计解压缩后文件的长度
  if(j==total) break;  
 }
 fclose(infile);
 fclose(outfile);



}
int main(int argc, char const *argv[])
{
	compress();
	return 0;
}