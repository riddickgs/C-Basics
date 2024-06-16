Storage classes are used to describe feature of a variable/function.
This feature tells users about storage, initial value, scope, life of variables.
1. auto
	storage class having storage on stack segment, initial value garbage, scope inside block/function, life till end of block/function.
2. static
	storage class having storage on data segment (uninitialized on bss), initial value 0, scope inside block/function, life till end of the program.
3. extern
	storage class having storage on data segment (global variable), initial value 0, scope whole program, life end of program
4. register
	storage class having storage on registers/stack (Not global), initial value garbage, scope inside block/function, life till end of block/function
