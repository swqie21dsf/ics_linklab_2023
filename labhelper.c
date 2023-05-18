#include<stdio.h>
#include<string.h>
#include <sys/stat.h>
#include<stdlib.h>

#define MAXSIZE 50005


char phase_text[MAXSIZE];
char cookie[10] = "211502009";

void read_phase(char filename[])
{
    __uint8_t tmp;
    char tmp_txt[3];
    FILE *fp;
    fp = fopen(filename,"rb");

    //fread(phase_text,sizeof(unsigned),sizeof(phase_text),fp);
    while(!feof(fp))
    {
        fread(&tmp,sizeof(__uint8_t),1,fp);
        sprintf(tmp_txt,"%02x",tmp);
        strcat(phase_text,tmp_txt);
    }
    fclose(fp);

}

void write_phase(char filename[])
{
    FILE *ptr_pre = fopen(filename,"w");
    fclose(ptr_pre);
    
    FILE *ptr = fopen(filename,"ab");
    char* start = phase_text;
    char tmp[3];
    unsigned tmp_num;

    int max_num = strlen(phase_text) - 1;
    if(max_num % 2) max_num--;
    int cnt = 0;
    while(start != NULL && cnt < max_num)
    {
        strncpy(tmp,start,2);
        sscanf(tmp,"%02x",&tmp_num);
        fwrite(&tmp_num,sizeof(char),1,ptr);
        start += 2;
        cnt += 2;
    }
    fclose(ptr);


}

void exec(char *cmd, char *res)
{
    char buf[BUFSIZ];
    FILE *ptr;
    char ps[1005];
    strcpy(ps,cmd);
    if((ptr = popen(ps,"r")) != NULL)
    {
        while(fgets(buf,sizeof(buf),ptr) != NULL)
        {
            if(strlen(res)+strlen(buf)>MAXSIZE)
            {
                printf("out of range!\n");
                break;
            }
            strcat(res,buf);
        }
        pclose(ptr);
        ptr = NULL;
    }
}

void str2ascii(char *src, char *ret)
{
    unsigned tmp;
    char tmp_text[3];
    //printf("\nin str2ascii:");

    for(int i=0;i<strlen(src);i++)
    {
        tmp = src[i];
        sprintf(tmp_text,"%02x",tmp);
        strcat(ret,tmp_text);
    }

}
//str with huichefu, len = strlen - 1
void strE2ascii(char *src, char *ret)
{
    unsigned tmp;
    char tmp_text[3];
    //printf("\nin str2ascii:");

    for(int i=0;i<strlen(src)-1;i++)
    {
        tmp = src[i];
        sprintf(tmp_text,"%02x",tmp);
        strcat(ret,tmp_text);
    }

}





void phase1ans()
{
    memset(phase_text,0,sizeof(phase_text));
    read_phase("phase1.o");
    
    char text[50005];
    exec("gcc -m32 -no-pie -o link main.o phase1.o",text);
    exec("./link",text);

    char ascii_text[MAXSIZE];
    char cookie_ascii[MAXSIZE];
    strE2ascii(text,ascii_text);
    str2ascii(cookie,cookie_ascii);

    strcat(cookie_ascii,"00");
 
    char* startptr = strstr(phase_text,ascii_text);
    if(startptr != NULL)
    {
        int pos = startptr - phase_text;
        strncpy(startptr,cookie_ascii,strlen(cookie_ascii));

    }

    write_phase("./Ans/phase1.o");

    memset(text,0,sizeof(text));
    exec("gcc -m32 -no-pie -o link main.o ./Ans/phase1.o",text);
    exec("./link",text);
    printf("phase1 return: %s",text);
}

void phase2ans()
{
    memset(phase_text,0,sizeof(phase_text));
    read_phase("phase2.o");

    char JieTouBiao[MAXSIZE];
    exec("readelf -S phase2.o",JieTouBiao);
    char* text_start_ptr = strstr(JieTouBiao,".text");
    char Name[100],Type[100];
    unsigned Addr,Off;
    sscanf(text_start_ptr," %s %s %x %x",Name,Type,&Addr,&Off);
    //printf("Name: %s Type: %s Addr: %x offset: %x \n",Name,Type,Addr,Off);

    char FuHaoBiao[MAXSIZE];
    exec("readelf -s phase2.o",FuHaoBiao);
    char* startptr =  strstr(FuHaoBiao,"15:");
    int offset, call_offset;
    if(startptr != NULL)
    {
       // printf("\nmatch\n");
        int flag = sscanf(startptr + 3," %x",&offset);
        if(flag){
            //printf("offset:%x\n",offset);
            char dump[MAXSIZE];
            exec("objdump -d phase2.o",dump);
            char *now_call_ptr = dump;
            char* call_ptr;
            char *start;
            do{
                start = now_call_ptr + 4;
                call_ptr = now_call_ptr;
                now_call_ptr = strstr(start,"call");

            }while(now_call_ptr != NULL);

            char *ch = call_ptr;
            while((*ch)!='\n')
            {
                ch -= 1;
            }
            sscanf(ch,"%x",&call_offset);
            //printf("%x\n",call_offset);

            int byte_num = 0;
            char ans_text[MAXSIZE] = "5589e56a";
            byte_num += 4;
            char ans_tmp[100];
            str2ascii(&cookie[8],ans_tmp);
            strcat(ans_text,ans_tmp);
            memset(ans_tmp,0,sizeof(ans_tmp));
            strcat(ans_text,"68");
            for(int i=1;i<=4;i++)
            {
                char tmp = cookie[3+i];
                unsigned num;
                num = tmp;
                sprintf(ans_tmp+2*(i-1),"%02x",num);
            }
            strcat(ans_text,ans_tmp);           
            strcat(ans_text,"68");
            for(int i=0;i<4;i++)
            {
                char tmp = cookie[i];
                unsigned num;
                num = tmp;
                sprintf(ans_tmp+2*i,"%02x",num);
            }
            strcat(ans_text,ans_tmp);      
            strcat(ans_text,"89e050e9dcffffff");

            //printf("%s\n",ans_text);
            int pos = offset + Off;
            char *start_pos = strstr(phase_text,"7f454c46");
            strncpy(start_pos + 2 * pos,ans_text,strlen(ans_text));

            //printf("%s",phase_text);
            write_phase("./Ans/phase2.o");

            char text[MAXSIZE];
            memset(text,0,sizeof(text));
            exec("gcc -m32 -no-pie -o link main.o ./Ans/phase2.o",text);
            exec("./link",text);
            printf("phase2 return: %s",text);

        }
    }



}


void phase3ans()
{
    memset(phase_text,0,sizeof(phase_text));
    read_phase("phase3.o");

    char dump[MAXSIZE];
    memset(dump,0,sizeof(dump));
    exec("objdump -d phase3.o",dump);

    char *pos1 = strstr(dump,"movl");
    
    char op[20],dollar,p_cookie[100];
    union src{
        unsigned operand;
        char code[4];
    }read_src;

    read_src.operand = 0;
    sscanf(pos1,"%s %c%x",op,&dollar,&read_src.operand);
    //printf("op: %s dollar:%c operand: %x\n",op,dollar,read_src.operand);
    strncat(p_cookie,read_src.code,4);

    char *pos2 = strstr(pos1+5,"movl");
    read_src.operand = 0;
    sscanf(pos2,"%s %c%x",op,&dollar,&read_src.operand);
    //printf("op: %s dollar:%c operand: %x \n",op,dollar,read_src.operand);
    strncat(p_cookie,read_src.code,4);

    char *pos3 = strstr(pos2+5,"movw");
    read_src.operand = 0;
    sscanf(pos3,"%s %c%x",op,&dollar,&read_src.operand);
    //printf("op: %s dollar:%c operand: %x \n",op,dollar,read_src.operand);
    strcat(p_cookie,read_src.code);   

    //printf("p_cookie: %s",p_cookie);
 
    for(int i=0;i<strlen(p_cookie);i++)
    {
        p_cookie[i] += 0x40;        
    }

    char FuHaoBiao[MAXSIZE];
    exec("readelf -s phase3.o",FuHaoBiao);
    char *sym_pos = strstr(FuHaoBiao,"256 OBJECT");
    char s_type[20],s_bind[20],s_vis[20],s_ndx[20],s_name[50];
    int size;
    sscanf(sym_pos,"%d %s %s %s %s %s",&size,s_type,s_bind,s_vis,s_ndx,s_name);
    //printf("name: %s\n",s_name);


    FILE *ptr = fopen("phase3_patch.c","wb");
    char c_code[100] = "char ";
    strcat(c_code,s_name);
    strcat(c_code,"[256] = {0x31};");
    fwrite(c_code,strlen(c_code),1,ptr);
    fclose(ptr);

    char gcc_text[1000];
    exec("gcc -m32 -c phase3_patch.c",gcc_text);

    memset(phase_text,0,sizeof(phase_text));    
    read_phase("phase3_patch.o");
    char asc_cookie[100];
    str2ascii(cookie,asc_cookie);
    char *copy_ptr = asc_cookie;

    char* start_pos = strstr(phase_text,"7f454c46");

    for(int i = 0;i < 9; i++)
    {
        strncpy(start_pos+2 * (__uint8_t)p_cookie[i],copy_ptr,2);
        copy_ptr += 2;
    }

    write_phase("./Ans/phase3_patch.o");

    char e_text[MAXSIZE];
    exec("gcc -m32 -no-pie -o link main.o phase3.o ./Ans/phase3_patch.o",e_text);
    exec("./link",e_text);
    printf("phase3 return: %s",e_text);

}

void phase4ans()
{
    memset(phase_text,0,sizeof(phase_text));
    read_phase("phase4.o");

    char dump[MAXSIZE];
    exec("objdump -d phase4.o",dump);

    char* do_phase_ptr = strstr(dump,"<do_phase>:");

    char *pos1 = strstr(do_phase_ptr,"movl");
    
    char op[20],dollar,p_cookie[100];
    union src{
        unsigned operand;
        char code[4];
    }read_src;

    read_src.operand = 0;
    sscanf(pos1,"%s %c%x",op,&dollar,&read_src.operand);
    strncat(p_cookie,read_src.code,4);

    char *pos2 = strstr(pos1+5,"movl");
    read_src.operand = 0;
    sscanf(pos2,"%s %c%x",op,&dollar,&read_src.operand);
    strncat(p_cookie,read_src.code,4);

    char *pos3 = strstr(pos2+5,"movw");
    read_src.operand = 0;
    sscanf(pos3,"%s %c%x",op,&dollar,&read_src.operand);
    strcat(p_cookie,read_src.code);   

    unsigned Numbers[10];
    char standard[24];
    for(char i = '0'; i <= '9'; i++)
    {
        strcat(standard,"$0x3");
        strncat(standard,&i,1);
        strcat(standard,",-0x9(%ebp)");
        
        char* mov_cmd_ptr = strstr(do_phase_ptr,standard);
        if(mov_cmd_ptr != NULL)
        {
            char* now = mov_cmd_ptr;
            char ch = *now;
            while(ch!='\n')
            {
                now--;
                ch = *now;
            }
            sscanf(now+1," %x",&Numbers[i-'0']);

        }          
        memset(standard,0,sizeof(standard));
    }


    char* elf_start = strstr(phase_text,"7f454c46");
    char* start_pos = strstr(elf_start + 0x80,"34000000");
    start_pos += 8;

    char mtext[200];

    int cookie2num[9];
    for(int i = 0; i < 9; i++)
    {
        int tmp = cookie[i] - '0';
        cookie2num[i] = Numbers[tmp];
    }

    for(int j = 0; j < 9; j++)
    {
        unsigned index = (unsigned char)p_cookie[j] - 0x41;
        char* jmp_pos = start_pos + index * 8;
        char num[5]="00";
        union INT8_C{
            unsigned data;
            unsigned char code[4];
        }number_text;
        number_text.data = cookie2num[j];

        sprintf(num,"%02x%02x",number_text.code[0],number_text.code[1]);
        //printf("%s ",num);
        strncpy(jmp_pos,num,4);
    }

    strncpy(mtext,start_pos,sizeof(mtext));

    //printf("write over");
    fflush(stdout);
    write_phase("./Ans/phase4.o");


    char e_text[200];
    exec("gcc -m32 -no-pie -o link main.o ./Ans/phase4.o",e_text);
    memset(e_text,0,sizeof(e_text));
    exec("./link",e_text);
    printf("phase4 return: %s\n",e_text);

}

int cmp(const void *a, const void *b)
{
    int* p = (int*)a;
    int* q = (int*)b;
    return p-q;
}



void phase5ans()
{
    memset(phase_text,0,sizeof(phase_text));
    read_phase("phase5.o");

    char copy_text[MAXSIZE]={0};
    strncpy(copy_text,phase_text,strlen(phase_text));

   unsigned jmp_off[25];
   int cur_pos = 0;
   memset(jmp_off,0xfff,sizeof(jmp_off));

    //char dump[MAXSIZE];
    //exec("objdump -d phase5.o",dump);
    fflush(stdout);
    char JieTouBiao[MAXSIZE] = {0};
    exec("readelf -S phase5.o",JieTouBiao);
    struct JieInfo
    {
        char Name[12];
        char Type[12];
        unsigned Addr;
        unsigned Off;
    }jie_text,jie_reltext,jie_data,jie_reldata;
    char* jie_text_ptr = strstr(JieTouBiao,".text");
    sscanf(jie_text_ptr,"%s %s %x %x",jie_text.Name,jie_text.Type,&jie_text.Addr,&jie_text.Off);
    //printf("%s %s %x %x",jie_text.Name,jie_text.Type,jie_text.Addr,jie_text.Off);
    char* jie_reltext_ptr = strstr(JieTouBiao,".rel.text");
    sscanf(jie_reltext_ptr,"%s %s %x %x",jie_reltext.Name,jie_reltext.Type,&jie_reltext.Addr,&jie_reltext.Off);
    char* jie_reldata_ptr = strstr(JieTouBiao,".rel.data");
    sscanf(jie_reldata_ptr,"%s %s %x %x",jie_reldata.Name,jie_reldata.Type,&jie_reldata.Addr,&jie_reldata.Off);


    char* elf_start = strstr(copy_text,"7f454c46");
    char* real_elf_start = strstr(phase_text,"7f454c46");
    char* text_start = elf_start + jie_text.Off * 2;
    char* r_mov = strstr(text_start,"8b048500000000");
    while(r_mov != NULL)
    {
        unsigned tmp = (r_mov - text_start) / 2;
        //printf("mov offset: %x\n",tmp+3);
        jmp_off[cur_pos++] = tmp + 3;
        r_mov = strstr(r_mov + 14,"8b048500000000");
    }
 
    char* r_call = strstr(text_start,"e8fcffffff");
    while(r_call != NULL)
    {
        unsigned tmp = (r_call- text_start) / 2;
        //printf("call offset: %x\n",tmp+1);
        jmp_off[cur_pos++] = tmp + 1;
        r_call = strstr(r_call + 14,"e8fcffffff");
    }

    r_call = strstr(text_start,"0fb68000000000");
    while(r_call != NULL)
    {
        unsigned tmp = (r_call- text_start) / 2;
        //printf("movzbl offset: %x\n",tmp+3);
        jmp_off[cur_pos++] = tmp + 3;
        strncpy(r_call,"31313131",8);
        r_call = strstr(r_call + 14,"0fb68000000000");
    }

    r_call = strstr(text_start,"8b1500000000");
    while(r_call != NULL)
    {
        unsigned tmp = (r_call- text_start) / 2;
        //printf("mov 0 edx offset: %x\n",tmp+2);
        jmp_off[cur_pos++] = tmp + 2;
        r_call = strstr(r_call + 14,"8b1500000000");
    }

    r_call = strstr(text_start,"83f8077774");
    unsigned tmp = (r_call- text_start) / 2;
    //printf("mov 5c offset: %x\n",tmp+8);
    jmp_off[cur_pos++] = tmp + 8;

    r_call = strstr(text_start,"6800000000");
    while(r_call != NULL)
    {
        unsigned tmp = (r_call- text_start) / 2;
        //printf("push offset: %x\n",tmp+1);
        jmp_off[cur_pos++] = tmp + 1;
        r_call = strstr(r_call + 10,"6800000000");
    }

    r_call = strstr(text_start,"a100000000");
    while(r_call != NULL)
    {
        unsigned tmp = (r_call- text_start) / 2;
        //printf("mov eax offset: %x\n",tmp+1);
        jmp_off[cur_pos++] = tmp + 1;
        r_call = strstr(r_call + 10,"a100000000");
    }

    r_call = strstr(text_start,"a300000000");
    while(r_call != NULL)
    {
        unsigned tmp = (r_call- text_start) / 2;
        //printf("mov eax to 0 offset: %x\n",tmp+1);
        jmp_off[cur_pos++] = tmp + 1;
        r_call = strstr(r_call + 10,"a300000000");
    }

    r_call = strstr(text_start,"a104000000");
    while(r_call != NULL)
    {
        unsigned tmp = (r_call- text_start) / 2;
        //printf("mov 4 to eax offset: %x\n",tmp+1);
        jmp_off[cur_pos++] = tmp + 1;
        r_call = strstr(r_call + 10,"a104000000");
    }

    //printf("total : %d\n",cur_pos);

    //qsort(jmp_off,25,sizeof(unsigned),cmp);

    unsigned sorted_off[23];
    for(int i=0;i < 23;i++)
        sorted_off[i] = jmp_off[i];
    unsigned tmpa[23];
    unsigned min = 0xffe;
    int index = -1;
    for(int i=0;i < 23;i++)
    {

        for(int j=0;j < 23;j++)
        {
            if(sorted_off[j] < min)
            {
                min = sorted_off[j];
                index = j;
            } 
        }
        tmpa[i] = min;
        sorted_off[index]=0xfff;
        min = 0xffe;
    }
    for(int i=0;i < 23;i++)
        jmp_off[i] = tmpa[i];

    char info_text[][9]={
        {"010d0000"},
        {"01050000"},
        {"010d0000"},
        {"010d0000"},
        {"010d0000"},
        {"010d0000"},
        {"010d0000"},
        {"010d0000"},
        {"010e0000"},
        {"010e0000"},
        {"020f0000"},
        {"010e0000"},
        {"02130000"},
        {"01110000"},
        {"010e0000"},
        {"02130000"},
        {"01110000"},
        {"010e0000"},
        {"02100000"},
        {"01150000"},
        {"010c0000"},
        {"010c0000"},
        {"02170000"}
    };

    union INT8_C{
            unsigned data;
            unsigned char code[4];
        }number_text;

    char offset_text_ans[MAXSIZE]={0};

    for(int i=0;i<23;i++)
    {
        number_text.data = jmp_off[i];
        char tmp_c[5] = "0000";
        fflush(stdout);
        sprintf(tmp_c,"%02x%02x",number_text.code[0],number_text.code[1]);
        strncat(offset_text_ans,tmp_c,4);
        strcat(offset_text_ans,"0000");
        strncat(offset_text_ans,info_text[i],8);
    }
    //printf("%s\n",offset_text_ans);


    char* data_start = elf_start + jie_reldata.Off * 2;
    unsigned data_off[3];

    char encode_1[3] = {0};
    strncpy(encode_1,data_start+16,2);
    sscanf(encode_1,"%2x",&data_off[0]);

    char encode_2[3] = {0};
    strncpy(encode_2,data_start+32,2);
    sscanf(encode_2,"%2x",&data_off[1]);
 
    char do_phase[3] = {0};
    strncpy(do_phase,data_start+48,2);
    sscanf(do_phase,"%2x",&data_off[2]);

    
    if(data_off[0]!=0)
    {
        data_off[1]=data_off[0]+4;
        data_off[2]=data_off[1]+4;
    }else if(data_off[1]!=0)
    {
        data_off[0]=data_off[1]-4;
        data_off[2]=data_off[1]+4;
    }else if(data_off[2]!=0)
    {
        data_off[1]=data_off[2]-4;
        data_off[0]=data_off[1]-4;
    }

    char data_info_text[][9]={
        {"01120000"},
        {"01140000"},
        {"01160000"}
    };

    char offset_data_ans[MAXSIZE]={0};

    for(int i=0;i<3;i++)
    {
        number_text.data = data_off[i];
        char tmp_c[5] = "0000";
        fflush(stdout);
        sprintf(tmp_c,"%02x%02x",number_text.code[0],number_text.code[1]);
        strncat(offset_data_ans,tmp_c,4);
        strcat(offset_data_ans,"0000");
        strncat(offset_data_ans,data_info_text[i],8);
    }
    //printf("%s\n",offset_data_ans);

    strncpy(real_elf_start+jie_reltext.Off*2 ,offset_text_ans,strlen(offset_text_ans));
    strncpy(real_elf_start+jie_reldata.Off*2+16,offset_data_ans,strlen(offset_data_ans));

    //printf("%s\n",phase_text);
    write_phase("./Ans/phase5.o");
    char e_text[200] = {0};
    exec("gcc -m32 -no-pie -o link main.o ./Ans/phase5.o",e_text);
    memset(e_text,0,sizeof(e_text));
    exec("./link",e_text);
    printf("phase5 return: %s\n",e_text);

}



int main()
{


    printf("studentID: ");
    scanf("%s",cookie);
    
    mkdir("./Ans",S_IRWXU);

    phase1ans();
    phase2ans();
    phase3ans();
    phase4ans();
    phase5ans();

    char duwee[1000]={0};
    char pack[1001]={"tar cvf ./Ans/"};
    strncat(pack,cookie,strlen(cookie));
    strcat(pack,".tar ./Ans/*.o");
    //printf("%s\n",pack);
    exec(pack,duwee);
    printf("%s\n",duwee);
    

    return 0;
}

