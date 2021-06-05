
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef __arm__
#include <wiringPi.h>
#include <wiringPiSPI.h>
#endif
#include "eisl.h"

void initexsubr(void){
    defsubr("RANDOM-REAL",f_random_real);
    defsubr("RANDOM",f_random);
    defsubr("NCONC",f_nconc);
    defsubr("FAST-ADDRESS",f_address);
    defsubr("MACROEXPAND-1",f_macroexpand_1);
    defsubr("MACROEXPAND-ALL",f_macroexpand_all);
    defsubr("BACKTRACE",f_backtrace);
    defsubr("BREAK",f_break);
    defsubr("EDIT",f_edit);
    defsubr("FREEDLL",f_freedll);
    defsubr("SYSTEM",f_system);
    defsubr("SUBRP",f_subrp);
    defsubr("MACROP",f_macrop);
    defsubr("FIXNUMP",f_fixnump);
    defsubr("LONGNUMP",f_longnump);
    defsubr("BIGNUMP",f_bignump);
    defsubr("READED-ARRAY-LIST",f_readed_array_list);
    defsubr("GET-METHOD",f_get_method);
    defsubr("GET-METHOD-BODY",f_get_method_body);
    defsubr("GET-METHOD-PRIORITY",f_get_method_priority);
    defsubr("IGNORE-TOPLEVEL-CHECK",f_ignore_toplevel_check);
    defsubr("SELF-INTRODUCTION",f_self_introduction);
    defsubr("CLASSP",f_classp);
    defsubr("C-INCLUDE",f_ignore);
    defsubr("C-DEFINE",f_ignore);
    defsubr("C-LANG",f_ignore);
    defsubr("C-OPTION",f_ignore);
    defsubr("HEAPDUMP",f_heapdump);
    defsubr("INSTANCE",f_instance);
    defsubr("MODULESUBST",f_modulesubst);
    defsubr("LINE-ARGUMENT",f_line_argument);
    

    #ifdef __NVCC__
    defsubr("GPU-ACCURACY",f_gpu_accuracy); 
    defsubr("GPU-ACTIVATE",f_gpu_activate); 
    defsubr("GPU-ADAGRAD",f_gpu_adagrad);
    defsubr("GPU-ADAM",f_gpu_adam);
    defsubr("GPU-ADD",f_gpu_add);
    defsubr("GPU-ANALIZER",f_gpu_analizer);
    defsubr("GPU-AVERAGE",f_gpu_average);
    defsubr("GPU-COPY",f_gpu_copy);
    defsubr("GPU-CONVOLUTE",f_gpu_convolute);
    defsubr("GPU-CORRECT",f_gpu_correct);
    defsubr("GPU-DECONVOLUTE",f_gpu_deconvolute);
    defsubr("GPU-DIFF",f_gpu_diff);
    defsubr("GPU-DROPOUT",f_gpu_dropout);
    defsubr("GPU-EMULT",f_gpu_emult);
    defsubr("GPU-FULL",f_gpu_full);
    defsubr("GPU-GRADFILTER",f_gpu_gradfilter);
    defsubr("GPU-IDENT",f_gpu_ident);
    defsubr("GPU-LOSS",f_gpu_loss);
    defsubr("GPU-MULT",f_gpu_mult);
    defsubr("GPU-POOLING",f_gpu_pooling);
    defsubr("GPU-RANDOM-SELECT",f_gpu_random_select);
    defsubr("GPU-SGD",f_gpu_sgd);
    defsubr("GPU-SUB",f_gpu_sub);
    defsubr("GPU-SUM",f_gpu_sum);
    defsubr("GPU-SMULT",f_gpu_smult);
    defsubr("GPU-TRACE",f_gpu_trace);
    defsubr("GPU-TRANSPOSE",f_gpu_transpose);
    defsubr("GPU-UNFULL",f_gpu_unfull);
    defsubr("GPU-UNPOOLING",f_gpu_unpooling);
    #endif
}

//Fast Project
int f_classp(int arglist){
	int arg1;
    
    arg1 = car(arglist);
    if(length(arglist) != 1)
    	error(ILLEGAL_ARGS,"classp",arglist);
    
    if(IS_CLASS(arg1))
    	return(T);
    else
    	return(NIL);
}


int f_ignore(int arglist){
    (void)arglist;
    return(T);
}


int f_self_introduction(int arglist){
    (void)arglist;
#if __APPLE__
    return(makesym("MACOS"));
#elif defined(__OpenBSD__)
    return(makesym("OPENBSD"));
#else
    return(makesym("LINUX"));
#endif
}


int f_ignore_toplevel_check(int arglist){
	int arg1;
    
    arg1 = car(arglist);
    if(arg1 == T)
        ignore_topchk = 1;
    else
    	ignore_topchk = 0;
    return(T);
}

int f_get_method_priority(int arglist){
	int arg1;
    
    arg1 = car(arglist);
    if(!(IS_METHOD(arg1)))
    	error(ILLEGAL_ARGS,"get-method-priority",arg1);
        
    return(makeint(GET_OPT(arg1)));
}


int f_get_method_body(int arglist){
	int arg1;
    
    arg1 = car(arglist);
    if(!(IS_METHOD(arg1)))
    	error(ILLEGAL_ARGS,"get-method-body",arg1);
        
    return(GET_CAR(arg1));
}

int f_get_method(int arglist){
	int arg1;
    
    arg1 = car(arglist);
    if(!genericp(arg1))
    	error(ILLEGAL_ARGS,"get-method",arg1);
    return(GET_CDR(GET_CAR(arg1)));
}


int f_readed_array_list(int arglist){
    int arg1;
    
    arg1 = car(arglist);
    return(GET_PROP(arg1));
}


int f_system(int arglist){
    int arg1;

	arg1 = car(arglist);
	if(system(GET_NAME(arg1)) == -1)
		error(SYSTEM_ERR, "system", arg1);
	return(T);
}



int f_freedll(int arglist){
    (void)arglist;  
    //dlclose(hmod);
    return(T);
}



int f_macrop(int arglist){
int arg1;

    arg1 = car(arglist);
    if(length(arglist) != 1)
        error(WRONG_ARGS, "macrop", arglist);
    if(IS_MACRO(GET_CAR(arg1)))
        return(T);
    else
        return(NIL);
}

int f_fixnump(int arglist){
int arg1;
	
    arg1 = car(arglist);
    if(length(arglist) != 1)
        error(WRONG_ARGS, "fixnump", arglist);
    if(IS_INTEGER(arg1))
    	return(T);
    else
    	return(NIL);
}

int f_longnump(int arglist){
    int arg1;
	
    arg1 = car(arglist);
    if(length(arglist) != 1)
        error(WRONG_ARGS, "longnump", arglist);
    if(IS_LONGNUM(arg1))
    	return(T);
    else
    	return(NIL);
}

int f_bignump(int arglist){
    int arg1;
	
    arg1 = car(arglist);
    if(length(arglist) != 1)
        error(WRONG_ARGS, "bignump", arglist);
    if(IS_BIGXNUM(arg1))
    	return(T);
    else
    	return(NIL);
}


int f_subrp(int arglist){
    int arg;
    
    arg = car(arglist);
    if(length(arglist) != 1)
        error(WRONG_ARGS, "subrp", arglist);
    if(IS_SUBR(GET_CAR(arg)))
        return(T);
    else
        return(NIL);
}

int f_random_real(int arglist){
    double d;
    
    if(length(arglist) !=0)
        error(WRONG_ARGS, "random-real", arglist);
    
    d = (double)rand()/RAND_MAX;
    return(makeflt(d));
}

int f_random(int arglist){
    int arg1,n;

    if(length(arglist) !=1)
        error(WRONG_ARGS, "random", arglist);

    arg1 = car(arglist);
    n = GET_INT(arg1);

    return(makeint(rand() % n));
}

int f_nconc(int arglist){
    int arg1,arg2;
    
    arg1 = car(arglist);
    arg2 = cadr(arglist);
    if(length(arglist) != 2)
        error(WRONG_ARGS,"nconc",arglist);
    
    return(nconc(arg1,arg2));
}

int f_address(int arglist){
    int arg1;
    
    arg1 = car(arglist);
    if(length(arglist) != 1)
        error(WRONG_ARGS,"address",arglist);
    
    return(makeint(arg1));
}

int f_macroexpand_1(int arglist){
    int arg1,args;
    
    arg1 = caar(arglist);
    args = cdar(arglist);
    if(length(arglist) != 1)
        error(WRONG_ARGS,"macroexpand-1",arglist);
    if(!symbolp(arg1))
        error(NOT_SYM,"macroexpand-1",arg1);
    
    return(macroexpand_1(arg1,args));
}

int macroexpand_1(int macsym, int args){
    int func,body,macrofunc,varlist,save,res;

    func = GET_CAR(macsym);
    save = ep;
    res = NIL;
    macrofunc = GET_CAR(func);
    varlist = car(GET_CAR(macrofunc));
    if(GET_OPT(func) >= 0){
        if(length(args) != (int)GET_OPT(func))
            error(WRONG_ARGS,"macroexpand-1",args);
    }
    else{
        if(length(args) < (-1 * (int)GET_OPT(func) - 2))
            error(WRONG_ARGS,"macroexpand-1",args);
    }    
    body = cdr(GET_CAR(macrofunc));
    bindarg(varlist,args);
    while(!(IS_NIL(body))){
        res = eval(car(body));
        body = cdr(body);
    }
    unbind();
    ep = save;
    return(res);
}


int f_macroexpand_all(int arglist){
    int arg1;

    arg1 = car(arglist);
    if(length(arglist) != 1)
        error(WRONG_ARGS,"macroexpand-all",arglist);
    if(listp(arg1) && car(arg1) == makesym("DEFMACRO"))
        return(arg1);
    else
        return(macroexpand_all(arg1));
}


int macroexpand_all(int sexp){

    if(nullp(sexp))
        return(NIL);
    else if(atomp(sexp))
        return(sexp);
    else if(listp(sexp) && car(sexp) == makesym("QUOTE"))
        return(sexp);
    else if(listp(sexp) && macrop(car(sexp)))
        return(macroexpand_all(macroexpand_1(car(sexp),cdr(sexp))));
    else if(listp(sexp))
        return(cons(macroexpand_all(car(sexp)),macroexpand_all(cdr(sexp))));
    
    return(NIL);
}


int f_backtrace(int arglist){
    int arg1,l;
    
    if((l=length(arglist)) != 0 && l != 1)
        error(WRONG_ARGS,"backtrace",arglist);
    
    arg1 = car(arglist);
    
    if(l == 0){ 
        int i;

        for(i=0;i<BACKSIZE;i++){
            print(backtrace[i]);
            printf("\n");
        }
    }
    else if(arg1 == T)
        back_flag = 1;
    else if(arg1 == NIL)
        back_flag = 0;
    
    return(T);
}

int f_break(int arglist){
    (void)arglist;
    printf("break\n");
    debugger();
    return(T);
}

int f_instance(int arglist){
    int arg1,addr;

    arg1 = car(arglist);
    addr = get_int(arg1);
    print(addr);
    return(T);
}


int f_modulesubst(int arglist){
    int arg1,arg2;

    arg1 = car(arglist);
    arg2 = cadr(arglist);

    return(modulesubst(arg1,arg2,NIL));
}

int f_line_argument(int arglist)
{
    int arg1, n;
    
    if (length(arglist) != 1) {
        error(WRONG_ARGS, "line-argument", arglist);
    }
    arg1 = car(arglist);
    n = GET_INT(arg1);
    if (n < gArgC) {
        return makestr(gArgV[n]);
    } else {
        return NIL;
    }
}
