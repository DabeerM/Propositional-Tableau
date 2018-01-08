#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */

int Fsize=50;
int cases=10;


int i;
int j;

struct tableau {
  char *root; //i.e. the formula
  struct  tableau *left;
  struct tableau *right;
  struct tableau *parent;
};

int bin_con_pos(char * g){ //Gets the position of the binary connective
	int bin_count = 0;
	int i, j, bin_con = 0;
	for (i = 0; i < strlen(g); i++){
		if ((bin_count==1)&&(g[i]=='v'||g[i]=='^'||g[i]=='>')){ //Binary connective can only occur when: # of '(' > # of ')'
			bin_con = i;
		}
		if (g[i]=='('){bin_count++;}
		else if (g[i]==')'){bin_count--;}
		if (bin_con>0){return bin_con;}
	}
	return 0;
}

int parse(char *g) //Checks if formula is valid recursively by
{
	if ((strlen(g)==1)&&((g[0]=='p')||(g[0]=='q')||(g[0]=='r'))){return 1;} //i.e. proposition

    else if ((g[0]=='-')&&(parse(g+1)>0)){return 2;} //i.e. negation

    else if ((g[0]=='(')&&(g[strlen(g)-1]==')')){ //i.e. binary connective
		int bin_con = bin_con_pos(g);
		if (bin_con>0){
            //Makes 2 strings of maximum length 100 to store the 2 parts of the binary forumula
			char part1[100];
			char part2[100];
            //Adding the halves of the formulas to each string.
			int x = 0, y = 0;
			while (x<bin_con-1){
				part1[x] = g[x+1];
				x++;
			}
			part1[bin_con-1]='\0';
			while (y<strlen(g)-(bin_con+1)-1){
				part2[y]=g[bin_con+1+y];
				y++;
			}
			part2[strlen(g)-(bin_con+1)-1] = '\0';
            //recursively checking the rest of the formula
			if ((parse(part1)>0)&&(parse(part2)>0)){return 3;}
		}
	}
	return 0;
	//Check which type of formula it is by checking the first character. If it starts w/: '(' it's binary connective, 'E' Existential, 'A' Universal, '-' Negative & 'X' Atomic
	//See if it's a valid formula recursively
	/* return 1 if proposition, 2 if  neg, 3 if binary ow 0*/
}

char * spliceString(char * fullstring, int start, int end){
    char newstr[end-start];
    char * new_str = (char*)malloc(sizeof(newstr));
    for (int i = start; i < end; i++){
        new_str[i-start] = fullstring[i];
    }
    return new_str;
}

int isAlpha(char * formula){ //Returns if a formula is an alpha formula or not.
    char bin_connective = formula[bin_con_pos(formula)];
    if (((formula[0]=='(' && bin_connective=='^')||(formula[0]=='-' && bin_connective=='v')||(formula[0]=='-' && bin_connective=='>')||(formula[0]=='-' && formula[1]=='-'))){
        return 1;
    }
    return 0;
}

int isBeta(char * formula){
    char bin_connective = formula[bin_con_pos(formula)];
    if ((strlen(formula)>3)&&((formula[0]=='(' && bin_connective=='v')||(formula[0]=='(' && bin_connective=='>')||(formula[0]=='-' && bin_connective=='^'))){
        return 1;
    }
    return 0;
}

int isLeaf(struct tableau * cur_node){
    return ((cur_node->right==NULL) && (cur_node->left==NULL));
}

void addBeta(struct tableau * node, struct tableau * tab1, struct tableau * tab2){ //Adds a beta formula onto the tableau. Does this by traversing the tableau and adding onto leaf nodes.
    if (node->left != NULL){
        addBeta(node->left, tab1, tab2);
    }
    if (node->right != NULL){
        addBeta(node->right, tab1, tab2);
    }
    if (isLeaf(node)){
        node->left = tab1;
        node->right = tab2;
    }
}

void addAlpha(struct tableau * node, struct tableau * tab_to_add){ //adds an alpha formula to the current tableau
    if (node->left != NULL){
        addAlpha(node->left, tab_to_add);
    }
    if (node->right != NULL){
        addAlpha(node->right, tab_to_add);
    }
    if (isLeaf(node)){
        node->left = tab_to_add;
    }
}

char * notStr(char * my_str){
    char newStr[strlen(my_str)+1];
    char * new_str = (char*)malloc(sizeof(newStr));
    new_str[0] = '-';
    for (int i = 1; i < (strlen(my_str)+1); i++){
        new_str[i] = my_str[i-1];
    }
    return new_str;
}

void complete(struct tableau *t){
    char * formula = t->root;
    //printf("Working on this one currently: %s\n", formula);
    char * formula1 = "";
    char * formula2 = "";
    int bin_con = bin_con_pos(formula); //gives the position of the binary connective
    if (isAlpha(formula)){
        if (formula[0]=='-' && formula[1]=='-'){ //i.e. formula is of form ¬¬formula
            formula1 = spliceString(formula, 2, strlen(formula));
            struct tableau *newTab1 = (struct tableau *) malloc(sizeof(struct tableau));
            newTab1->right = NULL;
            newTab1->left = NULL;
            newTab1->parent = t;
            newTab1->root = formula1;
            addAlpha(t, newTab1);
        }
        else{
            if (formula[0]=='('){ //i.e. (form1^form2)
                formula1 = spliceString(formula,1,bin_con);
                formula2 = spliceString(formula,bin_con+1,strlen(formula)-1);
            }
            else if (formula[bin_con]=='v'){ //i.e. the formula is of the form ¬(formula1vformula2)
                formula1 = notStr(spliceString(formula,2,bin_con)); //i.e.¬(form1)
                formula2 = notStr(spliceString(formula, bin_con+1, strlen(formula)-1)); //'-' ++ formula[bin_con+1:]
            }
            else{ //i.e. ¬(formula1->formula2)
                formula1 = spliceString(formula, 2, bin_con); //formula[2:bin_con];
                formula2 = notStr(spliceString(formula, bin_con+1,strlen(formula)-1)); //'-' ++ formula[bin_con+1:]
            }
            //Making new tableaus
            struct tableau *newTab1 = (struct tableau *) malloc(sizeof(struct tableau));
            struct tableau *newTab2 = (struct tableau *) malloc(sizeof(struct tableau));

            newTab1->parent = t;
            newTab1->root = formula1;
            newTab1->right = NULL;
            newTab1->left = newTab2;
            newTab2->parent = newTab1;
            newTab2->root = formula2;
            newTab2->right = NULL;
            newTab2->left = NULL;
            addAlpha(t, newTab1);
        }
    }
    else if (isBeta(formula)){ //i.e. a beta formula
        if (formula[bin_con]=='^'){ //i.e. ¬(form1 ^ form2)
            formula1 = notStr(spliceString(formula,2,bin_con)); // '-' ++ formula[2:bin_con];
            formula2 = notStr(spliceString(formula, bin_con+1, strlen(formula)-1)); //'-' ++ formula[bin_con+1:];
        }
        else if (formula[bin_con]=='>'){
            formula1 = notStr(spliceString(formula, 1, bin_con)); //'-' ++ formula[1:bin_con];
            formula2 = spliceString(formula, bin_con+1, strlen(formula)-1);
        }
        else{ //i.e. (formula1 v formula2)
            formula1 = spliceString(formula, 1, bin_con);
            formula2 = spliceString(formula, bin_con+1, strlen(formula)-1);
        }

        //Adding new left and right tableaus
        struct tableau *newTab1 = (struct tableau *) malloc(sizeof(struct tableau));
        struct tableau *newTab2 = (struct tableau *) malloc(sizeof(struct tableau));

        newTab1->parent = t;
        newTab1->root = formula1;
        newTab1->right = NULL;
        newTab1->left = NULL;
        newTab2->parent = t;
        newTab2->root = formula2;
        newTab2->right = NULL;
        newTab2->left = NULL;
        //Adding values on
        addBeta(t, newTab1, newTab2);
    }
    //Change up the logic for this so that it's called twice for beta formula.
    if (t->left != NULL){
        complete(t->left);
    }
    if (t->right != NULL){
        complete(t->right);
    }
}

/*
void printout(struct tableau * t){ //prints out the tableau
    if (!(t->parent==NULL)){printf("%s's parent is: %s\n", t->root, t->parent->root);}
    else{printf("%s\n", t->root);}
    if (t->left!=NULL){
        printout(t->left);
    }
    if (t->right!=NULL){
        printout(t->right);
    }
}
*/

int isProposition(char * formula){
    if ((strlen(formula)==1 && (formula[0]=='p'||formula[0]=='q'||formula[0]=='r')) || (strlen(formula)==2 && (formula[0]=='-'))){return 1;}
    return 0;
}

int evaluate(struct tableau * cur_tab, int literals[]){
    char * formula = cur_tab->root;
    int my_list[6];
    memcpy(my_list, literals, 6 * sizeof(int));
    //Adding it to my list if the current formula is a proposition
    if (isProposition(formula)){
        if (strlen(formula)==1){ //i.e. a single proposition => p | q | r
            switch (formula[0]) {
                case 'p': my_list[0] = 1; break;
                case 'q': my_list[1] = 1; break;
                case 'r': my_list[2] = 1; break;
            }
        }
        else{ //i.e. ¬proposition => ¬p | ¬q | ¬r
            switch (formula[1]) {
                case 'p': my_list[3] = 1; break;
                case 'q': my_list[4] = 1; break;
                case 'r': my_list[5] = 1; break;
            }
        }
    }
    //Checking my_list for any contradictions
    for (int i = 0; i < 3; i++){if ((my_list[i]==1)&&(my_list[i+3]==1)){return 1;}}
    //Evaluate the rest of the child tableaus
    if ((cur_tab->left != NULL) && (cur_tab->right == NULL)){return evaluate(cur_tab->left, my_list);}
    if ((cur_tab->left != NULL) && (cur_tab->right != NULL)){return (evaluate(cur_tab->right, my_list) && evaluate(cur_tab->left, my_list));}
    return 0; //i.e. if you're at a leaf node, return True as we'd have already checked if there's a contradiction
}

int closed(struct tableau * tab){
    //Note that the array stores the number of instances of [p,q,r,¬p,¬q,¬r]. => [0,0,0,0,0,0] means there aren't any instances of any propositions
    int prop_count[6] = {0,0,0,0,0,0};
    return evaluate(tab, prop_count);
}

int main(){ /*input a string and check if its a propositional formula */
  char *name = malloc(Fsize);
  FILE *fp, *fpout;

  /* reads from input.txt, writes to output.txt*/
  if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
  if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}

  int j;
  for(j=0;j<cases;j++)
    {
    fscanf(fp, "%s",name);/*read formula*/
    switch (parse(name))
	{
        case(0): fprintf(fpout, "%s is not a formula.  ", name);break;
    	case(1): fprintf(fpout, "%s is a proposition.  ", name);break;
    	case(2): fprintf(fpout, "%s is a negation.  ", name);break;
    	case(3):fprintf(fpout, "%s is a binary.  ", name);break;
    	default:fprintf(fpout, "What the f***!  "); 
	}

    /*make new tableau with name at root, no children, no parent*/

    struct tableau t={name, NULL, NULL, NULL};

    /*expand the root, recursively complete the children*/
    if (parse(name)!=0)
    	{
            complete(&t);
    	    if (closed(&t)) fprintf(fpout, "%s is not satisfiable.\n", name);
    	    else fprintf(fpout, "%s is satisfiable.\n", name);
    	}
    else fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }

  fclose(fp);
  fclose(fpout);
  free(name);

  return(0);
}
