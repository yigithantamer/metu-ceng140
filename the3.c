#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <float.h>

/*the information about the stochastic variables (their names(A-Z), lower limit, upper limit,probabilities)*/
/*will be held in an array of this */
typedef struct stochastic{

	char letter;
	double lower_limit, upper_limit, *probabilities;

} sto;

/*For the stacks in the shunting_yard algorithm*/
typedef struct element{

	struct element *next, *prev;
	char *str_value;
	double double_value;

} emnt;

/*For the sorted list*/
typedef struct list_element{

	struct list_element *next;
	double value;
	long int count;

} lemnt;

void add_list_element(lemnt *list_begin, double value);
double *perform_experiments(char* formula, sto* stochastic_list, int interval_count, long int experiment_count);
double *create_probability_distribution(lemnt *experiment_list, int interval_count, long int experiment_count, double min_value, double max_value);
double get_random_value(sto *stochastic, int interval_count);
int draw_interval(double *interval_list, int interval_count);
double random_d(void);
char **tokenize_formula(char *formula);
char *pop_str_from_stack(emnt *stack_start);
double pop_lf_from_stack(emnt *stack_start);
void push_str_into_stack(emnt *stack_start, char *value_of_new_element);
void push_lf_into_stack(emnt *stack_start, double value_of_new_element);
void perform_operation_plus(emnt *operand_stack_start);
void perform_operation_minus(emnt *operand_stack_start);
void perform_operation_mult(emnt *operand_stack_start);
void perform_operation_div(emnt *operand_stack_start);
void perform_operation_pow(emnt *operand_stack_start);
void perform_operation_neg(emnt *operand_stack_start);
void perform_operation_sin(emnt *operand_stack_start);
void perform_operation_cos(emnt *operand_stack_start);
void perform_operation_sqrt(emnt *operand_stack_start);
void perform_operation_ln(emnt *operand_stack_start);
void perform_operation(emnt *operand_stack_start, char *operation);
emnt *shunting_yard(char *formula);
double evaluate(emnt *postfix_formula, double *variable_value_list);
int precedence(char character);
int count_letters(char *input_string);
int count_letters_unique(char *input_string);
int count_numbers(char *input_string);
int count_operators(char *input_string);
char *remove_spaces(char *input_string);
int count_characters(char *input_string);
int is_letter(char character);
int is_number(char character);
double value_of(char *operand);

int main(){
	char input_formula[210], *spaceless_formula;
	int interval_count, number_of_letters, dummy_index_1, dummy_index_2;
	double *output_list;
	long int experiment_count;
	sto *stochastic_list;
	/*sets the seed to the current time*/
	srand(time(NULL));
	/*takes the formula and removes all spaces*/
	fgets(input_formula,210,stdin);
	spaceless_formula=remove_spaces(input_formula);
	/*takes the rest of the input. Calculates number_of_letters for the for loop*/
	scanf("%d %ld", &interval_count, &experiment_count);
	number_of_letters=count_letters_unique(spaceless_formula);
	stochastic_list=(sto*) calloc(number_of_letters,sizeof(sto));
	for(dummy_index_1=0;dummy_index_1<number_of_letters;dummy_index_1++){
		scanf(" %c %lf %lf",&stochastic_list[dummy_index_1].letter,&stochastic_list[dummy_index_1].lower_limit, &stochastic_list[dummy_index_1].upper_limit);
		stochastic_list[dummy_index_1].probabilities=(double*) calloc(interval_count,sizeof(double));
		for(dummy_index_2=0;dummy_index_2<interval_count;dummy_index_2++){
			scanf(" %lf",&stochastic_list[dummy_index_1].probabilities[dummy_index_2]);
		}
	}
	/*calls the function that will compute the result*/
	output_list=perform_experiments(spaceless_formula, stochastic_list, interval_count, experiment_count);
	for(dummy_index_1=0; dummy_index_1<interval_count+2; dummy_index_1++){
		printf("%.3f ",output_list[dummy_index_1]);
	}
	printf("\n");
	/*Frees stuff*/
	free(output_list);
	for(dummy_index_1=0;dummy_index_1<number_of_letters;dummy_index_1++){
		free(stochastic_list[dummy_index_1].probabilities);
	}
	free(stochastic_list);
	free(spaceless_formula);
	return 0;

}

void add_list_element(lemnt *list_begin, double input_value){

	lemnt *new_list_element;
	new_list_element=(lemnt*) calloc(1,sizeof(lemnt));
	new_list_element->next=list_begin->next;
	list_begin->next=new_list_element;
	new_list_element->value=input_value;
	new_list_element->count=1;
	return;

}

double *perform_experiments(char* formula, sto* stochastic_list, int interval_count, long int experiment_count){

	/*perfroms the so-called "experiments", returns the resultant probability distribution*/
	int letter_count, dummy_index_1;
	char *temp;
	long int experiment_index;
	emnt *postfix_stack;
	double variable_value_list[26], result, *probability_distribution, max_value, min_value;
	lemnt *list_begin, *cursor, *cursor_del;
	list_begin=(lemnt*) calloc(1,sizeof(lemnt));
	list_begin->next=(lemnt*) calloc(1,sizeof(lemnt));
	list_begin->count=list_begin->next->count=-1;
	letter_count=count_letters_unique(formula);
	for(dummy_index_1=0;dummy_index_1<26;dummy_index_1++){
		variable_value_list[dummy_index_1]=0;
	}
	postfix_stack=shunting_yard(formula);
	max_value=-DBL_MAX;
	min_value=DBL_MAX;
	for(experiment_index=0;experiment_index<experiment_count;experiment_index++){
		for(dummy_index_1=0;dummy_index_1<letter_count;dummy_index_1++){
			variable_value_list[stochastic_list[dummy_index_1].letter-'A']=get_random_value(stochastic_list+dummy_index_1, interval_count);
		}
		result=evaluate(postfix_stack,variable_value_list);
		if(result<min_value){
			min_value=result;
		}
		if(result>max_value){
			max_value=result;
		}
		add_list_element(list_begin, result);
	}
	probability_distribution = create_probability_distribution(list_begin,interval_count,experiment_count, min_value, max_value);
	cursor=cursor_del=list_begin->next;
	while(cursor->count != -1){
		cursor=cursor->next;
		free(cursor_del);
		cursor_del=cursor;
	}
	free(cursor);
	free(list_begin);
	while(strcmp(postfix_stack->next->str_value,"end")){
		temp=pop_str_from_stack(postfix_stack);
		free(temp);
	}
	free(postfix_stack->next->str_value);
	free(postfix_stack->next);
	free(postfix_stack->str_value);
	free(postfix_stack);
	return probability_distribution;
}

double *create_probability_distribution(lemnt *experiment_list, int interval_count, long int experiment_count, double min_value, double max_value){

	long int index, *value_count;
	double *probability_distribution;
	lemnt *cursor;
	probability_distribution=(double*) calloc(interval_count+2,sizeof(double));
	probability_distribution[0]=min_value;
	probability_distribution[1]=max_value;
	value_count=(long int*) calloc(interval_count,sizeof(long int));
	for(index=0;index<interval_count;index++){
		value_count[index]=0;
	}
	for(cursor=experiment_list->next;cursor->count!=-1;cursor=cursor->next){
		index=(int)((cursor->value-min_value)/((max_value-min_value)/interval_count));
		if(index==interval_count){
			value_count[index-1]++;
		}else{
			value_count[index]++;
		}
	}
	for(index=0;index<interval_count;index++){
		probability_distribution[index+2]=(double)value_count[index]/(double)experiment_count;
	}
	free(value_count);
	return probability_distribution;

}

double get_random_value(sto *stochastic, int interval_count){

	/*calls draw_interval for the given stochastic variable*/
	int interval_index;
	interval_index=draw_interval(stochastic->probabilities, interval_count);
	return stochastic->lower_limit+((double)interval_index+random_d())*((stochastic->upper_limit-stochastic->lower_limit)/((double)interval_count));

}

int draw_interval(double *interval_list, int interval_count){

	/*"draws" an interval based on the probability distribution of the intervals and returns the index of the drawn interval*/
	int return_index;
	double random_double,sum;
	sum=0;
	random_double=random_d();
	for(return_index=0;return_index<interval_count;return_index++){
		sum=sum+interval_list[return_index];
		if(random_double<=sum)
			break;
	}
	return return_index;

}

double random_d(void){

	/*returns a random double between 0 and 1*/
	return (double)rand()/RAND_MAX;

}

char **tokenize_formula(char *formula){

	/*Tokenizes the given formula. Creates an end token to indicate the end of the formula.*/
	int token_count, token_index, is_currently_number;
	char **tokenized_formula, *cursor, *number_start_pointer;
	token_count=count_letters(formula)+count_operators(formula)+count_numbers(formula)+1;
	tokenized_formula=(char**) calloc(token_count,sizeof(char*));
	cursor=formula;
	token_index=0;
	is_currently_number=0;
	while(1){
		if(is_letter(*cursor)){
			tokenized_formula[token_index]=(char*) calloc(2,sizeof(char));
			*tokenized_formula[token_index]=*cursor;
			*(tokenized_formula[token_index]+1)='\0';
			token_index=token_index+1;
			cursor=cursor+1;
		}else if(is_number(*cursor)){
			if(is_currently_number){
				if(is_number(*(cursor+1))){
					cursor=cursor+1;
				}else{
					tokenized_formula[token_index]=(char*) calloc((cursor-number_start_pointer)+2,sizeof(char));
					strncpy(tokenized_formula[token_index],number_start_pointer,(cursor-number_start_pointer)+1);
					tokenized_formula[token_index][(cursor-number_start_pointer)+1]='\0';
					token_index=token_index+1;
					cursor=cursor+1;
					is_currently_number=0;
				}
			}else{
				if(is_number(*(cursor+1))){
					is_currently_number=1;
					number_start_pointer=cursor;
					cursor=cursor+1;
				}else{
					tokenized_formula[token_index]=(char*) calloc(2,sizeof(char));
					tokenized_formula[token_index][0]=*cursor;
					tokenized_formula[token_index][1]='\0';
					token_index=token_index+1;
					cursor=cursor+1;
				}
			}
		}else{
			if(*cursor=='\0'){
				tokenized_formula[token_index]=(char*) calloc(4,sizeof(char));
				strcpy(tokenized_formula[token_index],"end");
				break;
			}else if(*cursor=='l'){
				tokenized_formula[token_index]=(char*) calloc(3,sizeof(char));
				strcpy(tokenized_formula[token_index],"ln");
				token_index=token_index+1;
				cursor=cursor+2;
			}else if(*cursor=='c'){
				tokenized_formula[token_index]=(char*) calloc(4,sizeof(char));
				strcpy(tokenized_formula[token_index],"cos");
				token_index=token_index+1;
				cursor=cursor+3;
			}else if(*cursor=='s'){
				if(*(cursor+1)=='q'){
					tokenized_formula[token_index]=(char*) calloc(5,sizeof(char));
					strcpy(tokenized_formula[token_index],"sqrt");
					token_index=token_index+1;
					cursor=cursor+4;
				}else{
					tokenized_formula[token_index]=(char*) calloc(4,sizeof(char));
					strcpy(tokenized_formula[token_index],"sin");
					token_index=token_index+1;
					cursor=cursor+3;
				}
			}else{
				tokenized_formula[token_index]=(char*) calloc(2,sizeof(char));
				*tokenized_formula[token_index]=*cursor;
				*(tokenized_formula[token_index]+1)='\0';
				token_index=token_index+1;
				cursor=cursor+1;
			}
		}
	}
	return tokenized_formula;

}

char *pop_str_from_stack(emnt *stack_start){

	/*pops count-many elements from the stack*/
	char *stack_value;
	emnt *del;
	del=stack_start->next;
	stack_value=del->str_value;
	stack_start->next=del->next;
	stack_start->next->prev=stack_start;
	free(del);
	return stack_value;

}

double pop_lf_from_stack(emnt *stack_start){

	/*pops count-many elements from the stack*/
	double stack_value;
	emnt *del;
	del=stack_start->next;
	stack_value=del->double_value;
	stack_start->next=del->next;
	stack_start->next->prev=stack_start;
	free(del->str_value);
	free(del);
	return stack_value;

}

void push_str_into_stack(emnt *stack_start, char *str_value){

	/*pushes a an element of value value into the stack*/
	emnt *new_element=(emnt*) calloc(1,sizeof(emnt));
	new_element->next=stack_start->next;
	stack_start->next=new_element;
	new_element->str_value=(char*) calloc(count_characters(str_value),sizeof(char));
	strcpy(new_element->str_value,str_value);
	new_element->prev=stack_start;
	new_element->next->prev=new_element;
	return;

}
void push_lf_into_stack(emnt *stack_start, double lf_value){

	/*pushes a an element of value value into the stack*/
	emnt *new_element=(emnt*) calloc(1,sizeof(emnt));
	new_element->next=stack_start->next;
	new_element->str_value=(char*) calloc(5,sizeof(char));
	strcpy(new_element->str_value,"0num");
	new_element->double_value=lf_value;
	stack_start->next=new_element;
	new_element->prev=stack_start;
	new_element->next->prev=new_element;
	return;

}

void perform_operation_plus(emnt *operand_stack_start){

	double first,second;
	first=pop_lf_from_stack(operand_stack_start);
	second=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,first+second);
	return;

}

void perform_operation_minus(emnt *operand_stack_start){

	double first,second;
	first=pop_lf_from_stack(operand_stack_start);
	second=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,second-first);
	return;

}

void perform_operation_mult(emnt *operand_stack_start){

	double first,second;
	first=pop_lf_from_stack(operand_stack_start);
	second=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,first*second);
	return;

}

void perform_operation_div(emnt *operand_stack_start){

	double first,second;
	first=pop_lf_from_stack(operand_stack_start);
	second=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,second/first);
	return;

}

void perform_operation_pow(emnt *operand_stack_start){

	double first,second;
	first=pop_lf_from_stack(operand_stack_start);
	second=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,pow(second,first));
	return;


}

void perform_operation_neg(emnt *operand_stack_start){

	double first;
	first=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,(-1)*(first));
	return;

}

void perform_operation_sin(emnt *operand_stack_start){

	double first;
	first=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,sin(first));
	return;

}

void perform_operation_cos(emnt *operand_stack_start){

	double first;
	first=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,cos(first));
	return;

}

void perform_operation_sqrt(emnt *operand_stack_start){

	double first;
	first=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,sqrt(first));
	return;

}

void perform_operation_ln(emnt *operand_stack_start){

	double first;
	first=pop_lf_from_stack(operand_stack_start);
	push_lf_into_stack(operand_stack_start,log(first));
	return;

}

void perform_operation(emnt *operand_stack_start, char *operation){

	if(!strcmp(operation,"+")){
		perform_operation_plus(operand_stack_start);
	}else if(!strcmp(operation,"-")){
		perform_operation_minus(operand_stack_start);
	}else if(!strcmp(operation,"*")){
		perform_operation_mult(operand_stack_start);
	}else if(!strcmp(operation,"/")){
		perform_operation_div(operand_stack_start);
	}else if(!strcmp(operation,"^")){
		perform_operation_pow(operand_stack_start);
	}else if(!strcmp(operation,"~")){
		perform_operation_neg(operand_stack_start);
	}else if(!strcmp(operation,"sin")){
		perform_operation_sin(operand_stack_start);
	}else if(!strcmp(operation,"cos")){
		perform_operation_cos(operand_stack_start);
	}else if(!strcmp(operation,"sqrt")){
		perform_operation_sqrt(operand_stack_start);
	}else if(!strcmp(operation,"ln")){
		perform_operation_ln(operand_stack_start);
	}
	return;

}

emnt *shunting_yard(char *formula){

	/*runs the shunting-yard algorithm on the formula for the given values of the variables*/
	char **input_stack, **input_stack_cursor, *temp;
	int input_stack_index;
	emnt *operator_stack_start, *operand_stack_start;
	input_stack=tokenize_formula(formula);
	input_stack_index=0;
	/*initializes the stacks*/
	operator_stack_start=(emnt*) calloc(1,sizeof(emnt));
	operator_stack_start->next=(emnt*) calloc(1,sizeof(emnt));
	operator_stack_start->next->prev=operator_stack_start;
	operand_stack_start=(emnt*) calloc(1,sizeof(emnt));
	operand_stack_start->next=(emnt*) calloc(1,sizeof(emnt));
	operand_stack_start->next->prev=operand_stack_start;
	operand_stack_start->str_value=(char*) calloc(6,sizeof(char));
	operator_stack_start->str_value=(char*) calloc(6,sizeof(char));
	operand_stack_start->next->str_value=(char*) calloc(4,sizeof(char));
	operator_stack_start->next->str_value=(char*) calloc(4,sizeof(char));
	strcpy(operand_stack_start->str_value,"start");
	strcpy(operator_stack_start->str_value,"start");
	strcpy(operand_stack_start->next->str_value,"end");
	strcpy(operator_stack_start->next->str_value,"end");
	while(strcmp(input_stack[input_stack_index],"end")){
		if(is_letter(*input_stack[input_stack_index])){
			push_str_into_stack(operand_stack_start,input_stack[input_stack_index]);
		}else if(is_number(*input_stack[input_stack_index])){
			push_lf_into_stack(operand_stack_start, strtod(input_stack[input_stack_index],NULL));
		}else if(*input_stack[input_stack_index]=='('){
			push_str_into_stack(operator_stack_start,input_stack[input_stack_index]);
		}else if(*input_stack[input_stack_index]==')'){
			while(*operator_stack_start->next->str_value!='('){
				temp=pop_str_from_stack(operator_stack_start);
				push_str_into_stack(operand_stack_start,temp);
				free(temp);
			}
			temp=pop_str_from_stack(operator_stack_start);
			free(temp);
		}else{
			while(strcmp(operator_stack_start->next->str_value,"end")&&(
			(*input_stack[input_stack_index]!='^' && precedence(*operator_stack_start->next->str_value)>=precedence(*input_stack[input_stack_index]))
			||(*input_stack[input_stack_index]=='^' && precedence(*operator_stack_start->next->str_value)>precedence(*input_stack[input_stack_index])))){
				temp=pop_str_from_stack(operator_stack_start);
				push_str_into_stack(operand_stack_start,temp);
				free(temp);
			}
			push_str_into_stack(operator_stack_start,input_stack[input_stack_index]);
		}
		input_stack_index=input_stack_index+1;
	}
	while(strcmp(operator_stack_start->next->str_value,"end")){
		temp=pop_str_from_stack(operator_stack_start);
		push_str_into_stack(operand_stack_start,temp);
		free(temp);
	}
	for(input_stack_cursor=input_stack;strcmp(*input_stack_cursor,"end");input_stack_cursor++){
		free(*input_stack_cursor);
	}
	free(*input_stack_cursor);
	free(input_stack);
	free(operator_stack_start->next->str_value);
	free(operator_stack_start->next);
	free(operator_stack_start->str_value);
	free(operator_stack_start);
	return operand_stack_start;

}


double evaluate(emnt *postfix_formula, double *variable_value_list){

	double result;
	emnt *cursor, *eval_stack_start;
	eval_stack_start=(emnt*) calloc(1,sizeof(emnt));
	eval_stack_start->next=(emnt*) calloc(1,sizeof(emnt));
	eval_stack_start->next->prev=eval_stack_start;
	eval_stack_start->str_value="start";
	eval_stack_start->next->str_value="end";
	for(cursor=postfix_formula;strcmp(cursor->str_value,"end");cursor=cursor->next);
	cursor=cursor->prev;
	while(strcmp(cursor->str_value,"start")){
		if(is_letter(*cursor->str_value)){
			push_lf_into_stack(eval_stack_start,variable_value_list[*cursor->str_value-'A']);
		}else if(is_number(*cursor->str_value)){
			push_lf_into_stack(eval_stack_start,cursor->double_value);
		}else{
			perform_operation(eval_stack_start,cursor->str_value);
		}
		cursor=cursor->prev;
	}
	result=pop_lf_from_stack(eval_stack_start);
	free(eval_stack_start->next);
	free(eval_stack_start);
	return result;

}

int precedence(char character){

	int precedence_value;
	switch(character){
		case '(':
			precedence_value=-1;
			break;
		case '+':
		case '-':
			precedence_value=0;
			break;
		case '*':
		case '/':
			precedence_value=2;
			break;
		case '^':
			precedence_value=3;
			break;
		default:
			precedence_value=4;
			break;
	}
	return precedence_value;

}

int count_letters(char* input_string){

	/*counts the total # of letters in the formula (or any other string) and returns it*/
	int letter_count;
	char* cursor;
	letter_count=0;
	for(cursor=input_string;*cursor!='\0';cursor++){
		if(is_letter(*cursor)){
			letter_count++;
		}
	}
	return letter_count;

}

int count_letters_unique(char* input_string){

	/*Counts the # of unique uppercase letters in the formula (or any other string) and returns it*/
	int return_count, letters[26], dummy_index_1;
	char *cursor;
	for(dummy_index_1=0;dummy_index_1<26;dummy_index_1++){
		letters[dummy_index_1]=0;
	}
	for(cursor=input_string; *cursor!='\0'; cursor++){
		if(is_letter(*cursor)){
			if(letters[*cursor-'A']==0){
				letters[*cursor-'A']=1;
			}
		}
	}
	return_count=0;
	for(dummy_index_1=0;dummy_index_1<26;dummy_index_1++){
		if(letters[dummy_index_1]==1){
			return_count=return_count+1;
		}
	}
	return return_count;

}

int count_numbers(char* input_string){

	char *cursor;
	int current_is_number, number_count;
	current_is_number=0;
	number_count=0;
	for(cursor=input_string;*cursor!='\0';cursor++){
		if(is_number(*cursor) || *cursor=='.'){
			if(!current_is_number){
				current_is_number=1;
				number_count=number_count+1;
			}
		}else{
			current_is_number=0;
		}
	}
	return number_count;

}

int count_operators(char *input_string){

	/*counts the total # of operators (,),*,/,<function>,+,-,^ */
	/*in the formula (or any other string) and returns it*/
	int operator_count, cursor_increment;
	char *cursor;
	operator_count=0;
	cursor=input_string;
	while(*cursor!='\0'){
		cursor_increment=0;
		switch(*cursor){
			case 's':
				if(*(cursor+1)!='i'){
					cursor_increment=cursor_increment+1;
				}
			case 'c':
				cursor_increment=cursor_increment+1;
			case 'l':
				cursor_increment=cursor_increment+1;
			case '(':
			case ')':
			case '*':
			case '/':
			case '+':
			case '-':
			case '^':
			case '~':
				operator_count=operator_count+1;
			default:
				cursor_increment=cursor_increment+1;
		}
		cursor=cursor+cursor_increment;
	}
	return operator_count;

}

char* remove_spaces(char *input_string){

	/*Creates a new string, copies all non-space and non-endline characters in input_string to the new string and returns it*/
	char *output_string, *cursor;
	int nonspace_count, current_character_count;
	nonspace_count=1;
	for(cursor=input_string;*cursor!='\0';cursor++){
		if(*cursor!=' ' && *cursor!='\n'){
			nonspace_count++;
		}
	}
	output_string=(char*) calloc(nonspace_count,sizeof(char));
	current_character_count=0;
	for(cursor=input_string;*cursor!='\0';cursor++){
		if(*cursor!=' ' && *cursor!='\n'){
			output_string[current_character_count]=*cursor;
			current_character_count++;
		}
	}
	output_string[current_character_count]=*cursor;
	return output_string;

}

int count_characters(char *input_string){

	/*Counts the number of characters in a string, including the \0 at the end*/
	char *cursor;
	for(cursor=input_string;*cursor!='\0';cursor++);
	return cursor-input_string+1;

}

int is_letter(char input_character){

	return ('A'<=input_character && input_character <='Z');

}

int is_number(char input_character){

	return (('0'<=input_character && input_character <='9')||input_character=='.');

}
