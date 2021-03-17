#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* and_or_to_comma(char* input_string);
int sizeof_string(char* input_string);
char* concatenate_substrings(char** input_strings, int substring_count);
char** split_string(char* input_string);
int count_substrings(char* input_string);
int is_axiom(char* input_string);
char* remove_implications(char* input_string);
char* eliminate_implication_connective(char* input_string);
char* drop_negation(char* input_string);
int is_disproven(char* initial_string);
char apply_rules(char* initial_sequent);
char** split_proposition(char* input_string, char type);
char** or_on_the_left(char* input_string);
char** and_on_the_right(char*input_string);
int main(){
	char input[210], *initial_sequent;
	int i=0;


	/*gets input from user, creates the initial string and calls apply_rules with the initial string */


	/*reads input from stdin*/
	fgets(input,210,stdin);

	/*creates string initial_sequent from input with the correct size */
	while(input[i]!='\0' && input[i]!='\n')i++;
	initial_sequent= (char*) calloc(i+1,sizeof(char));
	strncpy(initial_sequent,input,i);
	initial_sequent[i]='\0';

	printf("%c",apply_rules(initial_sequent));

	return 0;

}


char apply_rules(char* initial_sequent){

	char **sequent_list, program_output, *current_sequent, *return_sequent, **new_sequent_list,**return_sequent_list;
	int sequent_count,i,j;

	/* holds a list of sequents. Applies all 7 rules until on the elements of this list until one of them is disproven or all are proven */

	/* initializes sequent_list                    */
	sequent_list=(char**) calloc(1,sizeof(char*));
	*sequent_list=initial_sequent;
	sequent_count=1;

	/* applies the rules */
	program_output='?';
	while(program_output=='?'){
		for(i=0;i<sequent_count;i++){

			current_sequent=sequent_list[i];

			/*RULE 1*/
			return_sequent=drop_negation(current_sequent);
			if(strcmp(current_sequent,return_sequent)!=0){
				free(sequent_list[i]);
				sequent_list[i]=return_sequent;
				break;
			}else{
				free(return_sequent);
			}

			/*RULE 2*/
			return_sequent=and_or_to_comma(current_sequent);
			if(strcmp(current_sequent,return_sequent)!=0){
				free(sequent_list[i]);
				sequent_list[i]=return_sequent;
				break;
			}else{
				free(return_sequent);
			}


			/*RULE 3*/
			return_sequent_list=or_on_the_left(current_sequent);
			if(strcmp(current_sequent,*return_sequent_list)!=0){
				new_sequent_list=(char**) calloc(sequent_count+1,sizeof(char*));
				for(j=0;j<i;j++)
					new_sequent_list[j]=sequent_list[j];
				for(j=i+1;j<sequent_count;j++)
					new_sequent_list[j-1]=sequent_list[j];
				new_sequent_list[sequent_count-1]=return_sequent_list[0];
				new_sequent_list[sequent_count]=return_sequent_list[1];
				free(sequent_list[i]);
				free(sequent_list);
				free(return_sequent_list);
				sequent_list=new_sequent_list;
				sequent_count++;
				break;
			}else{
				free(return_sequent_list);
			}


			/*RULE 4*/
			return_sequent_list=and_on_the_right(current_sequent);
			if(strcmp(current_sequent,*return_sequent_list)!=0){
				new_sequent_list=(char**) calloc(sequent_count+1,sizeof(char*));
				for(j=0;j<i;j++)
					new_sequent_list[j]=sequent_list[j];
				for(j=i+1;j<sequent_count;j++)
					new_sequent_list[j-1]=sequent_list[j];
				new_sequent_list[sequent_count-1]=return_sequent_list[0];
				new_sequent_list[sequent_count]=return_sequent_list[1];
				free(sequent_list[i]);
				free(sequent_list);
				free(return_sequent_list);
				sequent_list=new_sequent_list;
				sequent_count++;
				break;
			}else{
				free(return_sequent_list);
			}


			/*RULE 5*/
			return_sequent=remove_implications(current_sequent);
			if(strcmp(current_sequent,return_sequent)!=0){
				free(sequent_list[i]);
				sequent_list[i]=return_sequent;
				break;
			}else{
				free(return_sequent);
			}


			/*RULE 6*/
			if(is_axiom(current_sequent)){

				new_sequent_list=(char**) calloc(sequent_count-1,sizeof(char*));
				for(j=0;j<i;j++)
					new_sequent_list[j]=sequent_list[j];
				for(j=i+1;j<sequent_count;j++)
					new_sequent_list[j-1]=sequent_list[j];
				free(sequent_list);
				sequent_list=new_sequent_list;
				sequent_count--;
				break;


			}

			/*RULE 7*/
			if(is_disproven(current_sequent)){
				program_output='F';
				break;

			}

		}
		if(sequent_count==0){
			program_output='T';
		}


	}

	return program_output;
}

int able_to_split(char* input_string, char type){

	char *cursor;
	int can_be_split, parantheses_count;

	/* Determines whether or not a proposition is of type (p?q), for any character ?*/

	can_be_split=0;
	parantheses_count=0;
	for(cursor=input_string;*cursor!='\0';cursor++){
		if(*cursor=='('){
			parantheses_count++;
		}else if(*cursor==')'){
			parantheses_count--;
		}else if(*cursor==type){
			if(parantheses_count==1){
				can_be_split=1;
				break;
			}
		}

	}

	return can_be_split;

}

char** or_on_the_left(char* input_string){

	char **output_strings, **sequent,*current_string,***new_sequents, **returned_sequent;
	int substring_count,has_run,sequent_arrow_index,i,j;

	/*calculates the number of substrings and splits the string into these substrings*/
	substring_count=count_substrings(input_string);
	sequent=split_string(input_string);


	/* splits the propositions of the form (p|q) on the left-hand side of the sequent arrow into two different sequents.
									Rellocates memory for char** sequent when needed. */
	/* finds which substring is the sequent arrow # */
	for(i=0;i<substring_count;i++){
		if(strcmp(sequent[i],"#")==0){
			sequent_arrow_index=i;
			break;
		}
	}

	has_run=0;
	for(i=0;i<sequent_arrow_index;i++){
		current_string=sequent[i];
		if(able_to_split(current_string,'|')){
			has_run=1;
			new_sequents=(char***) calloc(2,sizeof(char**));
			new_sequents[0]=(char**) calloc(substring_count,sizeof(char*));
			new_sequents[1]=(char**) calloc(substring_count,sizeof(char*));
			for(j=0;j<i;j++){
				new_sequents[0][j]=new_sequents[1][j]=sequent[j];
			}
			for(j=i+1;j<substring_count;j++){
				new_sequents[0][j]=new_sequents[1][j]=sequent[j];

			}
			returned_sequent=split_proposition(current_string,'|');
			new_sequents[0][i]=returned_sequent[0];
			new_sequents[1][i]=returned_sequent[1];
			break;
		}
	}
	if(!has_run){
		output_strings=(char**) calloc(1,sizeof(char*));
		*output_strings=concatenate_substrings(sequent,substring_count);
	}else{

		output_strings=(char**) calloc(2,sizeof(char*));
		*output_strings=concatenate_substrings(new_sequents[0],substring_count);
		*(output_strings+1)=concatenate_substrings(new_sequents[1],substring_count);
		free(new_sequents[0]);
		free(new_sequents[1]);
		free(new_sequents);
		free(returned_sequent);

	}

	for(i=0;i<substring_count;i++){
		free(sequent[i]);
	}
	free(sequent);

	return output_strings;

}

char** and_on_the_right(char*input_string){



	char **output_strings, **sequent,*current_string,***new_sequents, **returned_sequent;
	int substring_count,has_run,sequent_arrow_index,i,j;

	/*calculates the number of substrings and splits the string into these substrings*/
	substring_count=count_substrings(input_string);
	sequent=split_string(input_string);


	/* splits the propositions of the form (p|q) on the left-hand side of the sequent arrow into two different sequents.
									Rellocates memory for char** sequent when needed. */
	/* finds which substring is the sequent arrow # */
	for(i=0;i<substring_count;i++){
		if(strcmp(sequent[i],"#")==0){
			sequent_arrow_index=i;
			break;
		}
	}

	has_run=0;
	for(i=sequent_arrow_index+1;i<substring_count;i++){
		current_string=sequent[i];
		if(able_to_split(current_string,'&')){
			has_run=1;
			new_sequents=(char***) calloc(2,sizeof(char**));
			new_sequents[0]=(char**) calloc(substring_count,sizeof(char*));
			new_sequents[1]=(char**) calloc(substring_count,sizeof(char*));
			for(j=0;j<i;j++){
				new_sequents[0][j]=new_sequents[1][j]=sequent[j];
			}
			for(j=i+1;j<substring_count;j++){
				new_sequents[0][j]=new_sequents[1][j]=sequent[j];

			}
			returned_sequent=split_proposition(current_string,'&');
			new_sequents[0][i]=returned_sequent[0];
			new_sequents[1][i]=returned_sequent[1];
			break;
		}
	}
	if(!has_run){
		output_strings=(char**) calloc(1,sizeof(char*));
		*output_strings=concatenate_substrings(sequent,substring_count);
	}else{

		output_strings=(char**) calloc(2,sizeof(char*));
		*output_strings=concatenate_substrings(new_sequents[0],substring_count);
		*(output_strings+1)=concatenate_substrings(new_sequents[1],substring_count);
		free(new_sequents[0]);
		free(new_sequents[1]);
		free(new_sequents);
		free(returned_sequent);

	}

	for(i=0;i<substring_count;i++){
		free(sequent[i]);
	}
	free(sequent);

	return output_strings;



}


char** split_proposition(char* input_string, char type){

	/* creates p,q from a proposition (p?q) for any given ? */

	char **output_strings, *divide_pointer, *null_pointer, *cursor;
	int parantheses_count;


	/*finds the character at which the string will be divided*/
	parantheses_count=0;
	for(cursor=input_string;*cursor!='\0';cursor++){
		if(*cursor=='('){
			parantheses_count++;
		}else if(*cursor==')'){
			parantheses_count--;
		}else if(*cursor==type){
			if(parantheses_count==1){
				divide_pointer=cursor;
				break;
			}
		}

	}

	/* finds the null pointer */
	for(cursor=input_string;*cursor!='\0';cursor++);
	null_pointer=cursor;

	/* allocates memory for the two post-split propositions */
	output_strings=calloc(2,sizeof(char*));
	*output_strings=(char*) calloc(divide_pointer-input_string,sizeof(char));
	*(output_strings+1)=(char*) calloc(null_pointer-divide_pointer-1,sizeof(char));

	/* copies the two propositions into their allocated memory areas */
	strncpy(*output_strings,input_string+1,divide_pointer-input_string-1);
	strncpy(*(output_strings+1),divide_pointer+1,null_pointer-divide_pointer-2);
	(*output_strings)[divide_pointer-input_string-1]=(*(output_strings+1))[null_pointer-divide_pointer-2]='\0';

	return output_strings;

}

char* and_or_to_comma(char* input_string){

	char **sequent, *output_string, *current_string, **new_sequent, **returned_sequent;
	int substring_count, sequent_arrow_index, i,j;
	/*determines if it is possible to apply Rule 2 and if so, applies it using split_proposition*/


	/*calculates the number of substrings and splits the string into these substrings*/
	substring_count=count_substrings(input_string);
	sequent=split_string(input_string);


	/* splits the propositions of the form (p&q) on the left-hand side of the sequent arrow and
	(p|q) on the right-hand side of the sequent arrow. Rellocates memory for char** sequent when needed. */

	/* finds which substring is the sequent arrow # */
	for(i=0;i<substring_count;i++){
		if(strcmp(sequent[i],"#")==0){
			sequent_arrow_index=i;
			break;
		}
	}

	for(i=0;i<substring_count;i++){
		current_string=sequent[i];
		if(i<sequent_arrow_index && able_to_split(current_string,'&')){
			new_sequent=(char**) calloc(substring_count+1,sizeof(char*));
			for(j=0;j<i;j++){
				new_sequent[j+2]=sequent[j];
			}
			for(j=i+1;j<substring_count;j++){
				new_sequent[j+1]=sequent[j];
			}
			returned_sequent=split_proposition(sequent[i],'&');
			new_sequent[0]=returned_sequent[0];
			new_sequent[1]=returned_sequent[1];

			substring_count++;
			free(returned_sequent);
			free(sequent[i]);
			free(sequent);
			sequent=new_sequent;
			break;

		}else if(i>sequent_arrow_index && able_to_split(current_string,'|')){
			new_sequent=(char**) calloc(substring_count+1,sizeof(char*));
			for(j=0;j<i;j++){
				new_sequent[j]=sequent[j];
			}
			for(j=i+1;j<substring_count;j++){
				new_sequent[j-1]=sequent[j];
			}
			returned_sequent=split_proposition(sequent[i],'|');

			new_sequent[substring_count]=returned_sequent[0];
			new_sequent[substring_count-1]=returned_sequent[1];
			substring_count++;
			free(returned_sequent);
			free(sequent[i]);
			free(sequent);
			sequent=new_sequent;
			break;
		}
	}

	output_string=concatenate_substrings(sequent,substring_count);

	/* frees the substring list*/
	for(i=0;i<substring_count;i++){
		free(sequent[i]);
	}
	free(sequent);

	return output_string;

}


int sizeof_string(char* input_string){
	char* cursor;

	/*returns the number of characters in a given string, including the null ('\0') character. */
	for(cursor=input_string;*cursor!='\0';cursor++);
	return cursor-input_string+1;

}

char* concatenate_substrings(char**input_strings, int substring_count){



	/* concatenates substrings of a sequent into an appropriate sequent string
        (Using ',' between substrings unless one of the given substring is a sequent arrow "#")*/

	char *output_string,*cursor;
	int sequent_arrow_index,i,total_length;

	/* finds which substring is the sequent arrow */
	for(i=0;i<substring_count;i++){
		if(*input_strings[i]=='#'){
			sequent_arrow_index=i;
			break;
		}
	}

	/*calculates the total length of the string to be created*/
	for(i=0;i<substring_count;i++){
		if(i!=sequent_arrow_index)
			total_length+=sizeof_string(input_strings[i]);
	}

	/* allocates memory for the output string */
	output_string=(char* ) calloc(total_length, sizeof(char));

	/* copies the substrings into the output string */
	cursor=output_string;
	for(i=0;i<substring_count;i++){
		strncpy(cursor,input_strings[i],sizeof_string(input_strings[i])-1);
		cursor=cursor+sizeof_string(input_strings[i])-1;

		if(i==substring_count-1){
			*cursor='\0';
		}else if(i==sequent_arrow_index-1 || i==sequent_arrow_index){
			/* no commas or \0 needed, continuing */
		}else{

			*cursor=',';
			cursor++;
		}


	}

	return output_string;
}
char** split_string(char* input_string){
	int count,string_count;
	char *cursor;
	char** output_strings;

	/* splits a sequent string into several substrings made up of propositions and one sequent arrow (#) */

	/*counts the number of substrings the string will be split into	and allocates memory for string pointers */
	count=count_substrings(input_string);
	output_strings=(char** )calloc(count,sizeof(char*));


	/* allocates the memory for each substring and copies each substring to their
	 respective locations in output_strings */
	cursor=input_string;
	count=string_count=0;
	while(1){
		if(*cursor=='\0'){

			output_strings[string_count]=(char*) calloc(count+1,sizeof(char));
			strncpy(output_strings[string_count],cursor-count,count);
			output_strings[string_count][count]='\0';
			break;

		}else if(*cursor==','){

			output_strings[string_count]=(char*) calloc(count+1,sizeof(char));
			strncpy(output_strings[string_count],cursor-count,count);
			output_strings[string_count][count]='\0';
			string_count++;
			cursor++;
			count=0;
		}else if(*cursor=='#'){

			output_strings[string_count]=(char*) calloc(count+1,sizeof(char));
			strncpy(output_strings[string_count],cursor-count,count);
			output_strings[string_count][count]='\0';

			string_count++;

			output_strings[string_count]=(char*) calloc(2,sizeof(char));
			strcpy(output_strings[string_count],"#");
			string_count++;
			cursor++;
			count=0;
		}else{
			cursor++;
			count++;
		}


	}


	return output_strings;

}

int count_substrings(char* input_string){
	char* cursor;
	int count=0;

	/* calculates and returns the number of substrings the string will be split into,
	which is the number of propositions plus one for the sequent arrow string "#" */

	for(cursor=input_string; *cursor !='\0';cursor++){
		if(*cursor==','||*cursor=='#')
			count++;
	}

	return count+2;
}

int is_axiom(char* input_string){
	int sequent_arrow_index, i, j, is_axiom,substring_count;
	char **sequent;

	/*determines whether or not a proposition is present in both sides of the sequent arrow (#)*/

	/*calculates the number of substrings and splits the string into these substrings*/
	substring_count=count_substrings(input_string);
	sequent=split_string(input_string);

	/*finds which substring is the sequent arrow ("#")*/
	for(i=0;i<substring_count;i++){
		if(!strcmp("#",sequent[i])){
			sequent_arrow_index=i;
			break;
		}
	}

	/*determines whether or not the given sequent is an axiom */
	is_axiom=0;
	for(i=0;i<sequent_arrow_index;i++){
		for(j=sequent_arrow_index+1;j<substring_count;j++){

			if(!strcmp(sequent[i],sequent[j]))
				is_axiom=1;
		}
	}

	/*frees sequent*/
	for(i=0;i<substring_count;i++){
		free(sequent[i]);
	}
	free(sequent);

	return is_axiom;
}

char* remove_implications(char* input_string){

	char** sequent, *output_string;
	int substring_count,i;

	/* splits the sequent string into proposition substrings, finds propositions of type (p>q) and retplaces them
	with eliminate_implication_connective's return value (Removes implications) */

	/* splits the string into substrings, calculates the number of substrings*/
	substring_count=count_substrings(input_string);
	sequent=split_string(input_string);

	/* determines whether or not each substring contains an implication connector that can be removed
							if so, removes the connector by applying Rule 5*/
	for(i=0;i<substring_count;i++){

		if(able_to_split(sequent[i],'>')){
			sequent[i]=eliminate_implication_connective(sequent[i]);

		}
	}

	/* concatenates the list of substrings into a string */
	output_string=concatenate_substrings(sequent,substring_count);

	/* frees the list of substrings */
	for(i=0;i<substring_count;i++)
		free(sequent[i]);
	free(sequent);

	return output_string;
}

char* eliminate_implication_connective(char* input_string){
	/* Turns a formula of the type (p>q) into (-p|q)
	   and (-(P)>(Q)) into (--(P)|(Q))                */
	int count_parantheses,length;
	char *cursor, *arrow_pointer, *output_string, *null_pointer;

	/* Finds the correct '>' character in the given string*/
	cursor = input_string;
	count_parantheses=0;
	while(*cursor!='\0'){
		if(*cursor=='('){
			count_parantheses++;
		}else if(*cursor==')'){
			count_parantheses--;
		}else if(*cursor=='>'){
			if(count_parantheses==1){
				arrow_pointer=cursor;
				break;
			}
		}
		cursor++;
	}

	/* finds the length of the input string		  */
	length=sizeof_string(input_string);
	null_pointer=input_string+length-1;

	/* allocates memory for the string to be returned */
	output_string=(char*) calloc(length+1,sizeof(char));


	/* creates the output string                      */

	/* copies '(' into the 0th character              */
	*output_string='(';

	/* copies '-' into the 1st character               */
	*(output_string+1)='-';

	/* copies ')' into the (length)th character        */
	*(output_string+length-1)=')';

	/* copies '\0' into the (length+1)th character     */
	*(output_string+length)='\0';

	/* copies '|' into the (arrow_pointer-input_string+1)th character */
	*(output_string+(arrow_pointer-input_string)+1)='|';

	/* copies the rest of the input string into the output string */
	strncpy(output_string+2,input_string+1,(arrow_pointer-input_string)-1);
	strncpy(output_string+(arrow_pointer-input_string)+2,arrow_pointer+1,null_pointer-arrow_pointer-2);


	return output_string;
}

char* drop_negation(char* input_string){
	int substring_count, negation_required,i,j,length,sequent_arrow_index;
	char **sequent, *new_substring, *output_string;

	/* splits the sequent string into proposition substrings, finds propositions of the type -P and
	moves them to the other side of the sequent arrow (#), thus removing the negation sign - */

	/*calculates the number of substrings and splits the string into these substrings*/
	substring_count=count_substrings(input_string);
	sequent=split_string(input_string);

	/* finds and removes all propositions with a negation before them */
	negation_required=1;
	while(negation_required){
		negation_required=0;
		for(i=0;i<substring_count;i++){
			if(*sequent[i]=='#'){
				sequent_arrow_index=i;
				break;
			}
		}
		for(i=0;i<substring_count;i++){
			if(sequent[i][0]=='-'){
				negation_required=1;
				length=sizeof_string(sequent[i]);
				new_substring=(char *) calloc(length-1,sizeof(char));
				strcpy(new_substring,sequent[i]+1);
				free(sequent[i]);
				if(i>sequent_arrow_index){
					for(j=i;j>0;j--){
						sequent[j]=sequent[j-1];
					}
					sequent[0]=new_substring;

				}else{
					for(j=i;j<substring_count-1;j++){
						sequent[j]=sequent[j+1];
					}
					sequent[substring_count-1]=new_substring;
				}
				break;
			}
		}
	}

	/* concatenates the final substrings together */
	output_string=concatenate_substrings(sequent,substring_count);

	/* frees the substring list*/
	for(i=0;i<substring_count;i++){
		free(sequent[i]);
	}
	free(sequent);
	return output_string;
}

int is_disproven(char* initial_string){

	char* cursor;
	int no_connectives;
	/*Checks if a given sequent string can be disproven*/


	/*checks if there is at least one connective character in the string*/
	no_connectives = 1;
	cursor=initial_string;
	while(*cursor!='\0'){
		if(*cursor=='>' || *cursor=='|' || *cursor=='&' || *cursor=='-'){
			no_connectives=0;
			break;
		}
		cursor++;
	}

	if(no_connectives==1 && !is_axiom(initial_string))
		return 1;
	else
		return 0;


}




