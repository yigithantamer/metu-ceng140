#include "loop.h"
#include <stdio.h>
#include <limits.h>
int curr_char;
int loop_vars[52][5];


int is_at_limit(int i){
	if(loop_vars[i][3]>0){
		if(!(loop_vars[i][4]+loop_vars[i][3]<=loop_vars[i][2])&&(loop_vars[i][4]+loop_vars[i][3]>=loop_vars[i][1]))
			return 1;
		else
			return 0;
	}else if(loop_vars[i][3]<0){
		if(!(loop_vars[i][4]+loop_vars[i][3]>=loop_vars[i][2])&&(loop_vars[i][4]+loop_vars[i][3]<=loop_vars[i][1]))
			return -1;
		else
			return 0;
	}else{
		return 0;


	}

}
int loop_variable_value(char c){
	int i,k;
	for(i=0;i<52;i++){
		if(loop_vars[i][0]==((int)c)){
			k=loop_vars[i][4];
			break;
		}
	}
	return k;
}
void loop_continue(char c){
	int i,limit;

	for(i=0;i<52;i++){
		if(loop_vars[i][0]==((int)c)){
			limit=i;
			break;
		}	
	}

	curr_char=limit;
	for(i=limit+1;i<52;i++){
		if(loop_vars[i][0]==INT_MIN)
			break;
		loop_vars[i][4]=loop_vars[i][1];
	}
	return;

}




int main(){
	int i,j,limit;

	char loopvar;
	int b1,b2,step;

	for(i=0;i<52;i++)
		for(j=0;j<5;j++)
			loop_vars[i][j]=INT_MIN;
	
	for(i=0;i<52;i++){
		do{
			loopvar=getchar();
		}while(loopvar=='\n');
		if(loopvar==EOF)
			break;
		scanf(" %d %d %d", &b1,&b2,&step);
		loop_vars[i][0]=(int)loopvar;
		loop_vars[i][1]=b1;
		loop_vars[i][2]=b2;
		loop_vars[i][3]=step;
		loop_vars[i][4]=b1;
	}
	curr_char=0;


	for(i=0;i<52;i++){
		if(loop_vars[i][0]==INT_MIN){
			limit=i-1;
			break;
		}	
	}
	
	for(i=0;i<=limit;i++){
		if(loop_vars[i][3]>0){
			if(loop_vars[i][1]>loop_vars[i][2])
				return 0;
		
		}else if(loop_vars[i][3]<0){
			if(loop_vars[i][1]<loop_vars[i][2])
				return 0;
		
		
		
		
		}
	
	}
	
	curr_char=limit;
	loop_execute();
	while(1){
		loopvar=(char)loop_vars[curr_char][0];
		b1=loop_vars[curr_char][1];
		b2=loop_vars[curr_char][2];
		step=loop_vars[curr_char][3];

		if(is_at_limit(curr_char)){
			if(curr_char==0){
				break;
			}else{
				curr_char--;
			}
		}else{
			loop_vars[curr_char][4]+=step;
			for(i=curr_char+1;i<=limit;i++){
				loop_vars[i][4]=loop_vars[i][1];
			}
			curr_char=limit;
			loop_execute();
		}	


	}
	return 0;
}

