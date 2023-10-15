.PHONY : test
test : clean eval
	./eval "1+0.0"
	./eval "NON(1+0)+NON"
	./eval "NON NON 1+0"
	./eval "1=>(NON (1+0).1)"
	./eval "1+a" 
	
debug : eval
	valgrind --leak-check=full ./eval "(1+0)"
	
eval : eval.c
	gcc -Wall -g -o eval eval.c
	
#pour la question 6, facultative
priorite : clean evalprio
	./eval_prio "1+0.0"
	./eval_prio "(1+0).0"
	./eval_prio "1 => NON 1 <=> 0+1"
	./eval_prio "((1 => NON 1) <=> 0)+1"
	./eval_prio "1=>NON NON(1+0.0)" 
	./eval_prio "1=>NON NON((1+0).0)"
	
debugp : evalprio
	valgrind --leak-check=full ./eval_prio "(1+0)"
	
evalprio : eval.c
	gcc -Wall -g -o eval_prio eval_prio.c

.PHONY: clean
clean :
	rm -f eval
	rm -f eval_prio
	ls -l
