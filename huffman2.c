#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//利用haffman 编码实现字符文件压缩和解压
typedef struct Hfnode   //huffman树节点  
{
	unsigned char name;//名称
    unsigned long weight;//权(字符出现的次数)
	char bits[256];
	int parent;
	int rson;
     int lson; 
	
}Hfnode;
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

void compress()//压缩函数
{    int i,j,k;
	int n=0;//记录最小权的开始位置
	int m=0;
	int flag;//编码的下标
	int temp;
	char BUF[512];
	unsigned long length=0;//文件长度
	int Minrecord;
	Hfnode Hf[512];
	Hfnode  tmp; 
	unsigned char p;
    FILE *rfp,*wfp;//定义文件流指针,用于打开读取的文件或者打开写操作的文件
    rfp=fopen("yasuo.txt","r+");//读取文件指针
    wfp=fopen("yasuofile.txt","w+");//压缩的写入指针
    for(i=0;i<512;i++)//初始化Hf数组
    {

    	  Hf[i].name=0;
    	  Hf[i].weight=0;
    	  Hf[i].parent=Hf[i].lson=Hf[i].rson=-1;
    }
    while(!feof(rfp))//feof是检测文件是否读取完的函数,读取完了返回1;未读取完则返回0;
    {
       fread(&p,1,1,rfp);//size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);(man fread中介绍) 
       //*ptr 用于接收数据的内存地址 size 是要读取的每个单位的字节数  nmemb 要读取的数据项的个数  stream是输入流指针
       Hf[p].weight++; 
       length++;
    }
    length--;
    Hf[p].weight--;//因为读到文件结尾会读到'\0'所以要去掉
	for(i=0;i<256;i++)//开始创建huffman函数
	{
		if(Hf[i].weight!=0)
		{
			Hf[i].name=(unsigned char)i;//要把整型的变成字符型
        }
	}
	for(i=0;i<256;i++)//排序
	{
	    for(j=i+1;j<256;j++)
	   {
	   	  if(Hf[i].weight<Hf[j].weight)//从大到小排序比从小到大排序优!
	   	  {
              tmp=Hf[i];
              Hf[i]=Hf[j];
              Hf[j]=tmp;
	   	  }
	    } 
	}
 for(i=0;i<256;i++)//找出weight=0的数
 {
 	if(Hf[i].weight==0)
 	{
 	   n=i;
 	   m=2*n-1;
 	   break;
 	} 
 }
  for(i=n;i<m;i++)
  {
     for(j=0;j<i;j++)
     {    
     	 Minrecord=999999;//作对比的一个类似于无穷大的数
     	 if(Hf[j].parent!=-1)//表示已经是做叶子了所以直接跳过;
         {
         	continue;
         }
         if(Minrecord>Hf[j].weight)
         {
         	temp=j;
         	Minrecord=Hf[j].weight;

         }
     }
         Hf[i].weight=Minrecord;
         Hf[i].lson=temp;
         Hf[temp].parent=i;
         Minrecord=999999;//用作对比的一个类似于无穷大的数
     for(j=0;j<i;j++)
     {
         if(Hf[j].parent!=-1)
         {
         	continue;
         }
         if(Minrecord>Hf[j].weight)
         {
         	temp=j;
         	Minrecord=Hf[j].weight;
         }
     }
     
         Hf[i].weight=Hf[i].weight+Minrecord;
         Hf[i].rson=temp;
         Hf[temp].parent=i;   
  }
   for(i=0;i<n;i++)//huffman树编码
  {   flag=i;
     j=0;
     while(Hf[flag].parent!=-1)
     {
          temp=flag;//中间变量
          flag=Hf[flag].parent;
       if(Hf[flag].lson==temp)
       {
        
           Hf[i].bits[j]='0';//左孩子为'0'
           j++;
       }
       else
       {
           Hf[i].bits[j]='1';//右孩子为'1'
          j++;
       }
     }
  }
   for(i=0;i<n;i++)
   {
   	  reserve(Hf[i].bits);
   }
   fseek(rfp,0,0);//把文件指针指向开头
   fwrite(&length,sizeof(unsigned long),1,wfp);//在输出文件里先写入压缩文件的长度
   fwrite(&n,sizeof(int),1,wfp);//写入一共用了多少种字符
   int maxlength=0;//找出字符中最长的编码长度方便写入
   for(i=0;i<n;i++)
   {
      if(maxlength<strlen(Hf[i].bits))
      	maxlength=strlen(Hf[i].bits);
   }
   fwrite(&maxlength,sizeof(int),1,wfp);//把最长的长度写入文件
   for(i=0;i<n;i++)
   {
      fwrite(&Hf[i].name,sizeof(unsigned char),1,wfp);//写入字符名称
      fwrite(&Hf[i].bits,maxlength,1,wfp);//写入对应的编码
   }
   j=0;//计算文本字符数
   BUF[0]=0;//初始化数组
   int total=0;//计算压缩文件中的字符数
   total=12+5*(n-1);//4+4+4+(4+1)*n
   unsigned char c;
   while(!feof(rfp))
   {  
   	  c=fgetc(rfp);
   	  j++;
   	  for(i=0;i<n;i++)
   	  {
   	  	 if(c==Hf[i].name)
   	  	 {
   	  	 	
   	  	 	break;
   	  	 }
   	  }
   	  strcat(BUF,Hf[i].bits);
     k=strlen(BUF);
     c=0;
    while(k>=8)//用while  不能用if  如果用if 只会一次 比如19个的话就只能到11就不能继续
     {
       for(i=0;i<8;i++)
       {
          if(BUF[i]=='0')
          {
          	c=c<<1;
          }
          else
          {
          	c=c<<1|1;
          }
          fwrite(&c,1,1,wfp);
          total++;
          strcpy(BUF,BUF+8);//把后面8个往前面排
          k=strlen(BUF);
       }
     }
     if(j==length)
     {
     	break;
     }
   }
   if(k>0)
   {
   	  strcat(BUF,"00000000");
   	     for(i=0;i<8;i++)
       {
          if(BUF[i]=='0')
          {
          	c=c<<1;
          }
          else
          {
          	c=c<<1|1;
          }
          fwrite(&c,1,1,wfp);
          total++;
       }

   }
   fclose(rfp);
   fclose(wfp);
   printf("压缩完毕!");
   float s=0;
   s=(float)total/(float)length;
   printf("压缩比率为:%f\n",&s);

}

void uncompress()
{ 
     Hfnode *Hf[512];




}
void menu()
{   printf("______________________________________________________\n");
	printf("|----------------------------------------------------|\n");
	printf("|                  菜单                              |\n");
	printf("|----------------------------------------------------|\n");
	printf("|-------------1.输入a开始压缩------------------------|\n");
	printf("|-------------2.输入b开始解压------------------------|\n");
	printf("|-------------3.输入q退出操作------------------------|\n");
	printf("|____________________________________________________|\n");
}

int main(int argc, char const *argv[])
 {  //  char c;
// 	 while(1)
//    {
// 	  menu();
// 	 scanf("%c",&c);
// 	 if(c=='a')
// 	 {
// 	 	compress();
// 	 }
// 	else if(c=='b')
// 	 {
// 	 	//uncompress();
// 	 }
// 	 else if(c=='q')
// 	 {
//         break;
// 	 }
// 	 else
// 	 {
// 	 	printf("输入错误!");
// 	 }

//    }
	compress();
	return 0;
}
