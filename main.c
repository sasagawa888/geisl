/* Easy-ISLisp (ISLisp)
written by kenichi sasagawa 2016/4~
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include "eisl.h"

//------pointer----
int ep; //environment pointer
int dp; //dynamic pointer
int hp; //heap pointer  for mark and sweep
int sp; //stack pointer
int fc; //free counter
int ap; //arglist pointer
int lp; //shelter pointer
int wp; //working pointer for copy GC
int ac; //allocate counter


//------class-----
int cobject;
int cbasic_array;
int cbasic_array_star;
int cgeneral_array_star;
int cbasic_vector;
int cgeneral_vector;
int cstring;
int cbuilt_in_class;
int ccharacter;
int cfunction;
int cgeneric_function;
int cstandard_generic_function;
int clist;
int ccons;
int cnull;
int csymbol;
int cnumber;
int cfloat;
int cinteger;
int cserious_condition;
int cerror;
int carithmetic_error;
int cdivision_by_zero;
int cfloating_point_overflow;
int cfloating_point_underflow;
int ccontrol_error;
int cparse_error;
int cprogram_error;
int cdomain_error;
int cclass_error;
int cundefined_entity;
int cunbound_variable;
int cundefined_function;
int csimple_error;
int cstream_error;
int cend_of_stream;
int cstorage_exhausted;
int cstandard_class;
int cstandard_object;
int cstream;
int cinvalid;
int cfixnum;
int clongnum;
int cbignum;
int cfloat_array;


//stream
int standard_input;
int standard_output;
int standard_error;
int input_stream;
int output_stream;
int error_stream;
char stream_str[STRSIZE];
char stream_str1[STRSIZE];
int charcnt; //for format-tab. store number of chars up to now.


//read scaner
token stok = {'\0',GO,OTHER,{'\0'}};
int line;
int column;
int buffer[256][10];
int buffer1[256];


//heap and stack
cell heap[CELLSIZE];
int stack[STACKSIZE];
int argstk[STACKSIZE];
int cell_hash_table[HASHTBSIZE];
int shelter[STACKSIZE];
int dynamic[DYNSIZE][2];

//object oriented
int generic_func; //generic function in eval.
int generic_vars; //args list of generic function in eval.
int next_method; //head address of finded method.
int generic_list = NIL; //symbol list of generic function.

//flag
int gArgC;
char **gArgV;
int gbc_flag = 1; //0=GC not display ,1= do display.
int genint = 1;   //integer of gensym.
int simp_flag = 1; //1=simplify, 0=Not for bignum
int ignore_flag = 0; //0=normal,1=ignore error
int open_flag = 0;  //0=normal,1=now loading
int str_flag = 0;   //0=stdio or FILE, 1=string
int top_flag = 1;   //1=top-level, 0=not-top-level
int redef_flag = 0; //1=redefine-class, 0=not-redefine
int start_flag = 1; //1=line-start, 1=not-line-start
int back_flag = 1;  //for backtrace, 1=on, 0=off
bool init_flag = true;  //for -c option, 1=initial,0=not-initial
int ignore_topchk = 0; //for FAST compiler 1=ignore,0=normal
bool repl_flag = true;  //for REPL read_line 1=on, 0=off
int exit_flag = 0;  //1= ctrl+C
int debug_flag = 0;  //for GC debug
int greeting_flag = 1; //for (quit)
bool script_flag = false;   //for -s option

//switch
int gc_sw = 0;     //0= mark-and-sweep-GC  1= copy-GC
int area_sw = 1;     //1= lower area 2=higher area

//longjmp control and etc
jmp_buf buf;
jmp_buf block_buf[50];
int block_env[50][2];
jmp_buf catch_buf[10][50];
int catch_env[10][50];
jmp_buf ignore_buf; //jump address of ignore-error
int block_tag[CTRLSTK]; //array of tag
int catch_tag[CTRLSTK];
int unwind_buf[CTRLSTK];
int catch_symbols = NIL; //to clear tag data
int block_pt;  //index of block. following are similer
int catch_pt = 0; //catch counter
int unwind_pt; //lambda address for unwind-protect
int block_arg; //recieve argument of block
int catch_arg; //recieve argument of catch
int tagbody_tag = NIL; //tag address fo tagbody
int error_handler; //for store first argument of with-handler
int trace_list = NIL; //function list of trace
int backtrace[BACKSIZE];

//-----debugger-----
int examin_sym;
int stepper_flag = 0;


int ed_lparen_col;
int ed_rparen_col;
char ed_candidate[50][30];
int ed_candidate_pt;
int ed_syntax_color = 1;   //default red
int ed_builtin_color = 6;  //default cyan
int ed_extended_color = 5; //default magenta
int ed_string_color = 3;   //default yellow
int ed_comment_color = 4;  //default blue
int ed_incomment = -1;     // #|...|# comment

//special form token
char special[40][12] = {
{"defun"},{"defmacro"},{"defglobal"},{"defdynamic"},{"defconstant"},
{"let"},{"let*"},{"case"},{"while"},{"progn"}
};
//syntax token
char syntax[60][30] = {
{"lambda"},{"labels"},{"flet"},{"let"},{"let*"},{"setq"},{"setf"},{"defconstant"},{"defun"},{"defmacro"},{"defglobal"},{"defdynamic"},
{"dynamic"},{"function"},{"function*"},{"symbol-function"},{"class"},
{"and"},{"or"},{"if"},{"cond"},{"while"},{"for"},{"block"},{"return-from"},
{"case"},{"case-using"},{"progn"},{"defclass"},{"defgeneric"},{"defgeneric*"},
{"defmethod"},{"dynamic-let"},{"ignore-errors"},{"catch"},{"throw"},
{"tagbody"},{"go"},{"unwind-protect"},{"with-standard-input"},
{"with-standard-output"},{"with-error-output"},{"with-handler"},
{"convert"},{"with-open-input-file"},{"with-open-output-file"},
{"with-open-io-file"},{"the"},{"assure"},{"time"},{"trace"},{"untrace"},{"defmodule"},{"defpublic"},{"substitute"}
};
//builtin token
char builtin[200][32] ={
{"-"},{"*"},{"/="},{"+"},{"<"},{"<="},{"="},{">"},{">="},
{"abs"},{"append"},{"apply"},{"aref"},{"arithmetic-error-operands"},
{"arithmetic-error-operation"},{"array-dimensions"},{"assoc"},{"atan"},
{"atan2"},{"atanh"},{"atom"},{"basic-array-p"},{"basic-array*-p"},
{"basic-vector-p"},{"call-next-method"},{"car"},{"cdr"},{"ceiling"},
{"cerror"},{"char-index"},{"char/="},{"char<"},{"char<="},{"char="},
{"char>"},{"char>="},{"characterp"},{"class-of"},{"close"},
{"condition-continuable"},{"cons"},{"consp"},{"continue-condition"},
{"cos"},{"cosh"},{"create-array"},{"create-list"},{"create-string-input-stream"},
{"create-string-output-stream"},{"create-string"},{"create-vector"},{"create*"},
{"div"},{"domain-error-object"},{"domain-error-expected-class"},
{"dummyp"},{"elt"},{"eq"},{"eql"},{"equal"},{"error-output"},{"error"},
{"eval"},{"exp"},{"expt"},{"file-length"},{"file-position"},{"finish-output"},
{"float"},{"floatp"},{"floor"},{"format-char"},{"format-fresh-line"},
{"format-float"},{"format-integer"},{"format-object"},{"format-tab"},{"format"},
{"funcall"},{"functionp"},{"garef"},{"gbc"},{"gcd"},{"general-array*-p"},
{"general-vector-p"},{"generic-function-p"},{"gensym"},{"get-internal-real-time"},
{"get-internal-run-time"},
{"get-output-stream-string"},{"get-universal-time"},{"hdmp"},{"identity"},
{"initialize-object*"},{"input-stream-p"},{"instancep"},{"integerp"},
{"internal-time-units-per-second"},{"isqrt"},{"lcm"},{"length"},{"list"},
{"listp"},{"load"},{"log"},{"map-into"},{"mapc"},{"mapcar"},{"mapcan"},
{"mapcon"},{"mapl"},{"maplist"},{"max"},{"member"},{"min"},{"mod"},
{"next-method-p"},{"not"},{"nreverse"},{"null"},{"numberp"},
{"open-input-file"},{"open-io-file"},{"open-output-file"},{"open-stream-p"},
{"output-stream-p"},{"parse-error-string"},{"parse-error-expected-class"},
{"parse-number"},{"preview-char"},{"prin1"},{"print"},{"probe-file"},
{"property"},{"quit"},{"quotient"},{"read-byte"},{"read-char"},{"read-line"},
{"read"},{"reciprocal"},{"remove-property"},{"reverse"},{"round"},{"set-aref"},
{"set-car"},{"set-cdr"},{"set-elt"},{"set-file-position"},{"set-garef"},
{"set-property"},{"signal-condition"},{"simple-error-format-argument"},
{"simple-error-format-string"},{"sin"},{"sinh"},{"slot-value"},{"sqrt"},
{"standard-input"},{"standard-output"},{"stream-error-stream"},{"streamp"},
{"stream-ready-p"},{"string-append"},{"string-index"},{"string/="},{"string<"},{"string<="},{"string="},{"string>"},{"string>="},{"stringp"},{"subclassp"},
{"subseq"},{"symbolp"},{"tan"},{"tanh"},{"truncate"},{"undefined-entity-name"},
{"undefined-entity-namespace"},{"vector"},{"write-byte"},{"import"}
};

//extended function
char extended[70][30] = {
{"random-real"},{"random"},{"heapdump"},{"instance"},
{"nconc"},{"fast-address"},{"macroexpand-1"},{"backtrace"},
{"break"},{"edit"},{"set-editor"},{"wiringpi-setup-gpio"},{"delay-microseconds"},
{"wiringpi-spi-setup-ch-speed"},{"pwm-set-mode"},{"pwm-set-range"},
{"pwm-set-clock"},{"pin-mode"},{"digital-write"},{"digital-read"},
{"pwm-write"},{"pull-up-dn-control"},{"delay"},{"compile-file"},{"compile-cuda"},{"formatter"},
{"c-include"},{"c-define"},{"c-lang"},{"c-option"},
{"gpu-mult"},{"gpu-add"},{"gpu-sub"},{"gpu-smult"},{"gpu-emult"},{"gpu-convolute"},{"gpu-deconvolute"},{"gpu-transpose"},
{"gpu-ident"},{"gpu-full"},{"gpu-unfull"},{"gpu-accuracy"},{"gpu-correct"},{"gpu-activate"},{"gpu-trace"},
{"gpu-loss"},{"gpu-average"},{"gpu-sum"},{"gpu-diff"},{"gpu-dropout"},{"gpu-gradfilter"},   
{"gpu-sgd"},{"gpu-momentum"},{"gpu-adagrad"},{"gpu-rms"},{"gpu-adam"},{"gpu-pooling"},{"gpu-unpooling"},
{"gpu-random-select"},{"gpu-nanalizer"},{"gpu-copy"},
};

static void usage(void)
{
    puts("List of options:\n"
         "-c           -- EISL starts after reading compiler.lsp.\n"
         "-f           -- EISL starts after reading formatter.lsp.\n"
         "-h           -- display help.\n"
         "-l filename  -- EISL starts after reading the file.\n"
         "-r           -- EISL does not use editable REPL.\n"
         "-s filename  -- EISL runs the file with script mode.\n"
         "-v           -- display version number.");
}

int main(int argc, char *argv[]){
    int ch;
    char *home,str[256];
    char *script_arg;

    initcell();
    initclass();
    initstream();
    initsubr();
    initexsubr();
    initsyntax();
    initgeneric();
    signal(SIGINT, signal_handler_c);

	
    input_stream = standard_input;
    output_stream = standard_output;
    error_stream = standard_error;
    
    int ret = setjmp(buf);
    if(init_flag){
        init_flag = false;
        FILE* fp = fopen("startup.lsp","r");
        if(fp != NULL){
            fclose(fp);
            f_load(list1(makestr("startup.lsp")));
        }
        while ((ch = getopt(argc, argv, "l:cfs:rhv")) != -1) {
            switch (ch) {
            case 'l':
                f_load(list1(makestr(optarg)));
                break;
            case 'c':
                home = getenv("HOME");
                strcpy(str,home);
                strcat(str,"/geisl/library/compiler.lsp");
                f_load(list1(makestr(str)));
                break;
            case 'f':
                home = getenv("HOME");
                strcpy(str,home);
                strcat(str,"/geisl/library/formatter.lsp");
                f_load(list1(makestr(str)));
                break;
            case 's':
                if (access(optarg, R_OK) == -1) {
                    puts("File doesn't exist.");
                    exit(EXIT_FAILURE);
                }
                repl_flag = false;
                script_flag = true;
                script_arg = optarg;
                break;
            case 'r':
                repl_flag = false;
                break;
            case 'v':
                printf("GEasy-ISLisp Ver%1.2f\n", VERSION);
                exit(EXIT_SUCCESS);
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
            default:
                usage();
                exit(EXIT_FAILURE);
            }
        }
        gArgC = argc - optind;
        gArgV = argv + optind;
        if (script_flag) {
            f_load(list1(makestr(script_arg)));
            exit(EXIT_SUCCESS);
        }
    }
    if(greeting_flag == 1)
        printf("GEasy-ISLisp Ver%1.2f\n", VERSION);
    repl:
    if(ret == 0)
        while(1){
            initpt();
            printf("> "); fflush(stdout);
            print(eval(sread ()));
            printf("\n"); fflush(stdout);
            if(redef_flag)
                redef_generic();
        }
    else
        if(ret == 1){
            ret = 0;
            goto repl;
        }
        else
            return 0;
}

void initpt(void){
    int ls;

    ep = 0;
    sp = 0;
    ap = 0;
    lp = 0;
    ac = 0;
    ls = catch_symbols;
    while(!nullp(ls)){
    	SET_PROP(car(ls),0);
        SET_OPT(car(ls),0);
        ls = cdr(ls);
    }
    block_pt = 0;
    catch_pt = 0;
    unwind_pt = 0;
    error_handler = NIL;
    top_flag = 1;
    start_flag = 1;
    charcnt = 0;
    //clear nest level of tracing function.
    ls = trace_list;
    while(!nullp(ls)){
        SET_TR(GET_CAR(car(ls)),0);
        ls = cdr(ls);
    }
}


void signal_handler_c(int signo){
   (void)signo;
   exit_flag = 1;
}




//-------read()--------
int readc(void){
    int c;
    if(input_stream == standard_input && repl_flag)
        c = read_line(1);
    else if(GET_OPT(input_stream) != EISL_INSTR){
        c = getc(GET_PORT(input_stream));
        //ctrl+D
        //if not script-mode quit system
        if(!script_flag && input_stream == standard_input && c == EOF){
            greeting_flag = 0;
            printf("\n");
            longjmp(buf,2);
        }
        else // if script-mode return(EOF)
            return(c);
        
    }
    else{
        c = GET_NAME(input_stream)[GET_CDR(input_stream)];
        SET_CDR(input_stream,GET_CDR(input_stream)+1);
        if(c == '\\'){
            c = GET_NAME(input_stream)[GET_CDR(input_stream)];
            SET_CDR(input_stream,GET_CDR(input_stream)+1);
         }
        else if(c == NUL){
            c = EOF;
            SET_CDR(input_stream,GET_CDR(input_stream)-1);
        }
    }
    
    if(c == EOL){
        line++;
        column = 0;
    }
    else
        column++;

    return(c);
}

void unreadc(char c){
    if(c == EOL)
        line--;
    else
        column--;
    if(input_stream == standard_input && repl_flag)
        (void)read_line(-1);
    else if(GET_OPT(input_stream) != EISL_INSTR)
        ungetc(c,GET_PORT(input_stream));
    else
        SET_CDR(input_stream, GET_CDR(input_stream)-1);
}



void gettoken(void){
    int c;
    int pos;

    if(stok.flag == BACK){
        stok.flag = GO;
        return;
    }

    if(stok.ch == ')'){
        stok.type = RPAREN;
        stok.ch = NUL;
        return;
    }

    if(stok.ch == '('){
        stok.type = LPAREN;
        stok.ch = NUL;
        return;
    }

    c = readc();

    skip:
    while(c == SPACE || c == EOL || c == TAB || c == RET)
        c=readc();

    //skip comment line
    //if find EOF at end of line, return FILEEND.
    if(c == ';'){
        while(!(c == EOL)){
            c = readc();
            if(c == EOF){
                stok.type = FILEEND;
                return;
            }
        }
        goto skip;
    }
    //if end of file,return FILEEND.
    if(c == EOF){
        stok.type = FILEEND;
        return;
    }

    switch(c){
        case '(':   stok.type = LPAREN; break;
        case ')':   stok.type = RPAREN; break;
        case '\'':  stok.type = QUOTE; break;
        case '.':   stok.type = DOT; break;
        case '`':   stok.type = BACKQUOTE; break;
        case ',':   stok.type = COMMA; break;
        case '@':   stok.type = ATMARK; break;
        case '"':  {c = readc();
                    pos = 0;
                    while(c != '"'){
                        stok.buf[pos++] = c;
                        if(c == '\\'){
                            c = readc();
                            stok.buf[pos++] = c;
                        }
                        if(c == EOF){
                            error(SYSTEM_ERR,"not exist right hand double quote", NIL);
                        }
                        c = readc();
                    }
                    stok.buf[pos] = NUL;
                    stok.type = STRING;
                    break;
                    }
        case '#':  {c=readc();
                    if(c == '\''){
                        stok.type = FUNCTION; break;
                    }
                    else if(c == '(' ){
                        stok.type = VECTOR; break;
                    }
                    else if(c == '\\'){
                        c = readc();
                        pos = 0;
                        stok.buf[pos++] = c;
                        if(c == ' ')
                            goto chskip;

                        while(((c=readc()) != EOL) && (c != EOF) && (pos < BUFSIZE - 1) &&
                                (c != SPACE) && (c != '(') && (c != ')')){
                            stok.buf[pos++] = c;
                        }

                        chskip:
                        stok.buf[pos] = NUL;
                        stok.type = CHARACTER;
                        if(c == EOF)
                            stok.ch = ' ';
                        else
                            stok.ch = c;
                        break;
                    }
                    else if(isdigit(c)){
                        pos = 0;
                        while(isdigit(c)){
                            stok.buf[pos] = c;
                            pos++;
                            c = readc();
                        }
                        stok.buf[pos] = NUL;
                        if(c == 'a' || c =='A'){
                            stok.type = ARRAY;
                            break;
                        }
                        if(c == 'f' || c =='F'){
                            stok.type = FARRAY;
                            break;
                        }
                        else{
                            stok.type = OTHER;
                            return;
                        }
                    }
                    if(c == '|'){
                        reskip:
                        c = readc();
                        while(c != '|'){
                            c = readc();
                        }
                        c = readc();
                        if(c == '#'){
                                c = readc();
                                goto skip;
                        }
                        else
                                goto reskip;
                    }
                    else
                        unreadc(c);
                    c = '#';
                    }
                   /*FALLTHROUGH*/
        default: {
            pos = 0; stok.buf[pos++] = c;
            while(((c=readc()) != EOL) && (c != EOF) && (pos < BUFSIZE - 1) &&
                    (c != SPACE) && (c != '(') && (c != ')') &&
                    (c != '`') && (c != ',') && (c != '@'))
                stok.buf[pos++] = c;

            stok.buf[pos] = NUL;
            stok.ch = c;

            if(flttoken(stok.buf)){
                stok.type = FLOAT_N;
                break;
            }
            //first step,check bignum. inttoken() ignores number of digits.
            if(bignumtoken(stok.buf)){
                stok.type = BIGNUM;
                break;
            }
            if(inttoken(stok.buf)){
                stok.type = INTEGER;
                break;
            }
            if(inttoken_nsgn(stok.buf)){
                stok.type = INTEGER;
                break;
            }
            if(bintoken(stok.buf)){
                stok.type = BINARY;
                break;
            }
            if(octtoken(stok.buf)){
                stok.type = OCTAL;
                break;
            }
            if(dectoken(stok.buf)){
                stok.type = DECNUM;
                break;
            }
            if(hextoken(stok.buf)){
                stok.type = HEXNUM;
                break;
            }
            if(expttoken(stok.buf)){
                stok.type = EXPTNUM;
                break;
            }
            if(symboltoken(stok.buf)){
                stok.type = SYMBOL;
                break;
            }
            stok.type = OTHER;
        }
    }
}

septoken separater(char buf[], char sep){
    int i,j;
    char c;
    septoken res;

    res.sepch = NUL;
    res.after[0] = NUL;

    res.before[0] = buf[0];
    i = 1; j = 1;
    while((c=buf[i]) != NUL)
        if(c == sep){
            res.before[j] = NUL;
            res.sepch = sep;
            i++;
            j = 0;
            while((c=buf[i]) != NUL){
                res.after[j] = c;
                i++;j++;
            }
            res.after[j] = NUL;
        }
        else{
            res.before[j] = c;
            i++; j++;
        }
    return(res);
}

void insertstr(char ch, char buf[]){
    int i;

    i = laststr(buf)+1;
    while(i >= 0){
        buf[i+1] = buf[i];
        i--;
    }
    buf[0] = ch;
}

int laststr(char buf[]){
    int i;

    i = 0;
    while(buf[i] != NUL)
        i++;
    return(i-1);
}


//remove #\ from char, for example #\a -> a.
void dropchar(char buf[]){
    int i,j;

    j = laststr(buf);
    for(i=2; i<=j; i++)
        buf[i-2] = buf[i];
    buf[i-2] = NUL;
}


//integer of sign. ignore number of digits.
int inttoken(char buf[]){
    int i;
    char c;

    if(buf[0] == NUL) // null string
        return(0);

    if(((buf[0] == '+') || (buf[0] == '-'))){
        if(buf[1] == NUL)
            return(0); // case {+,-} => symbol
        i = 1;
        while((c=buf[i]) != NUL)
            if(isdigit(c))
                i++;  // case {+123..., -123...}
            else
                return(0);
    }
    else {
        i = 0;    // {1234...}
        while((c=buf[i]) != NUL)
            if(isdigit(c))
                i++;
            else
                return(0);
    }
    return(1);
}

//integer without sign
int inttoken_nsgn(char buf[]){
    int i;
    char c;

    i = 0;
    while((c=buf[i]) != NUL){
        if(isdigit(c))
            i++;
        else
            return(0);
    }
    return(1);
}

int flttoken(char buf[]){
    septoken tok;

    if(buf[0] == '.'){
        char bufcp[SYMSIZE];
        
        if(buf[1] == '0')
            return(0);
        strcpy(bufcp,buf);
        insertstr('0',bufcp);
        if(flttoken(bufcp))
            return(1);
    }


    tok = separater(buf, '.');

    if(tok.sepch == NUL)
        return(0);

    if(tok.after[0] == NUL) // "".""
        return(0);
    else if(inttoken(tok.before) && inttoken_nsgn(tok.after))
        return(1);
    else
        return(0);
}

int bignumtoken(char buf[]){
    int i;
    char c;

    if(((buf[0] == '+') || (buf[0] == '-'))){
        if(buf[1] == NUL)
            return(0); // case {+,-} => symbol
        i = 1;
        while((c=buf[i]) != NUL)
            if(isdigit(c))
                i++;  // case {+123..., -123...}
            else
                return(0);
        if(strlen(buf) <= 10)
            return(0); //case not bignum
    }
    else {
        i = 0;    // {1234...}
        while((c=buf[i]) != NUL)
            if(isdigit(c))
                i++;
            else
                return(0);
        if(strlen(buf) <= 9)
            return(0); //case not bignum
    }
    return(1); //bignum
}


int symboltoken(char buf[]){
    int i;
    char c;

    i = 0;
    while((c=buf[i]) != NUL)
        if((isalpha(c)) || (isdigit(c)) || (issymch(c)))
            i++;
        else
            return(0);

    //fold to upper letter.
    i = 0;
    while((c=buf[i]) != NUL){
        buf[i] = toupper(c);
        i++;
    }
    return(1);
}

int bintoken(char buf[]){
    int i;
    char c;

    if(!(buf[0] == '#' && (buf[1] == 'b' || buf[1] == 'B')))
        return(0);
    
    if(buf[2] == '+' || buf[2] == '-')
        i = 3;
    else
        i = 2;

    while((c=buf[i]) != NUL)
        if(c == '0' || c == '1')
            i++;
        else
            return(0);
    
    if(i == 3 && (buf[2] == '+' || buf[2] == '-'))
        return(0);
    else if(i != 2){
        dropchar(buf);
        return(1);
    }
    else
        return(0);
}

int octtoken(char buf[]){
    int i;
    char c;

    if(!(buf[0] == '#' && (buf[1] == 'o' || buf[1] == 'O')))
        return(0);
    if(buf[2] == '+' || buf[2] == '-')
        i = 3;
    else
        i = 2;

    while((c=buf[i]) != NUL)
        if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
           c == '5' || c == '6' || c == '7')
            i++;
        else
            return(0);

    if(i == 3 && (buf[2] == '+' || buf[2] == '-'))
        return(0);
    else if(i != 2){
        dropchar(buf);
        return(1);
    }
    else
        return(0);
}

int dectoken(char buf[]){
    int i;
    char c;

    if(!(buf[0] == '#' && (buf[1] == 'd' || buf[1] == 'D')))
        return(0);
    if(buf[2] == '+' || buf[2] == '-')
        i = 3;
    else
        i = 2;

    while((c=buf[i]) != NUL)
        if(isdigit(c))
            i++;
        else
            return(0);

    if(i != 2){
        dropchar(buf);
        return(1);
    }
    else
        return(0);
}

int hextoken(char buf[]){
    int i;
    char c;

    if(!(buf[0] == '#' && (buf[1] == 'x' || buf[1] == 'X')))
        return(0);
    if(buf[2] == '+' || buf[2] == '-')
        i = 3;
    else
        i = 2;

    while((c=buf[i]) != NUL)
        if(isdigit(c) ||
         c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
         c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f')
            i++;
        else
            return(0);

    if(i == 3 && (buf[2] == '+' || buf[2] == '-'))
        return(0);
    else if(i != 2){
        dropchar(buf);
        return(1);
    }
    else
        return(0);
}


int expttoken(char buf[]){
    septoken tok;
    char buf1[BUFSIZE];

    if(buf[0] == '.') // e.g. ".2E3"
        return(0);

    strcpy(buf1,buf);
    tok = separater(buf, 'e');
    if(tok.sepch == NUL)
        goto exit;

    if((inttoken(tok.before)  || flttoken(tok.before)) &&
        inttoken(tok.after)){

        return(1);
    }

    exit:
    strcpy(buf,buf1);
    tok = separater(buf, 'E');
    if(tok.sepch == NUL)
        return(0);
    if((inttoken(tok.before)  || flttoken(tok.before)) &&
        inttoken(tok.after)){

        return(1);
    }
    else
        return(0);
}


int issymch(char c){
    switch(c){
        case '!':
        case '?':
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '<':
        case '>':
        case '_':
        case '.':
        case ':':
        case '#':
        case '$':
        case '@':
        case '%':
        case '&':
        case '~':
        case '^':
        case '|':
        case '\\':
        case '{':
        case '}':
        case '[':
        case ']': return(1);
        default:  return(0);
    }
}


int sread(void){
    int n;
    char *e;

    gettoken();
    switch(stok.type){
        case FILEEND:   return(FEND);
        case INTEGER:   return(makeint(atoi(stok.buf)));
        case FLOAT_N:   return(makeflt(atof(stok.buf)));
        case BIGNUM:    return(makebigx(stok.buf));
        case DECNUM:    return(makeint((int)strtol(stok.buf,&e,10)));
        case BINARY:    return(readbin(stok.buf));
        case OCTAL:     return(readoct(stok.buf));
        case HEXNUM:    return(readhex(stok.buf));
        case EXPTNUM:   return(makeflt(atof(stok.buf)));
        case VECTOR:    return(vector(readlist()));
        case ARRAY:     n = atoi(stok.buf);
                        return(array(n,sread()));
        case FARRAY:     n = atoi(stok.buf);
                        return(farray(n,sread()));
        case STRING:    return(makestr(stok.buf));
        case CHARACTER: return(makechar(stok.buf));
        case SYMBOL:    return(makesym(stok.buf));
        case QUOTE:     return(cons(makesym("QUOTE"), cons(sread(),NIL)));
        case BACKQUOTE: return(cons(makesym("QUASI-QUOTE"), cons(sread(),NIL)));
        case COMMA:    {gettoken();
                        if(stok.type == ATMARK)
                            return(cons(makesym("UNQUOTE-SPLICING"), cons(sread(),NIL)));
                        else{
                            stok.flag = BACK;
                            return(cons(makesym("UNQUOTE"), cons(sread(),NIL)));
                            }
                        }
        case FUNCTION:  return(cons(makesym("FUNCTION"),cons(sread(),NIL)));
        case LPAREN:    return(readlist());
        case RPAREN:    error(ILLEGAL_RPAREN,"read",NIL);
        default:        break;
    }
    fprintf(GET_PORT(error_stream),"%d", (int)stok.type);
    fprintf(GET_PORT(error_stream),"%s", stok.buf);
    error(ILLEGAL_INPUT,"read",NIL);
    return(0);
}

int readlist(void){
    int rl_car,rl_cdr;

    gettoken();
    if(stok.type == RPAREN)
        return(NIL);
    else
    if(stok.type == DOT){
        rl_cdr = sread();
        if(rl_cdr == FEND)
            error(ILLEGAL_RPAREN,"read",makesym("file end"));
        gettoken();
        return(rl_cdr);
    }
    else{
        stok.flag = BACK;
        rl_car = sread();
        if(rl_car == FEND)
            error(ILLEGAL_RPAREN,"read",makesym("file end"));
        rl_cdr = readlist();
        return(cons(rl_car,rl_cdr));
    }
}

int readbin(char* buf){
    char str[BUFSIZE],*e;
    int pos,n,res,inc;

    n = strlen(buf);
    if(n <= 31)
        return(makeint((int)strtol(buf,&e,2)));

    pos = 0;
    res = makeint(0);
    inc = makeint(2);

    while(pos < n){
        int part;
        
        str[0] = buf[pos];
        str[1] = NUL;
        pos++;
        part = makeint((int)strtol(str,&e,2));
        res = plus(mult(res,inc),part);
    }
    return(res);
}

int readoct(char* buf){
    char str[BUFSIZE],*e;
    int pos,n,res,inc;

    n = strlen(buf);
    if(n <= 10)
        return(makeint((int)strtol(buf,&e,8)));

    pos = 0;
    res = makeint(0);
    inc = makeint(8);

    while(pos < n){
        int part;
        
        str[0] = buf[pos];
        str[1] = NUL;
        pos++;
        part = makeint((int)strtol(str,&e,8));
        res = plus(mult(res,inc),part);
    }
    return(res);
}


int readhex(char* buf){
    char str[BUFSIZE],*e;
    int pos,n,res,inc;

    n = strlen(buf);
    if(n <= 7)
        return(makeint((int)strtol(buf,&e,16)));

    pos = 0;
    res = makeint(0);
    inc = makeint(16);

    while(pos < n){
        int part;
        
        str[0] = buf[pos];
        str[1] = NUL;
        pos++;
        part = makeint((int)strtol(str,&e,16));
        res = plus(mult(res,inc),part);
    }
    return(res);
}

//-----print------------------
void print(int addr){
    switch(GET_TAG(addr)){
        case INTN:  printint(addr); break;
        case FLTN:  printflt(GET_FLT(addr)); break;
        case LONGN: printlong(addr); break;
        case BIGX:  print_bigx(addr); break;
        case VEC:   printvec(addr); break;
        case ARR:   printarray(addr); break;
        case FARR:  printfarray(addr); break;
        case STR:   printstr(addr); break;
        case CHR:   printchar(addr); break;
        case SYM:   printsym(addr); break;
        case SUBR:  printobj("<subr>"); break;
        case FSUBR: printobj("<fsubr>"); break;
        case FUNC:  printobj("<function>"); break;
        case MACRO: printobj("<macro>") ;break;
        case CLASS: printclass(addr); break;
        case STREAM:
                    printstream(addr); break;
        case GENERIC:
                    printobj("<generic>"); break;
        case METHOD:
                    printobj("<method>"); break;
        case INSTANCE:
                    printobj("<instance>"); break;
        case LIS:   if(GET_OPT(output_stream) != EISL_OUTSTR)
                        fprintf(GET_PORT(output_stream),"(");
                    else{
                        sprintf(stream_str,"(");
                        strcat(GET_NAME(output_stream),stream_str);
                    }
                    printlist(addr); break;
        case DUMMY: printobj("<undef*>"); break;
        default:    printobj("<undef>"); break;
    }
}

void printint(int addr){
    if(GET_OPT(output_stream) != EISL_OUTSTR)
        fprintf(GET_PORT(output_stream),"%d", GET_INT(addr));
    else{
        sprintf(stream_str,"%d", GET_INT(addr));
        strcat(GET_NAME(output_stream),stream_str);
    }
}

void printflt(double x){
    if(GET_OPT(output_stream) != EISL_OUTSTR){
        if(x - ceil(x) != 0 ||  x >= SMALL_INT_MAX)
            fprintf(GET_PORT(output_stream), "%0.16g", x);
        else
            fprintf(GET_PORT(output_stream), "%0.1f", x);
    }
    else{
        if(x - ceil(x) != 0 ||  x >= SMALL_INT_MAX){
            sprintf(stream_str, "%0.16g", x);
            strcat(GET_NAME(output_stream),stream_str);
        }
        else{
            sprintf(stream_str, "%0.1f", x);
            strcat(GET_NAME(output_stream),stream_str);
        }
    }
}


void printlong(int addr){
    if(GET_OPT(output_stream) != EISL_OUTSTR){
        fprintf(GET_PORT(output_stream),"%lld", GET_LONG(addr));
        sprintf(stream_str,"%lld",GET_LONG(addr));
    }
    else{
	sprintf(stream_str,"%lld", GET_LONG(addr));
        strcat(GET_NAME(output_stream),stream_str);
	}
}


void printlist(int addr){
    if(IS_NIL(addr)){
        if(GET_OPT(output_stream) != EISL_OUTSTR)
            fprintf(GET_PORT(output_stream),")");
        else{
            sprintf(stream_str,")");
            strcat(GET_NAME(output_stream),stream_str);
        }
    }
    else if((!(listp(cdr(addr)))) && (! (nullp(cdr(addr))))){
        print(car(addr));
        if(GET_OPT(output_stream) != EISL_OUTSTR)
            fprintf(GET_PORT(output_stream)," . ");
        else{
            sprintf(stream_str," . ");
            strcat(GET_NAME(output_stream),stream_str);
        }
        print(cdr(addr));
        if(GET_OPT(output_stream) != EISL_OUTSTR)
            fprintf(GET_PORT(output_stream),")");
        else{
            sprintf(stream_str,")");
            strcat(GET_NAME(output_stream),stream_str);
        }
    }
    else{
        print(GET_CAR(addr));
        if(!(IS_NIL(GET_CDR(addr)))){
            if(GET_OPT(output_stream) != EISL_OUTSTR)
                fprintf(GET_PORT(output_stream)," ");
            else{
                sprintf(stream_str," ");
                strcat(GET_NAME(output_stream),stream_str);
            }
        }
        printlist(GET_CDR(addr));
    }
}

void printvec(int x){
    int len,i;

    if(GET_OPT(output_stream) != EISL_OUTSTR)
        fprintf(GET_PORT(output_stream), "#(");
    else{
        sprintf(stream_str, "#(");
        strcat(GET_NAME(output_stream),stream_str);
    }
    len = cdr(x);

    for(i=0; i<len; i++){
        print(GET_VEC_ELT(x,i));
        if(i != len-1){
            if(GET_OPT(output_stream) != EISL_OUTSTR)
                fprintf(GET_PORT(output_stream), " ");
            else{
                sprintf(stream_str, " ");
                strcat(GET_NAME(output_stream),stream_str);
            }
        }
    }
    if(GET_OPT(output_stream) != EISL_OUTSTR)
        fprintf(GET_PORT(output_stream), ")");
    else{
        sprintf(stream_str, " ");
        strcat(GET_NAME(output_stream),stream_str);
    }
}

void printarray(int x){
    int i,size,st,ls,dim;

    st = ls = GET_CDR(x);
    size = 1;
    dim = length(ls);
    while(!nullp(ls)){
        size = GET_INT(car(ls)) * size;
        ls = cdr(ls);
    }
    ls = NIL;
    for(i=0;i<size;i++)
        ls = cons(GET_VEC_ELT(x,i),ls);
    ls = reverse(ls);
    if(GET_OPT(output_stream) != EISL_INSTR)
        fprintf(GET_PORT(output_stream),"#%da",dim);
    else{
        sprintf(stream_str,"#%da",dim);
        strcat(GET_NAME(output_stream),stream_str);
    }
    if(dim == 0)
        print(car(ls));
    else
        print(structured(ls,st));
}

#define IDX2C(i,j,ld) (((j)*(ld))+(i))
#define IDX2R(i,j,ld) (((i)*(ld))+(j))
void printfarray(int x){
    int i,size,st,ls,dim;
    float *vec1;

    st = ls = GET_CDR(x);
    size = 1;
    dim = length(ls);
    while(!nullp(ls)){
        size = GET_INT(car(ls)) * size;
        ls = cdr(ls);
    }
    ls = NIL;
    if(length(st) == 2){
        if(size < 100){
            int j,r,c;
            float *vec2;
            
            vec1 = GET_FVEC(x);
            vec2 = (float *)malloc(sizeof(float)*size);
            r = GET_INT(car(st));
            c = GET_INT(cadr(st));
            for(i=0;i<r;i++)
                for(j=0;j<c;j++)
                    vec2[IDX2R(i,j,c)] = vec1[IDX2C(i,j,r)];
            for(i=0;i<size;i++)
                ls = cons(makeflt(vec2[i]),ls);
            free(vec2);
        }
        else{
            ls = cons(makesym("float-elements"),ls);
        }
    }
    else{
         if(size < 100){
            for(i=0;i<size;i++)
                ls = cons(makeflt(GET_FVEC_ELT(x,i)),ls);
        }
        else{
            ls = cons(makesym("float-element"),ls);
        }
    }
    ls = reverse(ls);
    if(GET_OPT(output_stream) != EISL_INSTR)
        fprintf(GET_PORT(output_stream),"#%df",dim);
    else{
        sprintf(stream_str,"#%df",dim);
        strcat(GET_NAME(output_stream),stream_str);
    }
    if(dim == 0)
        print(car(ls));
    else if(size < 100)
        print(structured(ls,st));
    else 
        print(ls);
}

void printstr(int addr){
    if(GET_OPT(output_stream) != EISL_OUTSTR){
        fprintf(GET_PORT(output_stream),"\"");
        fprintf(GET_PORT(output_stream),"%s", GET_NAME(addr));
        fprintf(GET_PORT(output_stream),"\"");
    }
    else{
        sprintf(stream_str,"\"");
        strcat(GET_NAME(output_stream),stream_str);
        sprintf(stream_str,"%s", GET_NAME(addr));
        strcat(GET_NAME(output_stream),stream_str);
        sprintf(stream_str,"\"");
        strcat(GET_NAME(output_stream),stream_str);
    }
}


void printchar(int addr){
    char c;

    if(GET_PORT(input_stream)){
        fprintf(GET_PORT(output_stream),"%c%c", '#', '\\');
        c = GET_CHAR(addr);
        if(c == SPACE)
            fprintf(GET_PORT(output_stream),"space");
        else if(c == EOL)
            fprintf(GET_PORT(output_stream),"newline");
        else
            fprintf(GET_PORT(output_stream),"%s", GET_NAME(addr));
    }
    else{
        sprintf(GET_NAME(output_stream),"%c%c", '#', '\\');
        c = GET_CHAR(addr);
        if(c == SPACE){
            sprintf(stream_str,"space");
            strcat(GET_NAME(output_stream),stream_str);
        }
        else if(c == EOL){
            sprintf(stream_str,"newline");
            strcat(GET_NAME(output_stream),stream_str);
        }
        else{
            sprintf(stream_str,"%s", GET_NAME(addr));
            strcat(GET_NAME(output_stream),stream_str);
        }
    }
}




void printsym(int addr){

    if(GET_OPT(output_stream) != EISL_OUTSTR){
        fprintf(GET_PORT(output_stream), "%s", GET_NAME(addr));
    }
    else{
        sprintf(stream_str, "%s", GET_NAME(addr));
        strcat(GET_NAME(output_stream),stream_str);
    }
}

void printobj(const char *str){
    if(GET_OPT(output_stream) != EISL_OUTSTR)
        fprintf(GET_PORT(output_stream), "%s", str);
    else{
        sprintf(stream_str, "%s", str);
        strcat(GET_NAME(output_stream),stream_str);
    }
}

void printclass(int addr){
    if(GET_OPT(output_stream) != EISL_OUTSTR)
        fprintf(GET_PORT(output_stream), "<class %s>", GET_NAME(addr));
    else{
        sprintf(stream_str, "<class %s>", GET_NAME(addr));
        strcat(GET_NAME(output_stream),stream_str);
    }
}

void printstream(int addr){
    if(GET_OPT(output_stream) != EISL_OUTSTR)
        fprintf(GET_PORT(output_stream), "<stream %s>", GET_NAME(addr));
    else{
        sprintf(GET_NAME(output_stream), "<stream %s>", GET_NAME(addr));
        strcat(GET_NAME(output_stream),stream_str);
    }
}

static void clean_stdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

//--------eval---------------
int eval(int addr){
    int val,res,temp;
    char c;

    (void)checkgbc();

    if(IS_NIL(addr) || IS_T(addr))
        return(addr);
    else if(numberp(addr))
        return(addr);
    else if(vectorp(addr))
        return(addr);
    else if(arrayp(addr))
        return(addr);
    else if(farrayp(addr))
        return(addr);
    else if(stringp(addr))
        return(addr);
    else if(charp(addr))
        return(addr);
    else if(symbolp(addr)){
        res = findenv(addr);
        if(res != FAILSE)
            return(res);
        else{
            if(GET_OPT(addr) == GLOBAL)
                return(GET_CDR(addr));
            else if(GET_OPT(addr) == CONSTN)
                return(GET_CDR(addr));
            else
                error(UNDEF_VAR, "eval", addr);

        }
    }
    else if(listp(addr)){
        if(back_flag)
            store_backtrace(addr);
        if(stepper_flag){
        	print(addr);printf("\n");fflush(stdout);
        	clean_stdin();
        	c = getc(stdin);
        	if(c == 'q')
        		debugger();
    	}

        if((symbolp(car(addr))) &&(HAS_NAME(car(addr),"QUOTE"))){
            if(improperlistp(cdr(addr)))
                error(ILLEGAL_ARGS,"quote",cdr(addr));
            else if(length(cdr(addr)) != 1)
                error(ILLEGAL_ARGS,"quote",cdr(addr));
            else
                return(cadr(addr));
        }
        else if((symbolp(car(addr))) &&(HAS_NAME(car(addr),"QUASI-QUOTE"))){
            temp = quasi_transfer(cadr(addr),0);
            shelterpush(temp);
            res = eval(temp);
            shelterpop();
            return(res);
        }
        else if(subrp(car(addr)))
            return(apply(caar(addr),evlis(cdr(addr))));
        else if(fsubrp(car(addr)))
            return(apply(caar(addr),cdr(addr)));
        else if((val=functionp(car(addr)))){
            temp = evlis(cdr(addr));
            examin_sym = car(addr);
            return(apply(val,temp));
        }
        else if(macrop(car(addr))){
            examin_sym = car(addr);
            return(apply(caar(addr),cdr(addr)));
        }
        else if(genericp(car(addr))){
            examin_sym = car(addr);
            return(apply(caar(addr),evlis(cdr(addr))));
        }
        else if(listp(car(addr)))
            return(apply(eval(car(addr)),evlis(cdr(addr))));
        else
            error(UNDEF_FUN, "eval", car(addr));
        
    }
    error(UNDEF_FUN, "eval", addr);
    return(0);
}

int apply(int func, int args){
    int varlist,body,res,macrofunc,method,pexist,aexist,i,n,trace;
    res = NIL;
    pexist = 0;
    aexist = 0;
    trace = 0;

    switch(GET_TAG(func)){
        case SUBR:  return((GET_SUBR(func))(args));
        case FSUBR: return((GET_SUBR(func))(args));
        case FUNC:  if(GET_TR(examin_sym) == 1){
                        trace = examin_sym;
                        n = GET_TR(func);
                        SET_TR(func,n+1);
                        for(i=0; i<n; i++)
                            printf(" ");
                        printf("ENTERING: ");
                        print(trace);
                        printf(" ");
                        print(args);
                        printf("\n");
                    }
                    push(ep);
                    ep = GET_CDR(func);
                    varlist = car(GET_CAR(func));
                    if(GET_OPT(func) >= 0){
                        if(length(args) != (int)GET_OPT(func))
                            error(WRONG_ARGS,GET_NAME(func),args);
                    }
                    else{
                        if(length(args) < (-1 * (int)GET_OPT(func) - 2))
                            error(WRONG_ARGS,GET_NAME(func),args);
                    }
                    body = cdr(GET_CAR(func));
                    bindarg(varlist,args);
                    while(!(IS_NIL(body))){
                        res = eval(car(body));
                        body = cdr(body);
                    }
                    unbind();
                    if(trace != NIL){
                        n = GET_TR(func);
                        n = n-1;
                        SET_TR(func,n);
                        for(i=0; i<n; i++)
                            printf(" ");
                        printf("EXITING:  ");
                        print(trace);
                        printf(" ");
                        print(res);
                        printf("\n");
                    }
                    ep = pop();
                    return(res);
        case MACRO:{if(improperlistp(args))
                        error(IMPROPER_ARGS, "apply", args);
                    macrofunc = GET_CAR(func);
                    varlist = car(GET_CAR(macrofunc));
                    if(GET_OPT(func) >= 0){
                        if(length(args) != (int)GET_OPT(func))
                            error(WRONG_ARGS,GET_NAME(func),args);
                    }
                    else{
                        if(length(args) < (-1 * (int)GET_OPT(func) - 2))
                            error(WRONG_ARGS,GET_NAME(func),args);
                    }
                    body = cdr(GET_CAR(macrofunc));
                    bindarg(varlist,args);
                    while(!(IS_NIL(body))){
                        res = eval(car(body));
                        body = cdr(body);
                    }
                    unbind();
                    res = eval(res);
                    return(res);
                    }

        case GENERIC:{
                    if(GET_OPT(func) >= 0){
                        if(length(args) != (int)GET_OPT(func))
                            error(WRONG_ARGS,GET_NAME(func),args);
                    }
                    else{
                        if(length(args) < (-1 * (int)GET_OPT(func) - 2))
                            error(WRONG_ARGS,GET_NAME(func),args);
                    }
                    generic_func = func;
                    generic_vars = args;
                    method = GET_CDR(func);
                    while(!nullp(method)){
                        varlist = car(GET_CAR(car(method)));
                        next_method = method;
                        if(matchp(varlist,args)){
                            if(GET_OPT(car(method)) == PRIORITY)
                                pexist = 1;
                            if(GET_OPT(car(method)) == AROUND)
                                aexist = 1;
                            varlist = genlamlis_to_lamlis(varlist);
                            body = cdr(GET_CAR(car(method)));
                            bindarg(varlist,args);
                            while(!nullp(body)){
                                res = eval(car(body));
                                body = cdr(body);
                            }
                            unbind();
                        }
                        if(GET_OPT(car(next_method)) == AROUND){
                            if(aexist==1)
                                goto exit;
                            else
                                method = cdr(method);
                        }
                        else if(GET_OPT(car(method)) == PRIORITY && pexist==1){
                            method = next_method;
                            while(GET_OPT(car(method)) == PRIORITY){
                                method = cdr(method);
                            }
                        }
                        else
                            method = cdr(next_method);

                    }
                    if(pexist==0)
                        error(NOT_EXIST_METHOD, "apply", args);

                    exit:
                    generic_func = NIL;
                    generic_vars = NIL;
                    return(res);
                    }
        default:    error(NOT_FUNC, "apply", list2(func,args));
    }
    return(0);
}

void bindarg(int varlist, int arglist){
    int arg1,arg2;

    push(ep);
    while(!(IS_NIL(varlist))){
        if(cddr(varlist) == NIL &&
          (car(varlist) == makesym(":REST") || car(varlist) == makesym("&REST"))){
            arg1 = cadr(varlist);
            arg2 = arglist;
            addlexenv(arg1,arg2);
            return;
        }
        else{
            arg1 = car(varlist);
            arg2 = car(arglist);
            addlexenv(arg1,arg2);
            varlist = cdr(varlist);
            arglist = cdr(arglist);
        }
    }
}

void unbind(void){
    ep = pop();
}


int evlis(int addr){
    argpush(addr);
    top_flag = 0;
    if(IS_NIL(addr)){
        argpop();
        return(addr);
    }
    else{
    	int car_addr,cdr_addr;

        car_addr = eval(car(addr));
        argpush(car_addr);
        cdr_addr = evlis(cdr(addr));
        car_addr = argpop();
        (void)argpop();
        return(cons(car_addr,cdr_addr));
    }
}
/*
check class matching of argument of lambda and recieved argument.
*/
int matchp(int varlist, int arglist){

    if(nullp(varlist) && nullp(arglist))
        return(1);
    else if(symbolp(car(varlist)))
        return(matchp(cdr(varlist),cdr(arglist)));
    else if(eqp(makesym(":rest"),car(varlist)))
        return(1);
    else if(eqp(makesym("&rest"),car(varlist)))
        return(1);
    else if(GET_AUX(cadar(varlist))==GET_AUX(car(arglist))) //match class
        return(matchp(cdr(varlist),cdr(arglist)));
    else if(subclassp(GET_AUX(car(arglist)),GET_AUX(cadar(varlist)))) //subclass
        return(matchp(cdr(varlist),cdr(arglist)));
    else
        return(0);
}

/*
change lambda list of function to normal argument.
ex ((x <number>)(y <list>)) -> (x y)
*/
int genlamlis_to_lamlis(int varlist){
    int res;

    res = NIL;
    while(!nullp(varlist)){
        if(symbolp(car(varlist)))
            res = cons(car(varlist),res);
        else
            res = cons(caar(varlist),res);

        varlist = cdr(varlist);
    }
    return(reverse(res));
}


//for stack to store ep(environment)
int push(int pt){
    if(sp >= STACKSIZE)
    	error(STACK_OVERF, "push", NIL);
    stack[sp++] = pt;

    return(T);
}

int pop(void){
    if(sp <= 0)
    	error(STACK_UNDERF, "pop", NIL);
    return(stack[--sp]);
}

//push/pop of arglist
int argpush(int addr){
    argstk[ap++] = addr;

    return(T);
}

int argpop(void){
    return(argstk[--ap]);
}

//shelter push/pop
int shelterpush(int addr){
    if(lp >= STACKSIZE)
    	error(SHELTER_OVERF, "shelterpush", NIL);
    shelter[lp++] = addr;

    return(T);
}

int shelterpop(void){
    if(lp <= 0)
    	error(SHELTER_UNDERF, "shelterpop", NIL);
    return(shelter[--lp]);
}

//--------system function
//regist subr to environment.
void defsubr(const char *symname, int(*func)(int)){
    bindfunc(symname, SUBR, func);
}
//regist fsubr(not eval argument)
void deffsubr(const char *symname, int(*func)(int)){
    bindfunc(symname, FSUBR, func);
}


void bindfunc(const char *name, tag tag, int(*func)(int)){
    int sym,val;

    sym = makesym(name);
    val = freshcell();
    SET_TAG(val,tag);
    SET_SUBR(val,func);
    SET_CDR(val,0);
    SET_AUX(val,cfunction); //class function
    SET_CAR(sym,val);
}

void bindmacro(char *name, int addr){
    int sym,val1,val2;
    char *str;

    sym = makesym(name);
    val1 = freshcell();
    SET_TAG(val1,FUNC);
    SET_CAR(val1,addr);
    SET_CDR(val1,0);
    val2 = freshcell();
    SET_TAG(val2,MACRO);
    str = (char *)malloc(strlen(name)+1);
    if(str == NULL)
        error(MALLOC_OVERF,"makemacro",NIL);
    heap[val2].name = str;
    strcpy(heap[val2].name,name);
    SET_CAR(val2,val1);
    SET_CDR(val2,0);
    SET_AUX(val2,cfunction); //class
    SET_OPT(val2,(signed char)count_args(car(addr))); //count of args
    SET_CAR(sym,val2);
}

void bindconst(const char *name, int obj){
    int sym;

    sym = makesym(name);
    SET_CDR(sym,obj);
    SET_OPT(sym,CONSTN);
}

//--------qusi quote---------------
int quasi_transfer(int x, int n){
    //printf("%d",n); print(x);printf("\n");

    if(nullp(x))
        return(NIL);
    else if(atomp(x))
        return(list2(makesym("QUOTE"),x));
    else if(listp(x) && eqp(car(x),makesym("UNQUOTE")) && n==0)
        return(cadr(x));
    else if(listp(x) && eqp(car(x),makesym("UNQUOTE-SPLICING")) && n==0)
        return(cadr(x));
    else if(listp(x) && eqp(car(x),makesym("QUASI-QUOTE")))
        return(list3(makesym("LIST"),
                     list2(makesym("QUOTE"),makesym("QUASI-QUOTE")),
                     quasi_transfer(cadr(x),n+1)));
    else if(listp(x) && eqp(caar(x),makesym("UNQUOTE")) && n==0)
        return(list3(makesym("CONS"), cadar(x), quasi_transfer(cdr(x),n)));
    else if(listp(x) && eqp(caar(x),makesym("UNQUOTE-SPLICING")) && n==0)
        return(list3(makesym("APPEND"), cadar(x) , quasi_transfer(cdr(x),n)));
    else if(listp(x) && eqp(caar(x),makesym("UNQUOTE")))
        return(list3(makesym("CONS"),
                     list3(makesym("LIST"),list2(makesym("QUOTE"),makesym("UNQUOTE")),quasi_transfer(cadar(x),n-1)),
                     quasi_transfer(cdr(x),n)));
    else if(listp(x) && eqp(caar(x),makesym("UNQUOTE-SPLICING")))
        return(list3(makesym("CONSc"),
                     list3(makesym("LIST"),list2(makesym("QUOTE"),makesym("UNQUOTE-SPLICING")),quasi_transfer(cadar(x),n-1)),
                     quasi_transfer(cdr(x),n)));
    else
        return(list3(makesym("CONS"), quasi_transfer(car(x),n), quasi_transfer(cdr(x),n)));
}

//--------debug---------------
void debugger(){
	int i,x;

    printf("debug mode ?(help)\n");
    loop:
    printf(">>");fflush(stdout);
    x = sread();
	if(eqp(x,makesym("?"))){
    	printf("?  help\n");
        printf(":a abort\n");
        printf(":b backtrace\n");
        printf(":d dynamic environment\n");
        printf(":e environment\n");
        printf(":i identify examining symbol\n");
        printf(":q quit\n");
        printf(":r room\n");
        printf(":s stepper ON/OFF\n");
        printf("other S exps eval\n");
    }
    else if(eqp(x,makesym(":A"))){
        longjmp(buf,1); 
    }
    else if(eqp(x,makesym(":B"))){
    	for(i=0;i<BACKSIZE;i++){
            print(backtrace[i]);
            printf("\n");
    	}
    }
    else if(eqp(x,makesym(":D"))){
        #ifdef DYN
        for(i=1;i<=dp;i++){
            print(dynamic[i][1]);
            printf(" = ");
            print(dynamic[1][1]);
            printf("\n");
        }
        #else
    	print(dp);
        printf("\n");
        #endif
    }
    else if(eqp(x,makesym(":E"))){
    	print(ep);
        printf("\n");
    }
    else if(eqp(x,makesym(":I"))){
    	print(examin_sym);
        printf("\n");
    }
    else if(eqp(x,makesym(":Q"))){
    	return;
    }
    else if(eqp(x,makesym(":R"))){
        printf("EP = %d (environment pointer)\n", ep);
    	printf("DP = %d (dynamic pointer)\n", dp);
        printf("HP = %d (heap pointer)\n", hp);
        printf("SP = %d (stack pointer)\n", sp);
        printf("FC = %d (free counter)\n", fc);
        printf("AP = %d (arglist pointer)\n", ap);
        printf("LP = %d (shelter pointer)\n", lp);
        printf("GC = %d (GC switch 0=m&sGC 1=copyGC)\n", gc_sw);
        printf("WP = %d (work area pointer)\n", wp);
        printf("SW = %d (current work area 1or2)\n", area_sw);
    }
    else if(eqp(x,makesym(":S"))){
    	if(stepper_flag == 0){
        	printf("stepper ON. enter 'q' to quit stepper\n");
            stepper_flag = 1;
        }
        else{
        	printf("stepper OFF\n");
            stepper_flag = 0;
        }
    }
    else{
    	print(eval(x));
        printf("\n");
    }
    goto loop;
}
