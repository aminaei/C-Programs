/*
 * CS-570 Program 1 -- getwords.c --
 * Instrctor: John Carroll
 * Due Date: Sep 18, 2017
 *
 *
 * Author Name: Ali Minaei
 * Created on: Sep 7, 2017
 *
 *
 * getword.c
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "getword.h"




int getword(char *w){
	int i=0;
	char ch;
	char endw;
	char ch2;
	int flag = 0;
	int flagidx = 0;
	char buff[STORAGE];
	int k =0;
	int j =0;


	w[0] = '\0';

	while((ch = getchar())!=EOF){  // IF it is not EOF --loop


		// -- case incoming char is a leading tab
		if(ch == '\t'){
			// printf("!!! Tab !!!\n");
			if(ch == '\t' && w[0] !='\0' && i>0 ){
				// printf("--> i = %d, w[i-1] = %c\n",i, w[0]);
				w[i] = '\0';
				return i;
			}
			else{
				flag = 0;
				w[0] = '\0';
				i=0;
			}
		}

		// -- case ch == '\'
		// printf("--> i = %d, flag = %d, ch = %c, endw = %c\n",i,flag,ch,endw);
		if(ch == '\\' && flag==0){
			ch2 = getchar();
			 //printf("--> i = %d, ch = %c, ch2 = %c\n",i,ch,ch2);

			// -- Case: '\' is not the 1st character
			// -- if it is before \n, retun the w array
			if(i!=0 && ch == '\\' ){
				if(ch2 == '\n'){
					ungetc('\n',stdin);
					w[i] = '\0';
					// w[++i] = '\0';
					return i;
				}

				else{
					w[i] = ch2;
					w[++i] = '\0';
				}
			}
            else if((ch2 == ' ') || (ch2 == '\t')){
                //printf("->-> i = %d, ch = %c, ch2 = %c\n",i,ch,ch2);
                w[i] = ch2;
                w[++i] = '\0';
                return i;
            }
            else if((ch2 == '&')){
                //printf("->-> i = %d, ch = %c, ch2 = %c\n",i,ch,ch2);
                w[i] = ch2;
                w[++i] = '\0';
                return i;
            }
            else if((ch2 == '\\')){
                //printf("->-> i = %d, ch = %c, ch2 = %c\n",i,ch,ch2);
                w[i] = ch2;
                w[++i] = '\0';
                return i;
            }
			else{
				ungetc(ch2,stdin);
				w[i] = ch;
				w[++i] = '\0';
				return i;
			}

		}

		// -- Case: if ch = '>' or '!' or '>!'
		else if(flag == 0 && (ch == '>' || ch == '!')){
			// -- 1. ch == '>' and ch2 == '!'
			if(ch == '>'){
				ch2 = getchar();
				if(ch2 == '!'){
					if(i!=0){
						ungetc(ch2,stdin);
						ungetc(ch,stdin);
						return i;
					}
					else{
						w[i] = ch;
						w[++i] = ch2;
						w[++i] = '\0';
						return i;
					}

				}
				else{
					ungetc(ch2,stdin);
					if(i!=0){
						ungetc(ch,stdin);
						return i;
					}
					else{
						w[i] = ch;
						w[++i] = '\0';
						return i;
					}

				}
			}

			// -- 2. w[i] == '>' and ch == '!'
			else if(ch == '!' && w[i-1] == '>'){
				if(i!=0){
					ungetc(ch,stdin);
					return i;
				}
				else{
					w[i] = ch;
					w[++i] = '\0';
					return i;
				}
			}

			// -- 3. ch = '>' or ch == '!'
			else{
				if(i!=0){
					ungetc(ch,stdin);
					return i;
				}
				else{
					w[i] = ch;
					w[++i] = '\0';
					return i;
				}
			}

		}
		// -- Case: ch = '<' or '\' or '&'
		else if(flag == 0 && (ch == '<' || ch == '|' )){

			if(i!=0){
				ungetc(ch,stdin);
				return i;
			}
			else{
				w[i] = ch;
				w[++i] = '\0';
				return i;
			}


		}
        else if(flag == 0 && (ch == '&')){

            if(i>0){
                ungetc(ch,stdin);
                return i;
            }
            else{
                w[i] = ch;
                w[++i] = '\0';
                return i;
            }


        }
		// -- Case: ch = ';'
		else if(ch == ';'){
			if(i>0){
				ungetc(';',stdin);
				return i;
			}
			else{
				flag = 0;
				w[0] = '\0';
				i=0;
				return i;
			}

		}
		// -- case ch = "'"
		else if(ch == '\'' || flag != 0){

			// -- 1. save characters after "'" to the buff array
			if(ch != '\\'){

				buff[k] = ch;
				buff[++k] = '\0';

			}

			// -- 2. If the previous char is not "\", raise the flag
			if(ch == '\'' && w[i-1] != '\\'){
				if(flag == 1)
                    flag = 2;
                else {
                    flag = 1;
                    flagidx = i;
                }
			}
            // printf("--> flag = %d, i = %d, flagidx = %d ch = %c \n",flag,i,flagidx,ch );

			// -- 3. There is no 2nd "'" until newline/EOF comes along
			ch2 = getchar();
            //printf("--> flag = %d, i = %d, flagidx = %d ch = %c ch2 = %c\n",flag,i,flagidx,ch,ch2 );
			if(flag == 1 && (ch2 == '\n')){
				//if(flagidx > 0){
					//printf("--> flag = %d, i = %d, flagidx = %d ch = %c \n",flag,i,flagidx,ch );
					ungetc(ch2,stdin);
					//for(j=k-1; j>=0;--j){
					//	ungetc(buff[j],stdin);
						// printf("buff[%d] = %c ",j,buff[j]);
					//}
					// printf("\n");
					//w[flagidx]='\0';
                    w[++i]='\0';

					return -30;
				//}
				// else{
				// 	ungetc(ch2,stdin);
				// 	w[i] = ch;
				//	w[++i]='\0';
				//	return i;
				// }
			}
			ungetc(ch2,stdin);

			// -- 4. ch = "'" and the previous ch = "\"
			if(flag == 1 && w[i-1] == '\\'){
				if(ch == '\''){
					w[i-1] = ch;
					w[i] = '\0';
				}
				else{
					w[i] = ch;
					w[++i] = '\0';

				}

			}
			// -- 5. Detected a 2nd ch = "'"
			else if(ch == '\'' && flag == 2){
				ch2 = getchar();
				if(!(ch2 =='\\'||ch2=='<'||ch2=='>'||ch2== '|'||ch2==';'||ch2=='&'||ch2=='\t'||ch2=='!'||ch2=='\n'||ch2==' ')){
					flag = 0;
					ungetc(ch2,stdin);
					// w[i] = ch;
					w[i] = '\0';
				}
				else{
					ungetc(ch2,stdin);
					flag = 0;
					return i;
				}
			}
			// -- 6. Detected newline of EOF
			else if(flag == 1 && (ch == '\n' || ch == -1)){
				if( ch == '\n')
					ungetc('\n',stdin);

				if(i > flagidx){
					for(j=i; j>=flagidx;--j ){
						ungetc(w[j],stdin);
						--i;
					}
					ungetc('\'',stdin);
					flag = 0;
					w[i]='\0';
				}

				return i;
			}
			// -- 7. Add ch to the w array
			else{
				if(ch != '\''){
					w[i] = ch;
					w[++i] = '\0';
				}

			}

		} // EOF Handeling "'"

		else{
			// -- Removing leading Tabs or spaces
			if(flag == 0 && i == 0 && (ch == ' ' || ch == '\t'))
				w[0] = '\0';

			else if(ch == ' ') // Makig a change here to separate words after ' '
				return i;
// 			else if(ch != ' '){   
			else {   
				w[i] = ch;
				w[++i] = '\0';
			}
			// if(i-1 == 0 && w[i-1]==' ')
			// 	w[--i] = '\0';


		}

		// 3. case incoming char is newline
		if(ch == '\n'){
			if(w[0] == '\n'){
				w[0] = '\0';
				i =0 ;
				return i;
			}
			else{
				ungetc('\n',stdin);
				w[--i]='\0';
				return i;
			}
		}
		// 4. keep last char
		if((endw = getchar()) > 0)
			ungetc(endw,stdin);
		else{
			// printf("!!! The END!!!\n");
			return i;
		}

		if (i == STORAGE-1){
			return i;
		}

	}

	return -1;  // EOF return -1

}


